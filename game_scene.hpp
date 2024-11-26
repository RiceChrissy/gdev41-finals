#ifndef GAME_SCENE
#define GAME_SCENE

#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>

#include "scene_manager.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FPS = 60;
const float TIMESTEP = 1 / FPS;
const float FRICTION = 1;

struct Player
{
    Vector2 position;
    float radius;
    Color color;

    float mass;
    float inverse_mass;
    Vector2 acceleration;
    Vector2 velocity;
};

// struct WeaponComponent
// {
// };

float GetWeaponAngle(Vector2 circle_position)
{
    Vector2 mouse_position = GetMousePosition();

    // Calculate the vector from the circle to the mouse
    float dx = mouse_position.x - circle_position.x;
    float dy = mouse_position.y - circle_position.y;

    // Get radians then convert to degrees
    float angle_degrees = atan2f(dy, dx) * (180.0f / PI);

    // Ensure the angle is in the range [0, 360]
    if (angle_degrees < 0)
    {
        angle_degrees += 360.0f;
    }

    return angle_degrees;
}

class GameScene : public Scene
{
    Texture raylib_logo;
    Vector2 logo_position;
    float move_dir_x = 1;
    float move_dir_y = 1;

    Texture sword;
    Vector2 sword_position;
    bool sword_is_active;
    float sword_rotation;
    float sword_animation;

    Texture shield;
    Vector2 shield_position;
    bool shield_is_active;
    float shield_animation;

    Player player;
    float accumulator;

public:
    void
    Begin() override
    {
        raylib_logo = ResourceManager::GetInstance()->GetTexture("Raylib_logo.png");
        logo_position = {300, 100};

        player.position = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 3};
        player.radius = 25.0f;
        player.color = BLUE;
        player.mass = 1.0f;
        player.inverse_mass = 1 / player.mass;
        player.acceleration = Vector2Zero();
        player.velocity = Vector2Zero();

        sword = ResourceManager::GetInstance()->GetTexture("Sword2.png");
        sword_position = player.position;
        sword_rotation = 0;
        sword_is_active = false;

        shield = ResourceManager::GetInstance()->GetTexture("Shield.png");
        shield_position = player.position;
        shield_is_active = false;

        accumulator = 0;
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

        Vector2 forces = Vector2Zero(); // every frame set the forces to a 0 vector

        // Adds forces with the magnitude of 100 in the direction given by WASD inputs
        if (IsKeyDown(KEY_W))
        {
            forces = Vector2Add(forces, {0, -300});
        }
        if (IsKeyDown(KEY_A))
        {
            forces = Vector2Add(forces, {-300, 0});
        }
        if (IsKeyDown(KEY_S))
        {
            forces = Vector2Add(forces, {0, 300});
        }
        if (IsKeyDown(KEY_D))
        {
            forces = Vector2Add(forces, {300, 0});
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !sword_is_active)
        {
            sword_rotation = GetWeaponAngle(player.position) - 120.0f;
            sword_is_active = true;
        }
        // Enables sword swing, then disables after swinging a specific amount
        if (sword_animation <= 120.0f && sword_is_active)
        {
            sword_animation += 5.0f;
        }
        else
        {
            sword_is_active = false;
            sword_rotation -= sword_animation;
            sword_animation = 0.0f;
        }

        // Shield Controls and Mechanics, how long the shield is active depends on shield_animation
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !shield_is_active)
        {
            shield_is_active = true;
        }
        // Enables sword swing, then disables after swinging a specific amount
        // Adding 1 per frame. Since it's 60 fps, it's a 1 sec duration
        if (shield_animation <= 60.0f && shield_is_active)
        {
            shield_animation += 1.0f;
        }
        else
        {
            shield_is_active = false;
            shield_animation = 0.0f;
        }

        // Does Vector - Scalar multiplication with the sum of all forces and the inverse mass of the ball1
        player.acceleration = Vector2Scale(forces, player.inverse_mass);

        // Physics step
        accumulator += delta_time;
        while (accumulator >= TIMESTEP)
        {
            for (int i = 0; i < 2; i++)
            {
                player.velocity = Vector2Add(player.velocity, Vector2Scale(player.acceleration, TIMESTEP));
                player.velocity = Vector2Subtract(player.velocity, Vector2Scale(player.velocity, FRICTION * player.inverse_mass * TIMESTEP));
                player.position = Vector2Add(player.position, Vector2Scale(player.velocity, TIMESTEP));

                // Negates the velocity at x and y if the object hits a wall. (Basic Collision Detection)
                if (player.position.x + player.radius >= WINDOW_WIDTH || player.position.x - player.radius <= 0)
                {
                    player.velocity.x *= -1;
                }
                if (player.position.y + player.radius >= WINDOW_HEIGHT || player.position.y - player.radius <= 0)
                {
                    player.velocity.y *= -1;
                }
            }
            accumulator -= TIMESTEP;
        }
    }

    void Draw() override
    {
        DrawTexturePro(raylib_logo, {0, 0, 256, 256}, {logo_position.x, logo_position.y, 200, 200}, {0, 0}, 0.0f, WHITE);
        if (sword_is_active)
        {
            DrawTexturePro(sword, {0, 0, 50, 115}, {player.position.x, player.position.y, 25, 64}, {12.5, -32}, sword_rotation + sword_animation, WHITE);
        }
        BeginBlendMode(BLEND_ALPHA);
        DrawCircleV(player.position, player.radius, player.color);
        if (shield_is_active)
        {
            DrawTexturePro(shield, {0, 0, 269, 269}, {player.position.x, player.position.y, player.radius * 3, player.radius * 3}, {player.radius * 1.5f, player.radius * 1.5f}, 0.0f, BLUE);
        }
        EndBlendMode();
    }
};

#endif