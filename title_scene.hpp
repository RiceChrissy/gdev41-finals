#ifndef TITLE_SCENE
#define TITLE_SCENE

#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


#include "game_components.hpp"
#include "scene_manager.hpp"



class TitleScene : public Scene
{
    Texture raylib_logo;

    UILibrary ui_library;
    BasicButton start_game_button;

    


    void(TitleScene::*fptr)() = &TitleScene::startScene;


public:
    void startScene(){
        if (GetSceneManager() != nullptr)
        {
            GetSceneManager()->SwitchScene(1);
        }
    }

    void Begin() override
    {
        raylib_logo = ResourceManager::GetInstance()->GetTexture("Raylib_logo.png");

        ui_library.root_container.bounds = {10, 10, 600, 500};

        start_game_button.text = "START GAME!";
        start_game_button.bounds = {350, 400, 100, 40};
        start_game_button.isClicked = false;
        ui_library.root_container.AddChild(&start_game_button);
    }

    void End() override {}

    void Update() override
    {
        if (IsKeyPressed(KEY_ENTER)|| start_game_button.isClicked)
        {
            if (GetSceneManager() != nullptr )
            {
                GetSceneManager()->SwitchScene(1);
            }
        }
        ui_library.Update();
    }

    void Draw() override
    {
        DrawTexturePro(raylib_logo, {0, 0, 256, 256}, {300, 100, 200, 200}, {0, 0}, 0.0f, WHITE);
        DrawText("Press ENTER", 300, 325, 30, BLACK);
        ui_library.Draw();
    }
};

#endif