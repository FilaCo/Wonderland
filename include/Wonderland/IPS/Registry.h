#ifndef WONDERLAND_IPS_REGISTRY_H
#define WONDERLAND_IPS_REGISTRY_H

#include "Wonderland/IPS/Id.h"
#include <vector>

namespace Wonderland::IPS {
class Registry {
public:
  Id spawn();
  void despawn(Id IdToDespawn) noexcept;
  bool isAlive(Id Id) const;
  bool isDead(Id Id) const;

private:
  Id recycle();
  Id spawnImpl();

  size_t Available;
  size_t NextPosition;
  std::pmr::vector<Id> Ids;
};
} // namespace Wonderland::IPS

#endif // WONDERLAND_IPS_REGISTRY_H
