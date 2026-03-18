#include "Wonderland/IPS/Registry.h"
#include "Wonderland/IPS/Id.h"

namespace Wonderland::IPS {
Id Registry::spawn() {
  if (Available > 0) {
    return recycle();
  }
  return spawnImpl();
}

Id Registry::recycle() {
  // holder stores recycled id version
  auto PositionToRecycle = NextPosition;
  auto Holder = Ids[PositionToRecycle];

  auto Recycled = Id(PositionToRecycle, Holder.Version);

  // restore invariants
  NextPosition = Holder.Position;
  --Available;

  return Recycled;
}
Id Registry::spawnImpl() {
  auto Spawned = Id(NextPosition);
  Ids.push_back(Spawned);

  // restore invariants
  ++NextPosition;

  return Spawned;
}

void Registry::despawn(Id IdToDespawn) noexcept {
  // holder stores previous NextPosition and actual version of the Id
  auto PositionToDespawn = IdToDespawn.Position;
  auto Holder = Id(NextPosition, IdToDespawn.Version + 1);
  Ids[PositionToDespawn] = Holder;

  // restore invariants
  NextPosition = PositionToDespawn;
  ++Available;
}

bool Registry::isAlive(Id Id) const {
  auto TargetPosition = Id.Position;
  return TargetPosition < Ids.size() &&
         Id.Version == Ids[TargetPosition].Version;
}
bool Registry::isDead(Id Id) const { return !isAlive(Id); }

} // namespace Wonderland::IPS
