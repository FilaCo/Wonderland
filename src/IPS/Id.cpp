#include "Wonderland/IPS/Id.h"
#include <cstddef>

namespace Wonderland::IPS {
static const uint8_t IdPositionBits = 20;
static const uint32_t IdPositionMask = (1 << 20) - 1;

Id::Id(size_t RawId) noexcept
    : Position(RawId & IdPositionMask), Version(RawId >> IdPositionBits) {}

Id::operator size_t() const { return Position | (Version << IdPositionBits); }

bool Id::operator==(Id Other) const {
  return Position == Other.Position && Version == Other.Version;
}
bool Id::operator!=(Id Other) const { return !(*this == Other); }

Id::Id(uint32_t Position, uint32_t Version) noexcept
    : Position(Position), Version(Version) {}

} // namespace Wonderland::IPS