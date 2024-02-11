#include <ecs/components/all_components.h>
#include <ecs/systems/event_queue_system.h>
#include <ecs/systems/keyboard_state_systems.h>
#include <ecs/systems/load_map_systems.h>
#include <ecs/systems/phisics_systems.h>
#include <ecs/systems/render_hud_systems.h>
#include <ecs/systems/render_objects_systems.h>
#include <my_common_cpp_utils/Logger.h>
#include <utils/file_system.h>
#include <utils/imgui_sdl_RAII.h>
#include <utils/sdl_RAII.h>

int main(int argc, char* args[])
{
    try
    {
        // Initialize the logger with the trace level.
        utils::Logger::getInstance(spdlog::level::info);

        // Create an EnTT registry.
        entt::registry registry;

        // Create a game state entity.
        auto& gameState = registry.emplace<GameState>(registry.create());
        gameState.cameraCenter = gameState.windowSize / 2.0f;

        // Create a physics world with gravity and store it in the registry.
        b2Vec2 gravity(0.0f, +9.8f);
        gameState.physicsWorld = std::make_shared<b2World>(gravity);

        // Initialize SDL, create a window and a renderer. Initialize ImGui.
        SDLInitializer sdlInitializer(SDL_INIT_VIDEO);
        SDLWindow window("Bouncing Ball with SDL, ImGui, EnTT & GLM", gameState.windowSize);
        SDLRenderer renderer(window.get());
        ImGuiSDL imguiSDL(window.get(), renderer.get());

        // Load the map.
        std::string mapPath = "C:\\dev\\my_tiled_maps\\map002_wofares\\map.json";
        LoadMap(registry, renderer.get(), mapPath);

        // Start the game loop.
        Uint32 lastTick = SDL_GetTicks();
        while (!gameState.quit)
        {
            Uint32 frameStart = SDL_GetTicks();

            if (utils::FileHasChanged(mapPath) || gameState.reloadMap)
            {
                UnloadMap(registry);
                LoadMap(registry, renderer.get(), mapPath);
            }

            EventQueueSystem(registry); // Impact on Camera.
            KeyboardStateSystem(registry); // Impact on Player.

            // Calculate delta time.
            Uint32 currentTick = SDL_GetTicks();
            float deltaTime = static_cast<float>(currentTick - lastTick) / 1000.0f;
            lastTick = currentTick;

            // Update the physics.
            PhysicsSystem(registry, deltaTime);

            // Render the scene and the HUD.
            imguiSDL.startFrame();
            RenderSystem(registry, renderer.get());
            DrawGridSystem(renderer.get(), gameState);
            RenderHUDSystem(registry, renderer.get());
            imguiSDL.finishFrame();

            // Cap the frame rate.
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            const Uint32 frameDelay = 1000 / gameState.fps;
            if (frameDelay > frameTime)
            {
                SDL_Delay(frameDelay - frameTime);
            }
        }
    }
    catch (const std::runtime_error& e)
    {
        MY_LOG_FMT(warn, "[main] Unhandled exception: {}", e.what());
        return -1;
    }

    return 0;
}
