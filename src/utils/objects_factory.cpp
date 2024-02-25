#include "objects_factory.h"
#include "box2d/b2_polygon_shape.h"
#include <ecs/components/game_components.h>
#include <utils/box2d_body_creator.h>
#include <utils/coordinates_transformer.h>

ObjectsFactory::ObjectsFactory(entt::registry& registry)
  : registry(registry), gameState(registry.get<GameState>(registry.view<GameState>().front())),
    physicsWorld(gameState.physicsWorld), box2dBodyCreator(registry)
{}

entt::entity ObjectsFactory::createPlayer(const glm::vec2& sdlPos)
{
    auto gap = gameState.physicsOptions.gapBetweenPhysicalAndVisual;

    glm::vec2 playerSdlSize{10, 10};
    glm::vec2 playerSdlBBox = playerSdlSize - glm::vec2{gap, gap};

    auto entity = registry.create();
    registry.emplace<RenderingInfo>(entity, playerSdlSize);
    registry.emplace<PlayerInfo>(entity);
    auto playerPhysicsBody = box2dBodyCreator.CreateDynamicPhysicsBody(entity, sdlPos, playerSdlBBox);

    // Add sensor to detect the ground below the player.
    box2dBodyCreator.AddThinSensorBelowTheBody(playerPhysicsBody, playerSdlBBox);

    registry.emplace<PhysicsInfo>(entity, playerPhysicsBody);
    return entity;
}