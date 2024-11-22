#ifndef GAME_SCENE
#define GAME_SCENE

#include <raylib.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include "scene_manager.hpp"

class GameScene : public Scene
{
    Texture raylib_logo;
    Vector2 logo_position;
    float move_dir_x = 1;
    float move_dir_y = 1;

public:
    void Begin() override
    {
        raylib_logo = ResourceManager::GetInstance()->GetTexture("Raylib_logo.png");
        logo_position = {300, 100};
    }

    void End() override {}

    void Update() override
    {
        float delta_time = GetFrameTime();

        logo_position.x += 100 * delta_time * move_dir_x;
        logo_position.y += 100 * delta_time * move_dir_y;

        if (logo_position.x + 200 >= 800 || logo_position.x <= 0)
        {
            move_dir_x *= -1;
        }
        if (logo_position.y + 200 >= 600 || logo_position.y <= 0)
        {
            move_dir_y *= -1;
        }
    }

    void Draw() override
    {
        DrawTexturePro(raylib_logo, {0, 0, 256, 256}, {logo_position.x, logo_position.y, 200, 200}, {0, 0}, 0.0f, WHITE);
    }
};

#endif