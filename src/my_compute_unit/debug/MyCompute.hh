/* Auto-added debug flag for MyCompute device
 * This header provides an inline debug flag instance so source
 * can use DPRINTF(MyCompute, ...) without requiring generated
 * build headers. It's safe to keep in source tree.
 */
#ifndef __DEBUG_MyCompute_HH__
#define __DEBUG_MyCompute_HH__

#include "base/compiler.hh"
#include "base/debug.hh"

namespace gem5
{
namespace debug
{
namespace unions
{
inline union MyCompute
{
    ~MyCompute() {}
    SimpleFlag flagMyCompute;

    MyCompute()
    : flagMyCompute(
        "MyCompute",
        "MyCompute device debug",
        false)
{}
} instanceMyCompute;

} // namespace unions

inline constexpr const auto &MyCompute =
    ::gem5::debug::unions::instanceMyCompute.flagMyCompute;

} // namespace debug
} // namespace gem5

#endif // __DEBUG_MyCompute_HH__
