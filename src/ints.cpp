#include <ints.h>

namespace core
{

bool IsInRange(u32 min, u32 n, u32 max)    { return (min < n && n < max); }
bool IsInRangeInc(u32 min, u32 n, u32 max) { return (min <= n && n <= max); }
bool IsInRange(i32 min, i32 n, i32 max)    { return (min < n && n < max); }
bool IsInRangeInc(i32 min, i32 n, i32 max) { return (min <= n && n <= max); }

} // namespace core
