#include "API.h"

namespace core {

// Zero cost defer:
#ifndef defer
    struct CORE_API_EXPORT defer_dummy {};
    template <class F> struct deferrer { F f; ~deferrer() { f(); } };
    template <class F> deferrer<F> operator*(core::defer_dummy, F f) { return {f}; }
    #define DEFER_(LINE) zz_defer##LINE
    #define DEFER(LINE) DEFER_(LINE)
    #define defer auto DEFER(__LINE__) = core::defer_dummy{} *[&]()
#endif

}
