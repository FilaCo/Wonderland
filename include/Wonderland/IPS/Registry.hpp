#ifndef WONDERLAND_IPS_REGISTRY_HPP
#define WONDERLAND_IPS_REGISTRY_HPP

#include "Wonderland/IPS/Id.hpp"
#include <cassert>
#include <cstdint>
#include <memory>
#include <new>
#include <optional>
#include <vector>

namespace Wonderland::IPS {
template <typename AllocatorT = std::allocator<Id>> class Registry final {
public:
  Registry() noexcept : Available(0), NextPosition(0) {}
  explicit Registry(AllocatorT &Allocator) noexcept
      : Available(0), NextPosition(0), Ids{Allocator} {}
  Id spawn() { return Available > 0 ? recycle() : spawnImpl(); }
  void despawn(Id IdToDespawn) noexcept {
    if (isDead(IdToDespawn)) {
      return;
    }
    auto PositionToDespawn = IdToDespawn.Position;
    uint32_t NextVersion = IdToDespawn.Version + 1;
    if (NextVersion >= Id::RetiredVersion) {
      // Version would wrap — retire the slot permanently, do not recycle
      Ids[PositionToDespawn] = Id(PositionToDespawn, Id::RetiredVersion);
      return;
    }
    Ids[PositionToDespawn] = Id(NextPosition, NextVersion);

    NextPosition = PositionToDespawn;
    ++Available;
  }
  /**
   * @note ids that have position, which is out of range, are considered **not**
   * alive.
   */
  bool isAlive(Id Id) const noexcept {
    const auto TargetPosition = Id.Position;
    return TargetPosition < Ids.size() &&
           Id.Version == Ids[TargetPosition].Version;
  }
  bool isDead(Id Id) const noexcept { return !isAlive(Id); }

private:
  Id recycle() noexcept {
    auto PositionToRecycle = NextPosition;
    // holder stores recycled id version
    auto Holder = Ids[PositionToRecycle];

    auto Recycled = Id(PositionToRecycle, Holder.Version);

    NextPosition = Holder.Position;
    --Available;

    return Recycled;
  }
  Id spawnImpl() {
    assert(NextPosition < Id::MaxPosition);
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
