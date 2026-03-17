/**
 * @file Declares SparseMap<K, V>
 */
#ifndef WONDERLAND_ADT_SPARSEMAP_H
#define WONDERLAND_ADT_SPARSEMAP_H

#include <concepts>
#include <cstddef>
#include <vector>

namespace Wonderland::ADT {
template <std::convertible_to<std::size_t> K, typename V> class SparseMap {
public:
  void clear();

private:
  K Size;
  std::pmr::vector<K> Sparse;
  std::pmr::vector<K> Dense;
  std::pmr::vector<V> DenseValues;
};
} // namespace Wonderland::ADT

#endif // WONDERLAND_ADT_SPARSEMAP_H
