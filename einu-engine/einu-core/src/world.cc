#include "einu-core/internal/world.h"

namespace einu {
namespace internal {

void World::AddEntityImpl(IEntity& ett) {
  entity_table_.emplace(ett.GetID(), &ett);
}

void World::RemoveEntityImpl(EID eid) { entity_table_.erase(eid); }

einu::IEntity& World::GetEntityImpl(EID eid) noexcept {
  return *entity_table_.at(eid);
}

const einu::IEntity& World::GetEntityImpl(EID eid) const noexcept {
  return *entity_table_.at(eid);
}

std::size_t World::GetEntityCountImpl() const noexcept {
  return entity_table_.size();
}

void World::GetAllEntitiesImpl(EntityBuffer& buffer) const {
  buffer.clear();
  for (auto [id, ett] : entity_table_) {
    buffer.push_back(*ett);
  }
}

void World::SetSinglentityImpl(IEntity& singlentity) noexcept {
  singlentity_ = &singlentity;
}

IEntity& World::GetSinglentityImpl() noexcept { return *singlentity_; }

const IEntity& World::GetSinglentityImpl() const noexcept {
  return *singlentity_;
}

}  // namespace internal
}  // namespace einu