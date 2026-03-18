#include "Wonderland/IPS/Id.h"

namespace Wonderland::IPS {
static const uint8_t IdPositionBits = 20;

Id::operator unsigned long long() {
  return Position | (Version << IdPositionBits);
}
} // namespace Wonderland::IPS