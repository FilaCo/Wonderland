#include "Wonderland/ADT/SparseMap.h"
#include <concepts>

namespace Wonderland::ADT {
template <std::convertible_to<size_t> K, typename V>
void SparseMap<K, V>::clear() noexcept {
  Size = 0;
}
} // namespace Wonderland::ADT