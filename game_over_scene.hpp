#ifndef GAME_OVER_SCENE
#define GAME_OVER_SCENE

#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "game_components.hpp"
#include "scene_manager.hpp"

class GameOverScene : public Scene
{
    UILibrary ui_library;

public:
    // void startScene()
    // {
    //     if (GetSceneManager() != nullptr)
    //     {
    //         GetSceneManager()->SwitchScene(2);
    //     }
    // }

    void Begin() override
    {
        ui_library.root_container.bounds = {10, 10, 600, 500};
    }

    void End() override {}

    void Update() override
    {

        ui_library.Update();
    }

    void Draw() override
    {
        DrawText("GAME OVER", 300, 325, 30, RED);
        ui_library.Draw();
    }
};

#endif