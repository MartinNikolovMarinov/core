#pragma once

#include <core_bits.h>
#include <core_cstr.h>
#include <core_expected.h>
#include <core_ints.h>
#include <core_traits.h>
#include <core_types.h>

#include <math/core_math.h>

// TODO2: [PERFORMACE] Everything in this file can be much faster.

namespace core {

using namespace coretypes;

enum struct ParseError : u8 {
    None,
    InputEmpty,
    InputHasMultipleDots,
    InputHasInvalidSymbol,
    InputNumberTooLarge, // will overflow return type
};
constexpr const char* parseErrorToCstr(ParseError err) {
    switch (err) {
        case ParseError::InputEmpty:             return "Input is empty";
        case ParseError::InputHasMultipleDots:   return "Input more than one dot";
        case ParseError::InputHasInvalidSymbol:  return "Input contains an invalid symbol";
        case ParseError::InputNumberTooLarge:    return "Input number is too large to fit in return type";
        case ParseError::None: break;
    }
    return "Unknown";
}

constexpr u32  intToCstr(u32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(u64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i32 n, char* out, addr_size outMax, u32 digits = 0);
constexpr u32  intToCstr(i64 n, char* out, addr_size outMax, u32 digits = 0);
constexpr void intToHex(u8 v, char* out, u64 hexLen = (sizeof(u8) << 1));
constexpr void intToHex(u16 v, char* out, u64 hexLen = (sizeof(u16) << 1));
constexpr void intToHex(u32 v, char* out, u64 hexLen = (sizeof(u32) << 1));
constexpr void intToHex(u64 v, char* out, u64 hexLen = (sizeof(u64) << 1));
constexpr void intToHex(i8 v, char* out, u64 hexLen = (sizeof(i8) << 1));
constexpr void intToHex(i16 v, char* out, u64 hexLen = (sizeof(i16) << 1));
constexpr void intToHex(i32 v, char* out, u64 hexLen = (sizeof(i32) << 1));
constexpr void intToHex(i64 v, char* out, u64 hexLen = (sizeof(i64) << 1));

template <typename TInt> constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen);

// FIXME: Replace these when the ryu implementation is ready ---
constexpr u32 floatToCstr(f32 n, char* out, addr_size outMax, u32 precision = 6);
constexpr u32 floatToCstr(f64 n, char* out, addr_size outMax, u32 precision = 6);
// FIXME: TO HERE ---

template <typename TFloat> constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen);

namespace detail {

template<typename TInt>
constexpr u32 intToCstr(TInt n, char* out, addr_size outMax, u32 digits) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");
    Assert(out);

    u32 idx = 0;

    auto safeAppend = [&idx, out, outMax](char c) {
        Assert(addr_size(idx) < outMax);
        out[idx++] = c;
    };

    if constexpr (core::is_signed_v<TInt>) {
        if (n < 0) {
            safeAppend('-');
            n = -n;
        }
    }

    i32 dc = (digits == 0) ? i32(digitCount(n)) : i32(digits);
    for (i32 i = dc - 1; i >= 0; i--) {
        // There is a lot of believe in all this static casting, but it 'should not' be dangerous.
        TInt curr = static_cast<TInt>((n / static_cast<TInt>(pow10(u32(i))))) % 10;
        safeAppend(digitToChar(curr));
        dc--;
    }

    return idx;
}

} // detail namespace

