#include "DebugPrinter.h"

#include "BrickLayout.h"

#include <SDL3/SDL_log.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>

namespace nuvelocity::frs42
{
    namespace
    {
        char PixelToGlyph(uint32_t pixel)
        {
            if (pixel == 0)
            {
                return '.';
            }

            static constexpr std::array<char, 62> kSymbols{
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
                'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
            return kSymbols[pixel % kSymbols.size()];
        }
    } // namespace

    void DebugPrinter::DumpToConsole(const BrickLayout& layout)
    {
        std::ostringstream stream;
        Dump(layout, stream);
        SDL_Log("%s", stream.str().c_str());
    }

    void DebugPrinter::Dump(const BrickLayout& layout, std::ostream& stream)
    {
        stream << "=== BrickLayout Dump ===\n";
        stream << "Display Name: " << layout.mDisplayName << "\n";
        stream << "Author: " << layout.mAuthor << "\n";
        stream << "Background Type: " << layout.mBackgroundType << "\n";
        stream << "Use Default Power-up Weights: "
               << (layout.mUseDefaultPowerUpWeights ? "true" : "false") << "\n";
        stream << "Brick Palette Size: " << layout.mBrickPalette.size() << "\n";

        for (size_t index = 0; index < layout.mBrickPalette.size(); ++index)
        {
            stream << "  [" << index << "] " << layout.mBrickPalette[index] << "\n";
        }

        stream << "Row Offset Count: " << layout.mRowsOffsets.size() << "\n";
        stream << "Floating Brick Count: " << layout.mFloatingBricks.size() << "\n";

        if (layout.mLayout == nullptr)
        {
            stream << "Layout: <null>\n";
            stream << "========================\n";
            return;
        }

        const int width = layout.mLayout->GetWidth();
        const int height = layout.mLayout->GetHeight();
        stream << "Layout: " << width << "x" << height
               << " (bpp=" << static_cast<unsigned int>(layout.mLayout->GetBitsPerPixel()) << ")\n";
        stream << "Layout ('.' = empty, other chars = brick value buckets):\n";

        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                const uint32_t pixel = layout.mLayout->GetPixel(col, row);
                stream << PixelToGlyph(pixel);
            }

            stream << "\n";
        }

        stream << "========================\n";
    }
} // namespace nuvelocity::frs42
