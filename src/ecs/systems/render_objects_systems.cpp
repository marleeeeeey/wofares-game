#include "render_objects_systems.h"
#include "my_common_cpp_utils/Logger.h"
#include <cmath>
#include <ecs/components/all_components.h>
#include <numbers>
#include <utils/colors.h>

void RenderSystem(entt::registry& registry, SDL_Renderer* renderer)
{
    // Clear the screen with white color.
    SetRenderDrawColor(renderer, ColorName::White);
    SDL_RenderClear(renderer);

    auto& gameState = registry.get<GameState>(registry.view<GameState>().front());

    { // Render tiles.
        auto tilesView = registry.view<Position, SizeComponent, TileInfo, PhysicalBody>();
        for (auto entity : tilesView)
        {
            auto& position = tilesView.get<Position>(entity);
            auto& size = tilesView.get<SizeComponent>(entity);
            auto& tileInfo = tilesView.get<TileInfo>(entity);
            auto& physicalBody = tilesView.get<PhysicalBody>(entity);

            // Compute the destination rectangle on the screen.
            glm::vec2 transformedPosition =
                (position.value - gameState.cameraCenter) * gameState.cameraScale + gameState.windowSize / 2.0f;

            // Have to render from the center of the object. Because the Box2D body is in the center of the object.
            SDL_Rect destRect = {
                static_cast<int>(transformedPosition.x - size.value.x * gameState.cameraScale / 2),
                static_cast<int>(transformedPosition.y - size.value.y * gameState.cameraScale / 2),
                static_cast<int>(size.value.x * gameState.cameraScale),
                static_cast<int>(size.value.y * gameState.cameraScale)};

            // Calculate the angle in degrees.
            auto angle = physicalBody.body->GetBody()->GetAngle();
            SDL_Point center = {destRect.w / 2, destRect.h / 2};
            double angleDegrees = angle * 180.0 / std::numbers::pi;

            // Render the tile with the calculated angle.
            SDL_RenderCopyEx(
                renderer, tileInfo.texture->get(), &tileInfo.srcRect, &destRect, angleDegrees, &center, SDL_FLIP_NONE);
        }
    }

    { // Render players.
        SetRenderDrawColor(renderer, ColorName::Blue);
        auto players = registry.view<Position, SizeComponent, PlayerNumber>();
        for (auto entity : players)
        {
            const auto& [position, size] = players.get<Position, SizeComponent>(entity);

            glm::vec2 transformedPosition =
                (position.value - gameState.cameraCenter) * gameState.cameraScale + gameState.windowSize / 2.0f;

            // Have to render from the center of the object. Because the Box2D body is in the center of the object.
            SDL_Rect rect = {
                static_cast<int>(transformedPosition.x - size.value.x * gameState.cameraScale / 2),
                static_cast<int>(transformedPosition.y - size.value.y * gameState.cameraScale / 2),
                static_cast<int>(size.value.x * gameState.cameraScale),
                static_cast<int>(size.value.y * gameState.cameraScale)};

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
