#ifndef GAME_SCENE
#define GAME_SCENE

#include <raylib.h>
#include <raymath.h>
#include "entt.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>

#include "scene_manager.hpp"
//#include "quad_tree.hpp"
#include "game_components.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FPS = 60;
const float TIMESTEP = 1 / FPS;
const float FRICTION = 1;

// struct WeaponComponent
// {
// };

float RandomDirection()
{
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // Make it [-1, 1]
    return x * 2.0f - 1.0f;
}

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

entt::entity initializePlayer(entt::registry &registry, Vector2 point){
    entt::entity Player = registry.create();
    TransformComponent &pos_comp = registry.emplace<TransformComponent>(Player);
    pos_comp.position = point;
    CircleComponent &circ_comp = registry.emplace<CircleComponent>(Player);
    PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(Player);
    BoxCollider2D &box_comp = registry.emplace<BoxCollider2D>(Player);
    return Player;
}

void InitializeProjectile(entt::registry &registry, float numberOfProjectiles, Vector2 point)
{
    for (int x = 0; x < numberOfProjectiles; x++)
    {
        entt::entity projectile = registry.create();
        TransformComponent &pos_comp = registry.emplace<TransformComponent>(projectile);
        pos_comp.position = point;
        CircleComponent &circ_comp = registry.emplace<CircleComponent>(projectile);
        circ_comp.radius = GetRandomValue(20, 50);
        PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(projectile);
        ProjectileComponent &proj_comp = registry.emplace<ProjectileComponent>(projectile);
        phys_comp.velocity = {500.0f * RandomDirection(), 500.0f * RandomDirection()};
        circ_comp.color = BLUE;
        circ_comp.color = GRAY;
    }
}

class GameScene : public Scene
{
    entt::registry registry;

    entt::entity player = initializePlayer(registry, Vector2{WINDOW_WIDTH/2,WINDOW_HEIGHT/2});
    

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

    //Player player;
    float accumulator;

    TransformComponent &player_transform = registry.get<TransformComponent>(player);
    CircleComponent &player_circle = registry.get<CircleComponent>(player);
    PhysicsComponent &player_physics = registry.get<PhysicsComponent>(player);
    BoxCollider2D &player_collider = registry.get<BoxCollider2D>(player);

public:
    void
    Begin() override
    {
        
        raylib_logo = ResourceManager::GetInstance()->GetTexture("Raylib_logo.png");
        logo_position = {300, 100};

        player_circle.radius = 25.0f;
        player_circle.color = BLUE;
        player_physics.mass = 1.0f;
        player_physics.inverse_mass = 1 / player_physics.mass;
        player_physics.acceleration = Vector2Zero();
        player_physics.velocity = Vector2Zero();

        sword = ResourceManager::GetInstance()->GetTexture("Sword2.png");
        sword_position = player_transform.position;
        sword_rotation = 0;
        sword_is_active = false;

        shield = ResourceManager::GetInstance()->GetTexture("Shield.png");
        shield_position = player_transform.position;
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
            sword_rotation = GetWeaponAngle(player_transform.position) - 120.0f;
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
        player_physics.acceleration = Vector2Scale(forces, player_physics.inverse_mass);

        // Physics step
        accumulator += delta_time;
        while (accumulator >= TIMESTEP)
        {
            for (int i = 0; i < 2; i++)
            {
                player_physics.velocity = Vector2Add(player_physics.velocity, Vector2Scale(player_physics.acceleration, TIMESTEP));
                player_physics.velocity = Vector2Subtract(player_physics.velocity, Vector2Scale(player_physics.velocity, FRICTION * player_physics.inverse_mass * TIMESTEP));
                player_transform.position = Vector2Add(player_transform.position, Vector2Scale(player_physics.velocity, TIMESTEP));

                // Negates the velocity at x and y if the object hits a wall. (Basic Collision Detection)
                if (player_transform.position.x + player_circle.radius >= WINDOW_WIDTH || player_transform.position.x - player_circle.radius <= 0)
                {
                    player_physics.velocity.x *= -1;
                }
                if (player_transform.position.y + player_circle.radius >= WINDOW_HEIGHT || player_transform.position.y - player_circle.radius <= 0)
                {
                    player_physics.velocity.y *= -1;
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
            DrawTexturePro(sword, {0, 0, 50, 115}, {player_transform.position.x, player_transform.position.y, 25, 64}, {12.5, -32}, sword_rotation + sword_animation, WHITE);
        }
        BeginBlendMode(BLEND_ALPHA);
        DrawCircleV(player_transform.position, player_circle.radius, player_circle.color);
        if (shield_is_active)
        {
            DrawTexturePro(shield, {0, 0, 269, 269}, {player_transform.position.x, player_transform.position.y, player_circle.radius * 3, player_circle.radius * 3}, {player_circle.radius * 1.5f, player_circle.radius * 1.5f}, 0.0f, BLUE);
        }
        EndBlendMode();
    }
};

#endif