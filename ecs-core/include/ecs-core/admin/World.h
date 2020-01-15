#pragma once
#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include "ecs-core/manager/Component_manager.h"
#include "ecs-core/manager/Entity_manager.h"
#include "ecs-core/system/I_system.h"

namespace ecs {
class World {
 private:
  using sys_ptr = std::unique_ptr<I_system>;
  template<typename T>
  using mgr_ptr = std::unique_ptr<T>;
  using ett_handle = Entity_manager::Entity_handle;
  using time = float;

 public:
  World() = default;

  void add_system(sys_ptr s);
  template<typename T>
  void add_component_manager(mgr_ptr<T> mgr);
  ett_handle create_entity();

  void update(time dt);

  Entity_manager& get_entity_manager() noexcept;

 private:
  std::vector<sys_ptr> system_arr_;
  Entity_manager entity_manager_;
};
}  // namespace ecs

namespace ecs {
inline void World::add_system(sys_ptr s) {
  system_arr_.push_back(std::move(s));
}

inline Entity_manager& World::get_entity_manager() noexcept {
  return entity_manager_;
}

inline void World::update(time dt) {
  using namespace std;
  for_each(
      begin(system_arr_), end(system_arr_), [=](auto&& s) { s->update(dt); });
}
}  // namespace ecs