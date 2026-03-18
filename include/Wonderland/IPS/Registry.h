#ifndef WONDERLAND_IPS_REGISTRY_H
#define WONDERLAND_IPS_REGISTRY_H

#include "Wonderland/IPS/Id.h"
#include <vector>

namespace Wonderland::IPS {
class Registry {
public:
  Id spawn();
  void despawn(Id IdToDespawn);
  bool isAlive(Id Id);
  bool isDead(Id Id);

private:
  Id recycle();
  Id spawnImpl();

  size_t Available;
  size_t NextPosition;
  std::pmr::vector<Id> Ids;
};
} // namespace Wonderland::IPS

#endif // WONDERLAND_IPS_REGISTRY_H