constexpr u32 intToCstr(u32 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(u64 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(i32 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }
constexpr u32 intToCstr(i64 n, char* out, addr_size outMax, u32 digits) { return detail::intToCstr(n, out, outMax, digits); }

namespace detail {

template <typename TFloat>
constexpr u32 floatToCstr(TFloat n, char* out, addr_size outMax, u32 precision) {
    static_assert(core::is_float_v<TFloat>, "TFloat must be a floating point type.");
    Assert(out);

    if constexpr (sizeof(TFloat) == 4) {
        Assert(precision < 8, "Precision must be less than 8 for float.");
    }
    else {
        Assert(precision < 16, "Precision must be less than 16 for double.");
    }

    u32 idx = 0;

    const auto safeAppend = [&idx, out, outMax](char c) {
        Assert(addr_size(idx) < outMax);
        out[idx++] = c;
    };

    if (core::isnan(n)) {
        safeAppend('n');
        safeAppend('a');
        safeAppend('n');
        return idx;
    }

    if (core::isinf(n)) {
        if (n < 0) safeAppend('-');
        safeAppend('i');
        safeAppend('n');
        safeAppend('f');
        return idx;
    }

    if (n < 0) {
        safeAppend('-');
        n = -n;
    }

    i64 intPart = i64(n);
    u64 fracMultiplier =  core::pow10(precision);
    TFloat fracPartFloat = (n - TFloat(intPart)) * TFloat(fracMultiplier);

    // Banker's rounding:
    i64 fracPart;
    TFloat roundPart = fracPartFloat - TFloat(i64(fracPartFloat));
    if (roundPart > TFloat(0.5)) {
        fracPart = i64(fracPartFloat) + 1;
    }
    else if (roundPart < TFloat(0.5)) {
        fracPart = i64(fracPartFloat);
    }
    else { // roundPart == 0.5, tie breaker.
        fracPart = i64(fracPartFloat);
        if (fracPart & 1) {
            fracPart++;
        }
    }

    // Handle overflow from rounding.
    if (fracPart >= i64(fracMultiplier)) {
        intPart++;
        fracPart = 0;
    }

    // Convert the integer part.
    idx += detail::intToCstr(intPart, out + idx, outMax, 0);
    if (precision > 0) {
        safeAppend('.');

        // Prepend zeroes if the fractional part is less than the precision.
        u32 zeroesToPrepend = precision - core::digitCount(fracPart);
        for (u32 i = 0; i < zeroesToPrepend; i++) {
            safeAppend('0');
        }

        // Convert the fractional part.
        u32 prev = idx;
        idx += detail::intToCstr(fracPart, out + idx, outMax, 0);
        u32 currentPrecision = (idx - prev) + zeroesToPrepend;

        // Add trailing zeroes if the precision is not reached.
        for (u32 i = currentPrecision; i < precision; i++) {
            safeAppend('0');
        }
    }

    return idx;
}

} // detail

constexpr u32 floatToCstr(f32 n, char* out, addr_size outMax, u32 precision) { return detail::floatToCstr(n, out, outMax, precision); }
constexpr u32 floatToCstr(f64 n, char* out, addr_size outMax, u32 precision) { return detail::floatToCstr(n, out, outMax, precision); }

// This function does not handle TInt overflows!
template <typename TInt>
constexpr core::expected<TInt, ParseError> cstrToInt(const char* s, u32 slen) {
    static_assert(core::is_integral_v<TInt>, "TInt must be an integral type.");

    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    u32 i = 0;
    [[maybe_unused]] bool neg = false;
    if constexpr (core::is_signed_v<TInt>) {
        neg = s[i] == '-';
        if (neg) i++;
    }

    TInt res = 0;
    while (i < slen) {
        char curr = s[i];
        if (!isDigit(curr)) {
            return core::unexpected(ParseError::InputHasInvalidSymbol);
        }

        TInt next = static_cast<TInt>(res * 10) + core::toDigit<TInt>(curr);
        if (next < res) {
            return core::unexpected(ParseError::InputNumberTooLarge);
        }
        res = next;
        i++;
    }

    if constexpr (core::is_signed_v<TInt>) {
        if (neg) res = -res;
    }

    return res;
}

namespace detail {

static constexpr const char* hexDigits = "0123456789ABCDEF";

// The out argument must have enough space to hold the result!
template <typename TInt>
constexpr void intToHex(TInt v, char* out, u64 hexLen) {
    for (size_t i = 0, j = (hexLen - 1) * 4; i < hexLen; i++, j-=4) {
        out[i] = detail::hexDigits[(v >> j) & 0x0f];
    }
}

} // namespace detail

constexpr void intToHex(u8 v, char* out, u64 hexLen)  { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u16 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u32 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(u64 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i8 v, char* out, u64 hexLen)  { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i16 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i32 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }
constexpr void intToHex(i64 v, char* out, u64 hexLen) { detail::intToHex(v, out, hexLen); }

#pragma region Cptr to float

// FIXME: Add a comment giving credit to the creator of the algorithm.

namespace detail {

constexpr u64 FLOAT_POW5_INV_SPLIT[55] = {
    576460752303423489u, 461168601842738791u, 368934881474191033u, 295147905179352826u,
    472236648286964522u, 377789318629571618u, 302231454903657294u, 483570327845851670u,
    386856262276681336u, 309485009821345069u, 495176015714152110u, 396140812571321688u,
    316912650057057351u, 507060240091291761u, 405648192073033409u, 324518553658426727u,
    519229685853482763u, 415383748682786211u, 332306998946228969u, 531691198313966350u,
    425352958651173080u, 340282366920938464u, 544451787073501542u, 435561429658801234u,
    348449143727040987u, 557518629963265579u, 446014903970612463u, 356811923176489971u,
    570899077082383953u, 456719261665907162u, 365375409332725730u, 292300327466180584u,
    467680523945888934u, 374144419156711148u, 299315535325368918u, 478904856520590269u,
    383123885216472215u, 306499108173177772u, 490398573077084435u, 392318858461667548u,
    313855086769334039u, 502168138830934462u, 401734511064747569u, 321387608851798056u,
    514220174162876889u, 411376139330301511u, 329100911464241209u, 526561458342785934u,
    421249166674228747u, 336999333339382998u, 539198933343012796u, 431359146674410237u,
    345087317339528190u, 552139707743245103u, 441711766194596083u
};

constexpr u32 DOUBLE_POW5_INV_TABLE_SIZE = 342;
constexpr u64 DOUBLE_POW5_INV_SPLIT[DOUBLE_POW5_INV_TABLE_SIZE][2] = {
    {                    1u, 2305843009213693952u }, { 11068046444225730970u, 1844674407370955161u },
    {  5165088340638674453u, 1475739525896764129u }, {  7821419487252849886u, 1180591620717411303u },
    {  8824922364862649494u, 1888946593147858085u }, {  7059937891890119595u, 1511157274518286468u },
    { 13026647942995916322u, 1208925819614629174u }, {  9774590264567735146u, 1934281311383406679u },
    { 11509021026396098440u, 1547425049106725343u }, { 16585914450600699399u, 1237940039285380274u },
    { 15469416676735388068u, 1980704062856608439u }, { 16064882156130220778u, 1584563250285286751u },
    {  9162556910162266299u, 1267650600228229401u }, {  7281393426775805432u, 2028240960365167042u },
    { 16893161185646375315u, 1622592768292133633u }, {  2446482504291369283u, 1298074214633706907u },
    {  7603720821608101175u, 2076918743413931051u }, {  2393627842544570617u, 1661534994731144841u },
    { 16672297533003297786u, 1329227995784915872u }, { 11918280793837635165u, 2126764793255865396u },
    {  5845275820328197809u, 1701411834604692317u }, { 15744267100488289217u, 1361129467683753853u },
    {  3054734472329800808u, 2177807148294006166u }, { 17201182836831481939u, 1742245718635204932u },
    {  6382248639981364905u, 1393796574908163946u }, {  2832900194486363201u, 2230074519853062314u },
    {  5955668970331000884u, 1784059615882449851u }, {  1075186361522890384u, 1427247692705959881u },
    { 12788344622662355584u, 2283596308329535809u }, { 13920024512871794791u, 1826877046663628647u },
    {  3757321980813615186u, 1461501637330902918u }, { 10384555214134712795u, 1169201309864722334u },
    {  5547241898389809503u, 1870722095783555735u }, {  4437793518711847602u, 1496577676626844588u },
    { 10928932444453298728u, 1197262141301475670u }, { 17486291911125277965u, 1915619426082361072u },
    {  6610335899416401726u, 1532495540865888858u }, { 12666966349016942027u, 1225996432692711086u },
    { 12888448528943286597u, 1961594292308337738u }, { 17689456452638449924u, 1569275433846670190u },
    { 14151565162110759939u, 1255420347077336152u }, {  7885109000409574610u, 2008672555323737844u },
    {  9997436015069570011u, 1606938044258990275u }, {  7997948812055656009u, 1285550435407192220u },
    { 12796718099289049614u, 2056880696651507552u }, {  2858676849947419045u, 1645504557321206042u },
    { 13354987924183666206u, 1316403645856964833u }, { 17678631863951955605u, 2106245833371143733u },
    {  3074859046935833515u, 1684996666696914987u }, { 13527933681774397782u, 1347997333357531989u },
    { 10576647446613305481u, 2156795733372051183u }, { 15840015586774465031u, 1725436586697640946u },
    {  8982663654677661702u, 1380349269358112757u }, { 18061610662226169046u, 2208558830972980411u },
    { 10759939715039024913u, 1766847064778384329u }, { 12297300586773130254u, 1413477651822707463u },
    { 15986332124095098083u, 2261564242916331941u }, {  9099716884534168143u, 1809251394333065553u },
    { 14658471137111155161u, 1447401115466452442u }, {  4348079280205103483u, 1157920892373161954u },
    { 14335624477811986218u, 1852673427797059126u }, {  7779150767507678651u, 1482138742237647301u },
    {  2533971799264232598u, 1185710993790117841u }, { 15122401323048503126u, 1897137590064188545u },
    { 12097921058438802501u, 1517710072051350836u }, {  5988988032009131678u, 1214168057641080669u },
    { 16961078480698431330u, 1942668892225729070u }, { 13568862784558745064u, 1554135113780583256u },
    {  7165741412905085728u, 1243308091024466605u }, { 11465186260648137165u, 1989292945639146568u },
    { 16550846638002330379u, 1591434356511317254u }, { 16930026125143774626u, 1273147485209053803u },
    {  4951948911778577463u, 2037035976334486086u }, {   272210314680951647u, 1629628781067588869u },
    {  3907117066486671641u, 1303703024854071095u }, {  6251387306378674625u, 2085924839766513752u },
    { 16069156289328670670u, 1668739871813211001u }, {  9165976216721026213u, 1334991897450568801u },
    {  7286864317269821294u, 2135987035920910082u }, { 16897537898041588005u, 1708789628736728065u },
    { 13518030318433270404u, 1367031702989382452u }, {  6871453250525591353u, 2187250724783011924u },
    {  9186511415162383406u, 1749800579826409539u }, { 11038557946871817048u, 1399840463861127631u },
    { 10282995085511086630u, 2239744742177804210u }, {  8226396068408869304u, 1791795793742243368u },
    { 13959814484210916090u, 1433436634993794694u }, { 11267656730511734774u, 2293498615990071511u },
    {  5324776569667477496u, 1834798892792057209u }, {  7949170070475892320u, 1467839114233645767u },
    { 17427382500606444826u, 1174271291386916613u }, {  5747719112518849781u, 1878834066219066582u },
    { 15666221734240810795u, 1503067252975253265u }, { 12532977387392648636u, 1202453802380202612u },
    {  5295368560860596524u, 1923926083808324180u }, {  4236294848688477220u, 1539140867046659344u },
    {  7078384693692692099u, 1231312693637327475u }, { 11325415509908307358u, 1970100309819723960u },
    {  9060332407926645887u, 1576080247855779168u }, { 14626963555825137356u, 1260864198284623334u },
    { 12335095245094488799u, 2017382717255397335u }, {  9868076196075591040u, 1613906173804317868u },
    { 15273158586344293478u, 1291124939043454294u }, { 13369007293925138595u, 2065799902469526871u },
    {  7005857020398200553u, 1652639921975621497u }, { 16672732060544291412u, 1322111937580497197u },
    { 11918976037903224966u, 2115379100128795516u }, {  5845832015580669650u, 1692303280103036413u },
    { 12055363241948356366u, 1353842624082429130u }, {   841837113407818570u, 2166148198531886609u },
    {  4362818505468165179u, 1732918558825509287u }, { 14558301248600263113u, 1386334847060407429u },
    { 12225235553534690011u, 2218135755296651887u }, {  2401490813343931363u, 1774508604237321510u },
    {  1921192650675145090u, 1419606883389857208u }, { 17831303500047873437u, 2271371013423771532u },
    {  6886345170554478103u, 1817096810739017226u }, {  1819727321701672159u, 1453677448591213781u },
    { 16213177116328979020u, 1162941958872971024u }, { 14873036941900635463u, 1860707134196753639u },
    { 15587778368262418694u, 1488565707357402911u }, {  8780873879868024632u, 1190852565885922329u },
    {  2981351763563108441u, 1905364105417475727u }, { 13453127855076217722u, 1524291284333980581u },
    {  7073153469319063855u, 1219433027467184465u }, { 11317045550910502167u, 1951092843947495144u },
    { 12742985255470312057u, 1560874275157996115u }, { 10194388204376249646u, 1248699420126396892u },
    {  1553625868034358140u, 1997919072202235028u }, {  8621598323911307159u, 1598335257761788022u },
    { 17965325103354776697u, 1278668206209430417u }, { 13987124906400001422u, 2045869129935088668u },
    {   121653480894270168u, 1636695303948070935u }, {    97322784715416134u, 1309356243158456748u },
    { 14913111714512307107u, 2094969989053530796u }, {  8241140556867935363u, 1675975991242824637u },
    { 17660958889720079260u, 1340780792994259709u }, { 17189487779326395846u, 2145249268790815535u },
    { 13751590223461116677u, 1716199415032652428u }, { 18379969808252713988u, 1372959532026121942u },
    { 14650556434236701088u, 2196735251241795108u }, {   652398703163629901u, 1757388200993436087u },
    { 11589965406756634890u, 1405910560794748869u }, {  7475898206584884855u, 2249456897271598191u },
    {  2291369750525997561u, 1799565517817278553u }, {  9211793429904618695u, 1439652414253822842u },
    { 18428218302589300235u, 2303443862806116547u }, {  7363877012587619542u, 1842755090244893238u },
    { 13269799239553916280u, 1474204072195914590u }, { 10615839391643133024u, 1179363257756731672u },
    {  2227947767661371545u, 1886981212410770676u }, { 16539753473096738529u, 1509584969928616540u },
    { 13231802778477390823u, 1207667975942893232u }, {  6413489186596184024u, 1932268761508629172u },
    { 16198837793502678189u, 1545815009206903337u }, {  5580372605318321905u, 1236652007365522670u },
    {  8928596168509315048u, 1978643211784836272u }, { 18210923379033183008u, 1582914569427869017u },
    {  7190041073742725760u, 1266331655542295214u }, {   436019273762630246u, 2026130648867672343u },
    {  7727513048493924843u, 1620904519094137874u }, {  9871359253537050198u, 1296723615275310299u },
    {  4726128361433549347u, 2074757784440496479u }, {  7470251503888749801u, 1659806227552397183u },
    { 13354898832594820487u, 1327844982041917746u }, { 13989140502667892133u, 2124551971267068394u },
    { 14880661216876224029u, 1699641577013654715u }, { 11904528973500979224u, 1359713261610923772u },
    {  4289851098633925465u, 2175541218577478036u }, { 18189276137874781665u, 1740432974861982428u },
    {  3483374466074094362u, 1392346379889585943u }, {  1884050330976640656u, 2227754207823337509u },
    {  5196589079523222848u, 1782203366258670007u }, { 15225317707844309248u, 1425762693006936005u },
    {  5913764258841343181u, 2281220308811097609u }, {  8420360221814984868u, 1824976247048878087u },
    { 17804334621677718864u, 1459980997639102469u }, { 17932816512084085415u, 1167984798111281975u },
    { 10245762345624985047u, 1868775676978051161u }, {  4507261061758077715u, 1495020541582440929u },
    {  7295157664148372495u, 1196016433265952743u }, {  7982903447895485668u, 1913626293225524389u },
    { 10075671573058298858u, 1530901034580419511u }, {  4371188443704728763u, 1224720827664335609u },
    { 14372599139411386667u, 1959553324262936974u }, { 15187428126271019657u, 1567642659410349579u },
    { 15839291315758726049u, 1254114127528279663u }, {  3206773216762499739u, 2006582604045247462u },
    { 13633465017635730761u, 1605266083236197969u }, { 14596120828850494932u, 1284212866588958375u },
    {  4907049252451240275u, 2054740586542333401u }, {   236290587219081897u, 1643792469233866721u },
    { 14946427728742906810u, 1315033975387093376u }, { 16535586736504830250u, 2104054360619349402u },
    {  5849771759720043554u, 1683243488495479522u }, { 15747863852001765813u, 1346594790796383617u },
    { 10439186904235184007u, 2154551665274213788u }, { 15730047152871967852u, 1723641332219371030u },
    { 12584037722297574282u, 1378913065775496824u }, {  9066413911450387881u, 2206260905240794919u },
    { 10942479943902220628u, 1765008724192635935u }, {  8753983955121776503u, 1412006979354108748u },
    { 10317025513452932081u, 2259211166966573997u }, {   874922781278525018u, 1807368933573259198u },
    {  8078635854506640661u, 1445895146858607358u }, { 13841606313089133175u, 1156716117486885886u },
    { 14767872471458792434u, 1850745787979017418u }, {   746251532941302978u, 1480596630383213935u },
    {   597001226353042382u, 1184477304306571148u }, { 15712597221132509104u, 1895163686890513836u },
    {  8880728962164096960u, 1516130949512411069u }, { 10793931984473187891u, 1212904759609928855u },
    { 17270291175157100626u, 1940647615375886168u }, {  2748186495899949531u, 1552518092300708935u },
    {  2198549196719959625u, 1242014473840567148u }, { 18275073973719576693u, 1987223158144907436u },
    { 10930710364233751031u, 1589778526515925949u }, { 12433917106128911148u, 1271822821212740759u },
    {  8826220925580526867u, 2034916513940385215u }, {  7060976740464421494u, 1627933211152308172u },
    { 16716827836597268165u, 1302346568921846537u }, { 11989529279587987770u, 2083754510274954460u },
    {  9591623423670390216u, 1667003608219963568u }, { 15051996368420132820u, 1333602886575970854u },
    { 13015147745246481542u, 2133764618521553367u }, {  3033420566713364587u, 1707011694817242694u },
    {  6116085268112601993u, 1365609355853794155u }, {  9785736428980163188u, 2184974969366070648u },
    { 15207286772667951197u, 1747979975492856518u }, {  1097782973908629988u, 1398383980394285215u },
    {  1756452758253807981u, 2237414368630856344u }, {  5094511021344956708u, 1789931494904685075u },
    {  4075608817075965366u, 1431945195923748060u }, {  6520974107321544586u, 2291112313477996896u },
    {  1527430471115325346u, 1832889850782397517u }, { 12289990821117991246u, 1466311880625918013u },
    { 17210690286378213644u, 1173049504500734410u }, {  9090360384495590213u, 1876879207201175057u },
    { 18340334751822203140u, 1501503365760940045u }, { 14672267801457762512u, 1201202692608752036u },
    { 16096930852848599373u, 1921924308174003258u }, {  1809498238053148529u, 1537539446539202607u },
    { 12515645034668249793u, 1230031557231362085u }, {  1578287981759648052u, 1968050491570179337u },
    { 12330676829633449412u, 1574440393256143469u }, { 13553890278448669853u, 1259552314604914775u },
    {  3239480371808320148u, 2015283703367863641u }, { 17348979556414297411u, 1612226962694290912u },
    {  6500486015647617283u, 1289781570155432730u }, { 10400777625036187652u, 2063650512248692368u },
    { 15699319729512770768u, 1650920409798953894u }, { 16248804598352126938u, 1320736327839163115u },
    {  7551343283653851484u, 2113178124542660985u }, {  6041074626923081187u, 1690542499634128788u },
    { 12211557331022285596u, 1352433999707303030u }, {  1091747655926105338u, 2163894399531684849u },
    {  4562746939482794594u, 1731115519625347879u }, {  7339546366328145998u, 1384892415700278303u },
    {  8053925371383123274u, 2215827865120445285u }, {  6443140297106498619u, 1772662292096356228u },
    { 12533209867169019542u, 1418129833677084982u }, {  5295740528502789974u, 2269007733883335972u },
    { 15304638867027962949u, 1815206187106668777u }, {  4865013464138549713u, 1452164949685335022u },
    { 14960057215536570740u, 1161731959748268017u }, {  9178696285890871890u, 1858771135597228828u },
    { 14721654658196518159u, 1487016908477783062u }, {  4398626097073393881u, 1189613526782226450u },
    {  7037801755317430209u, 1903381642851562320u }, {  5630241404253944167u, 1522705314281249856u },
    {   814844308661245011u, 1218164251424999885u }, {  1303750893857992017u, 1949062802279999816u },
    { 15800395974054034906u, 1559250241823999852u }, {  5261619149759407279u, 1247400193459199882u },
    { 12107939454356961969u, 1995840309534719811u }, {  5997002748743659252u, 1596672247627775849u },
    {  8486951013736837725u, 1277337798102220679u }, {  2511075177753209390u, 2043740476963553087u },
    { 13076906586428298482u, 1634992381570842469u }, { 14150874083884549109u, 1307993905256673975u },
    {  4194654460505726958u, 2092790248410678361u }, { 18113118827372222859u, 1674232198728542688u },
    {  3422448617672047318u, 1339385758982834151u }, { 16543964232501006678u, 2143017214372534641u },
    {  9545822571258895019u, 1714413771498027713u }, { 15015355686490936662u, 1371531017198422170u },
    {  5577825024675947042u, 2194449627517475473u }, { 11840957649224578280u, 1755559702013980378u },
    { 16851463748863483271u, 1404447761611184302u }, { 12204946739213931940u, 2247116418577894884u },
    { 13453306206113055875u, 1797693134862315907u }, {  3383947335406624054u, 1438154507889852726u },
    { 16482362180876329456u, 2301047212623764361u }, {  9496540929959153242u, 1840837770099011489u },
    { 11286581558709232917u, 1472670216079209191u }, {  5339916432225476010u, 1178136172863367353u },
    {  4854517476818851293u, 1885017876581387765u }, {  3883613981455081034u, 1508014301265110212u },
    { 14174937629389795797u, 1206411441012088169u }, { 11611853762797942306u, 1930258305619341071u },
    {  5600134195496443521u, 1544206644495472857u }, { 15548153800622885787u, 1235365315596378285u },
    {  6430302007287065643u, 1976584504954205257u }, { 16212288050055383484u, 1581267603963364205u },
    { 12969830440044306787u, 1265014083170691364u }, {  9683682259845159889u, 2024022533073106183u },
    { 15125643437359948558u, 1619218026458484946u }, {  8411165935146048523u, 1295374421166787957u },
    { 17147214310975587960u, 2072599073866860731u }, { 10028422634038560045u, 1658079259093488585u },
    {  8022738107230848036u, 1326463407274790868u }, {  9147032156827446534u, 2122341451639665389u },
    { 11006974540203867551u, 1697873161311732311u }, {  5116230817421183718u, 1358298529049385849u },
    { 15564666937357714594u, 2173277646479017358u }, {  1383687105660440706u, 1738622117183213887u },
    { 12174996128754083534u, 1390897693746571109u }, {  8411947361780802685u, 2225436309994513775u },
    {  6729557889424642148u, 1780349047995611020u }, {  5383646311539713719u, 1424279238396488816u },
    {  1235136468979721303u, 2278846781434382106u }, { 15745504434151418335u, 1823077425147505684u },
    { 16285752362063044992u, 1458461940118004547u }, {  5649904260166615347u, 1166769552094403638u },
    {  5350498001524674232u, 1866831283351045821u }, {   591049586477829062u, 1493465026680836657u },
    { 11540886113407994219u, 1194772021344669325u }, {    18673707743239135u, 1911635234151470921u },
    { 14772334225162232601u, 1529308187321176736u }, {  8128518565387875758u, 1223446549856941389u },
    {  1937583260394870242u, 1957514479771106223u }, {  8928764237799716840u, 1566011583816884978u },
    { 14521709019723594119u, 1252809267053507982u }, {  8477339172590109297u, 2004494827285612772u },
    { 17849917782297818407u, 1603595861828490217u }, {  6901236596354434079u, 1282876689462792174u },
    { 18420676183650915173u, 2052602703140467478u }, {  3668494502695001169u, 1642082162512373983u },
    { 10313493231639821582u, 1313665730009899186u }, {  9122891541139893884u, 2101865168015838698u },
    { 14677010862395735754u, 1681492134412670958u }, {   673562245690857633u, 1345193707530136767u }
};

constexpr inline u32 mulShift32(u32 m, u64 factor, i32 shift) {
    Assert(shift > 32);

    // The casts here help MSVC to avoid calls to the __allmul library function.
    u32 factorLo = u32(factor);
    u32 factorHi = u32(factor >> 32);
    u64 bits0 = u64(m) * u64(factorLo);
    u64 bits1 = u64(m) * u64(factorHi);

    u64 sum = (bits0 >> 32) + bits1;
    u64 shiftedSum = sum >> (shift - 32);

    Assert(shiftedSum <= core::limitMax<u32>());

    return u32(shiftedSum);
}

constexpr inline i32 mulPow5InvDivPow2(u32 m, u32 i, u32 j) {
    return mulShift32(m, FLOAT_POW5_INV_SPLIT[i], j);
}

constexpr u32 floorLog2(u32 x) { return 31 - core::intrin_countLeadingZeros(x); }
constexpr u32 floorLog2(u64 x) { return 63 - core::intrin_countLeadingZeros(x); }

// Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
constexpr inline i32 log2pow5(i32 e) {
    // This approximation works up to the point that the multiplication overflows at e = 3529.
    // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
    // than 2^9297.
    Assert(e >= 0);
    Assert(e <= 3528);
    return i32((u32(e) * 1217359) >> 19);
}

// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
constexpr inline i32 ceilLog2pow5(i32 e) {
    return log2pow5(e) + 1;
}

// Returns true if value is divisible by 2^p.
constexpr inline bool multipleOfPowerOf2_32(u32 value, u32 p) {
    return (value & ((1u << p) - 1)) == 0;
}

constexpr inline u32 pow5factor_32(u32 value) {
    u32 count = 0;
    for (;;) {
        Assert(value != 0);
        u32 q = value / 5;
        u32 r = value % 5;
        if (r != 0) break;
        value = q;
        ++count;
    }
    return count;
}

constexpr inline bool multipleOfPowerOf5_32(u32 value, u32 p) {
    return pow5factor_32(value) >= p;
}

constexpr inline u32 pow5Factor_64(u64 value) {
    u64 m_inv_5 = 14757395258967641293u; // 5 * m_inv_5 = 1 (mod 2^64)
    u64 n_div_5 = 3689348814741910323u;  // #{ n | n = 0 (mod 2^64) } = 2^64 / 5
    u32 count = 0;
    for (;;) {
        Assert(value != 0);
        value *= m_inv_5;
        if (value > n_div_5)
            break;
        ++count;
    }
    return count;
}

// Returns true if value is divisible by 5^p.
constexpr inline bool multipleOfPowerOf5_64(const u64 value, const u32 p) {
    return pow5Factor_64(value) >= p;
}

// TODO2: [PERFORMANCE] explore intrinsics usage to speed up some of these conversion functions.

constexpr inline u64 shiftRight128(const u64 lo, const u64 hi, const u32 dist) {
    // We don't need to handle the case dist >= 64 here (see above).
    Assert(dist < 64);
    Assert(dist > 0);
    return (hi << (64 - dist)) | (lo >> dist);
}

constexpr inline u64 umul128(const u64 a, const u64 b, u64* const productHi) {
    // The casts here help MSVC to avoid calls to the __allmul library function.
    const u32 aLo = u32(a);
    const u32 aHi = u32((a >> 32));
    const u32 bLo = u32(b);
    const u32 bHi = u32((b >> 32));

    const u64 b00 = u64(aLo * bLo);
    const u64 b01 = u64(aLo * bHi);
    const u64 b10 = u64(aHi * bLo);
    const u64 b11 = u64(aHi * bHi);

    const u32 b00Lo = u32(b00);
    const u32 b00Hi = u32((b00 >> 32));

    const u64 mid1 = b10 + b00Hi;
    const u32 mid1Lo = u32((mid1));
    const u32 mid1Hi = u32((mid1 >> 32));

    const u64 mid2 = b01 + mid1Lo;
    const u32 mid2Lo = u32((mid2));
    const u32 mid2Hi = u32((mid2 >> 32));

    const u64 pHi = b11 + mid1Hi + mid2Hi;
    const u64 pLo = (u64(mid2Lo) << 32) | b00Lo;

    *productHi = pHi;
    return pLo;
}

constexpr inline u64 mulShift64(const u64 m, const u64* const mul, const int32_t j) {
    // m is maximum 55 bits
    u64 high1;                                   // 128
    const u64 low1 = umul128(m, mul[1], &high1); // 64
    u64 high0;                                   // 64
    umul128(m, mul[0], &high0);                  // 0
    const u64 sum = high0 + low1;
    if (sum < high0) {
        ++high1; // overflow into high1
    }
    return shiftRight128(sum, high1, j - 64);
}

// This is faster if we don't have a 64x64->128-bit multiplication.
constexpr inline u64 mulShiftAll64(
    u64 m, const u64* const mul, const int32_t j,
    u64* const vp, u64* const vm, const u32 mmShift) {
    m <<= 1;
    // m is maximum 55 bits
    u64 tmp;
    const u64 lo = umul128(m, mul[0], &tmp);
    u64 hi;
    const u64 mid = tmp + umul128(m, mul[1], &hi);
    hi += mid < tmp; // overflow into hi

    const u64 lo2 = lo + mul[0];
    const u64 mid2 = mid + mul[1] + (lo2 < lo);
    const u64 hi2 = hi + (mid2 < mid);
    *vp = shiftRight128(mid2, hi2, u32(j - 64 - 1));

    if (mmShift == 1) {
        const u64 lo3 = lo - mul[0];
        const u64 mid3 = mid - mul[1] - (lo3 > lo);
        const u64 hi3 = hi - (mid3 > mid);
        *vm = shiftRight128(mid3, hi3, u32(j - 64 - 1));
    } else {
        const u64 lo3 = lo + lo;
        const u64 mid3 = mid + mid + (lo3 < lo);
        const u64 hi3 = hi + hi + (mid3 < mid);
        const u64 lo4 = lo3 - mul[0];
        const u64 mid4 = mid3 - mul[1] - (lo4 > lo3);
        const u64 hi4 = hi3 - (mid4 > mid3);
        *vm = shiftRight128(mid4, hi4, u32(j - 64));
    }

    return shiftRight128(mid, hi, u32(j - 64 - 1));
}

constexpr core::expected<f32, ParseError> stof(const char* s, u32 slen) {
    if (s == nullptr || slen == 0) {
        return core::unexpected(ParseError::InputEmpty);
    }

    constexpr i32 MAX_MANTISA_DIGITS = i32(core::maxMantisaDigitsBase10<f32>());
    constexpr i32 MANTISSA_BITS = i32(core::mantisaBits<f32>());
    constexpr i32 EXPONENT_BITS = i32(core::exponentBits<f32>());

    constexpr u32 POW5_INV_BITCOUNT = 59;
    constexpr u32 EXPONENT_BIAS = 127;

    i32 mantissaDigits = 0;
    u32 mantissa = 0;
    i32 exponent = 0;
    u32 dotIndex = slen;
    bool isNegative = false;

    // check if negative
    u32 i = 0;
    if (s[i] == '-') {
        isNegative = true;
        i++;
    }

    // check for special strings
    {
        if (slen - i == 3) {
            if ((s[0] == 'n' || s[0] == 'N') &&
                (s[1] == 'a' || s[1] == 'A') &&
                (s[2] == 'n' || s[2] == 'N')
            ) {
                return core::signalingNaN<f32>();
            }
            if ((s[0] == 'i' || s[0] == 'i') &&
                (s[1] == 'n' || s[1] == 'n') &&
                (s[2] == 'f' || s[2] == 'f')
            ) {
                return isNegative ? -core::infinity<f32>() : core::infinity<f32>();
            }
        }
    }

    // parse the mantissa
    {
        for (; i < slen; i++) {
            char c = s[i];
            if (c == '.') {
                if (dotIndex != slen) {
                    return core::unexpected(ParseError::InputHasMultipleDots);
                }
                dotIndex = i;
            }
            else {
                if (!core::isDigit(c)) return core::unexpected(ParseError::InputHasInvalidSymbol);
                if (mantissaDigits >= MAX_MANTISA_DIGITS) return core::unexpected(ParseError::InputNumberTooLarge);
                mantissa = 10 * mantissa + core::toDigit<decltype(mantissa)>(c);
                if (mantissa != 0) mantissaDigits++;
            }
        }
    }

    if (mantissa == 0) {
        return isNegative ? -0.0f : 0.0f;
    }

    exponent -= slen != dotIndex ? slen - i32(dotIndex) - 1 : 0;

    // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion was exact.
    i32 e2; u32 m2;
    bool trailingZeros;
    {
        e2 = floorLog2(mantissa) + exponent - ceilLog2pow5(-exponent) - (MANTISSA_BITS + 1);

        // We now compute [m * 10^e / 2^e2] = [m / (5^(-e) 2^(e2-e))].
        i32 j = e2 - exponent + ceilLog2pow5(-exponent) - 1 + POW5_INV_BITCOUNT;
        m2 = mulPow5InvDivPow2(mantissa, -exponent, j);

        // We also compute if the result is exact, i.e.,
        //   [m / (5^(-e) 2^(e2-e))] == m / (5^(-e) 2^(e2-e))
        //
        // If e2-e >= 0, we need to check whether (5^(-e) 2^(e2-e)) divides m, which is the
        // case if pow5(m) >= -e AND pow2(m) >= e2-e.
        //
        // If e2-e < 0, we have actually computed [m * 2^(e e2) / 5^(-e)] above,
        // and we need to check whether 5^(-e) divides (m * 2^(e-e2)), which is the case if
        // pow5(m * 2^(e-e2)) = pow5(m) >= -e.
        trailingZeros = (e2 < exponent || (e2 - exponent < 32 && multipleOfPowerOf2_32(mantissa, e2 - exponent)))
                        && multipleOfPowerOf5_32(mantissa, -exponent);
    }

    // Compute the final IEEE exponent.
    u32 ieee_e2 = core::core_max(0u, u32(e2) + EXPONENT_BIAS + u32(floorLog2(m2)));
    if (ieee_e2 > 0xfe) {
        // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
        u32 ieee = ((u32(isNegative)) << u32(EXPONENT_BITS + MANTISSA_BITS)) | (0xffu << MANTISSA_BITS);
        f32 ret = core::bitCast<f32>(ieee);
        return ret;
    }

    // We need to figure out how much we need to shift m2. The tricky part is that we need to take the final IEEE
    // exponent into account, so we need to reverse the bias and also special-case the value 0.
    i32 shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - EXPONENT_BIAS - MANTISSA_BITS;
    Assert(shift >= 0);

    // We need to round up if the exact value is more than 0.5 above the value we computed. That's equivalent to
    // checking if the last removed bit was 1 and either the value was not just trailing zeros or the result would
    // otherwise be odd.
    //
    // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
    trailingZeros &= (m2 & ((1u << (shift - 1)) - 1)) == 0;
    u32 lastRemovedBit = (m2 >> (shift - 1)) & 1;
    bool roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

    u32 ieee_m2 = (m2 >> shift) + roundUp;
    Assert(ieee_m2 <= (1u << (MANTISSA_BITS + 1)));
    ieee_m2 &= (1u << MANTISSA_BITS) - 1;
    if (ieee_m2 == 0 && roundUp) {
        // Rounding up may overflow the mantissa.
        // In this case we move a trailing zero of the mantissa into the exponent.
        // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
        ieee_e2++;
    }

    u32 ieee = (((u32(isNegative) << EXPONENT_BITS) | u32(ieee_e2)) << MANTISSA_BITS) | ieee_m2;
    f32 ret = core::bitCast<f32>(ieee);
    return ret;
}

constexpr core::expected<f64, ParseError> stod(const char* s, u32 slen) {
    constexpr i32 MAX_MANTISA_DIGITS = i32(core::maxMantisaDigitsBase10<f64>());
    constexpr i32 MANTISSA_BITS = i32(core::mantisaBits<f64>());
    constexpr i32 EXPONENT_BITS = i32(core::exponentBits<f64>());

    constexpr u32 POW5_INV_BITCOUNT = 125;
    constexpr u32 EXPONENT_BIAS = 1023;

    i32 mantissaDigits = 0;
    u64 mantissa = 0;
    i32 exponent = 0;
    u32 dotIndex = slen;
    bool isNegative = false;

    // check if negative
    u32 i = 0;
    if (s[i] == '-') {
        isNegative = true;
        i++;
    }

    // check for special strings
    {
        if (slen - i == 3) {
            if ((s[0] == 'n' || s[0] == 'N') &&
                (s[1] == 'a' || s[1] == 'A') &&
                (s[2] == 'n' || s[2] == 'N')
            ) {
                return core::signalingNaN<f64>();
            }
            if ((s[0] == 'i' || s[0] == 'i') &&
                (s[1] == 'n' || s[1] == 'n') &&
                (s[2] == 'f' || s[2] == 'f')
            ) {
                return isNegative ? -core::infinity<f64>() : core::infinity<f64>();
            }
        }
    }

    // parse the mantissa
    for (; i < slen; i++) {
        char c = s[i];
        if (c == '.') {
            if (dotIndex != slen) {
                return core::unexpected(ParseError::InputHasMultipleDots);
            }
            dotIndex = i;
        }
        else {
            if (!core::isDigit(c)) return core::unexpected(ParseError::InputHasInvalidSymbol);
            if (mantissaDigits >= MAX_MANTISA_DIGITS) return core::unexpected(ParseError::InputNumberTooLarge);
            mantissa = 10 * mantissa + core::toDigit<decltype(mantissa)>(c);
            if (mantissa != 0) mantissaDigits++;
        }
    }

    if (mantissa == 0) {
        return isNegative ? -0.0f : 0.0f;
    }

    exponent -= slen != dotIndex ? slen - i32(dotIndex) - 1 : 0;

    // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion was exact.
    i32 e2;
    u64 m2;
    bool trailingZeros;
    {
        e2 = floorLog2(mantissa) + exponent - ceilLog2pow5(-exponent) - (MANTISSA_BITS + 1);

        // We now compute [m * 10^e / 2^e2] = [m / (5^(-e) 2^(e2-e))].
        i32 j = e2 - exponent + ceilLog2pow5(-exponent) - 1 + POW5_INV_BITCOUNT;
        m2 = mulShift64(mantissa, detail::DOUBLE_POW5_INV_SPLIT[-exponent], j);
        trailingZeros = multipleOfPowerOf5_64(mantissa, -exponent);
    }

    // Compute the final IEEE exponent.
    u32 ieee_e2 = core::core_max(0u, u32(e2) + EXPONENT_BIAS + u32(floorLog2(m2)));
    if (ieee_e2 > 0x7fe) {
        // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
        u64 ieee = ((u64(isNegative)) << u64(EXPONENT_BITS + MANTISSA_BITS)) | (0x7ffull << MANTISSA_BITS);
        f64 ret = core::bitCast<f64>(ieee);
        return ret;
    }

    // We need to figure out how much we need to shift m2. The tricky part is that we need to take the final IEEE
    // exponent into account, so we need to reverse the bias and also special-case the value 0.
    i32 shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - EXPONENT_BIAS - MANTISSA_BITS;
    Assert(shift >= 0);

    // We need to round up if the exact value is more than 0.5 above the value we computed. That's equivalent to
    // checking if the last removed bit was 1 and either the value was not just trailing zeros or the result would
    // otherwise be odd.
    //
    // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
    trailingZeros &= (m2 & ((1ull << (shift - 1)) - 1)) == 0;
    u64 lastRemovedBit = (m2 >> (shift - 1)) & 1;
    bool roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

    u64 ieee_m2 = (m2 >> u64(shift)) + roundUp;
    Assert(ieee_m2 <= (1ull << (MANTISSA_BITS + 1)));
    ieee_m2 &= (1ull << MANTISSA_BITS) - 1;
    if (ieee_m2 == 0 && roundUp) {
        // Rounding up may overflow the mantissa.
        // In this case we move a trailing zero of the mantissa into the exponent.
        // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
        ieee_e2++;
    }

    u64 ieee = (((u64(isNegative) << EXPONENT_BITS) | u64(ieee_e2)) << u64(MANTISSA_BITS)) | ieee_m2;
    f64 res = core::bitCast<f64>(ieee);
    return res;
}

} // detail

template <typename TFloat>
constexpr core::expected<TFloat, ParseError> cstrToFloat(const char* s, u32 slen) {
    if constexpr (std::is_same_v<TFloat, f32>) {
        return detail::stof(s, slen);
    }
    else if constexpr (std::is_same_v<TFloat, f64>) {
        return detail::stod(s, slen);
    }
    else {
        static_assert(core::always_false<TFloat>, "cstrToFloat was called with an invalid type");
    }
}

#pragma endregion

} // namespace core
