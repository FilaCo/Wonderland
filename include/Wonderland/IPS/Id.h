#ifndef WONDERLAND_IPS_ID_H
#define WONDERLAND_IPS_ID_H

#include <cstddef>
#include <cstdint>

namespace Wonderland::IPS {
class Id {
public:
  explicit Id(size_t RawId) noexcept;

  operator size_t() const;
  bool operator==(Id Other) const;
  bool operator!=(Id Other) const;

private:
  explicit Id(uint32_t Position, uint32_t Version) noexcept;

  friend class Registry;
  uint32_t Position : 20;
  uint32_t Version : 12;
};
} // namespace Wonderland::IPS

#endif // WONDERLAND_IPS_ID_H
