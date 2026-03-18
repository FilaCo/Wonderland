#ifndef WONDERLAND_IPS_REGISTRY_HPP
#define WONDERLAND_IPS_REGISTRY_HPP

#include "Wonderland/IPS/Id.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Wonderland::IPS {
template <typename AllocatorT = std::allocator<Id>> class Registry final {
public:
  Registry() noexcept : Available(0), NextPosition(0) {}
  Id spawn() {
    if (Available > 0) {
      return recycle();
    }
    return spawnImpl();
  }
  void despawn(Id IdToDespawn) noexcept {
    // holder stores previous NextPosition and actual version of the Id
    auto PositionToDespawn = IdToDespawn.Position;
    auto Holder = Id(NextPosition, IdToDespawn.Version + 1);
    Ids[PositionToDespawn] = Holder;

    NextPosition = PositionToDespawn;
    ++Available;
  }
  bool isAlive(Id Id) const noexcept {
    const auto TargetPosition = Id.Position;
    return TargetPosition < Ids.size() &&
           Id.Version == Ids[TargetPosition].Version;
  }
  bool isDead(Id Id) const noexcept { return !isAlive(Id); }

private:
  Id recycle() noexcept {
    // holder stores recycled id version
    auto PositionToRecycle = NextPosition;
    auto Holder = Ids[PositionToRecycle];

    auto Recycled = Id(PositionToRecycle, Holder.Version);

    NextPosition = Holder.Position;
    --Available;

    return Recycled;
  }
  Id spawnImpl() {
    auto Spawned = Id(NextPosition);
    Ids.push_back(Spawned);

    ++NextPosition;

    return Spawned;
  }

  uint32_t Available;
  uint32_t NextPosition;
  std::vector<Id, AllocatorT> Ids;
};
} // namespace Wonderland::IPS

#endif // WONDERLAND_IPS_REGISTRY_HPP
