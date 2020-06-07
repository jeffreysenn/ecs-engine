#include "pch.h"
// pre-compiled header

#include <random>

#include "ecs-engine/core/component_context.h"
#include "ecs-engine/core/entity_manager.h"
#include "ecs-engine/extension/component/instanced_sprite_component.h"
#include "ecs-engine/extension/component/singleton_camera_component.h"
#include "ecs-engine/extension/component/transform_component.h"
#include "ecs-engine/extension/policy/threading_model.h"
#include "ecs-engine/extension/system/instanced_sprite_rendering_system.h"
#include "ecs-engine/graphics/graphics.h"
#include "ecs-engine/window/window.h"

namespace ecs {

struct ISRTest : testing::Test {
  using MyComponentList =
      ComponentList<TransformComponent, InstancedSpriteComponent>;
  using MySingletonComponentList =
      SingletonComponentList<SingletonCameraComponent>;
  using MyComponentContext =
      ComponentContext<MyComponentList, MySingletonComponentList>;
  using MyEntityManager = EntityManager<>;

  ISRTest()
      : window(Window::Mode::WINDOWED, 1920, 1080, "application")
      , ett_pool(1000)
      , ett_mgr(component_managers, singleton_components, ett_pool)
      , instanced_sprite_shader(
            [] {
              auto vertex_shader = VertexShader{};
              vertex_shader.LoadFromFile(
                  "ecs-engine/resource/shader/"
                  "instanced_sprite_vertex_shader.glsl");
              return vertex_shader;
            }(),
            [] {
              auto fragment_shader = FragmentShader{};
              fragment_shader.LoadFromFile(
                  "ecs-engine/resource/shader/"
                  "instanced_sprite_fragment_shader.glsl");
              return fragment_shader;
            }())
      , sys(ett_mgr, instanced_sprite_shader) {
    component_managers.Make<TransformComponent>(1000);
    component_managers.Make<InstancedSpriteComponent>(1000);
    singleton_components.Make<SingletonCameraComponent>();
    tex.LoadFromFile("ecs-engine-test/resource/white-triangle.png");
  }

  Window window;
  MyComponentContext context;
  ComponentManagerSet component_managers;
  SingletonComponentSet singleton_components;
  EntityPool ett_pool;
  MyEntityManager ett_mgr;
  ShaderProgram instanced_sprite_shader;
  InstancedSpriteRenderingSystem<MyEntityManager> sys;
  Texture tex;
};

TEST_F(ISRTest, Render) {
  Sampler sampler{};
  sampler.Set(GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  sampler.Bind();
  Sprite sprite{tex, {0, 0, tex.Width(), tex.Height()}};
  auto& cam = ett_mgr.GetSingletonComponent<SingletonCameraComponent>();
  cam.projection =
      ecs::Projection{ecs::Projection::Type::ORTHOGRAPHIC,
                      ecs::Projection::OrthographicAttrib{0, 1920, 0, 1080}};

  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<int> distribution_x(0, 1920);
  std::uniform_int_distribution<int> distribution_y(0, 1080);
  std::uniform_real_distribution<float> distribution_rotate(0, 360);

  for (auto i = std::size_t(0); i != 50; ++i) {
    auto ett_handle = ett_mgr.CreateEntity();
    auto& transform_comp = ett_handle.AddComponent<TransformComponent>();
    auto& transform = transform_comp.transform;
    transform.SetPosition(
        glm::vec3(distribution_x(generator), distribution_y(generator), 0));
    transform.SetRotation(glm::quat(
        glm::vec3(0, 0, glm::radians(distribution_rotate(generator)))));
    transform.SetScale(glm::vec3(0.05f, 0.1f, 1.f));
    auto& inst_sprite = ett_handle.AddComponent<InstancedSpriteComponent>();
    inst_sprite.sprite = &sprite;
  }

  while (!window.ShouldClose()) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sys.Render();

    window.SwapBuffer();
    window.PollEvents();
  }
}

}  // namespace ecs