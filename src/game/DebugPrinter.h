#ifndef NVE_DEBUG_PRINTER_H
#define NVE_DEBUG_PRINTER_H

#include <iosfwd>

namespace nuvelocity::frs42
{
    class BrickLayout;

    class DebugPrinter
    {
    public:
        static void DumpToConsole(const BrickLayout& layout);
        static void Dump(const BrickLayout& layout, std::ostream& stream);
    };
} // namespace nuvelocity::frs42

#endif // NVE_DEBUG_PRINTER_H
