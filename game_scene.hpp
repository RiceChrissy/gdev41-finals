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
// #include "quad_tree.hpp"
#include "game_components.hpp"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const float FPS;
extern const float TIMESTEP;
extern const float FRICTION;

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

void InitializeEnemyProjectile(entt::registry &registry, float numberOfProjectiles, int spawnArea)
{
    Vector2 spawnPoint;
    Vector2 direction;
    

    for (int x = 0; x < numberOfProjectiles; x++)
    {

        switch (spawnArea)
        {
        case KEY_UP:
            spawnPoint = Vector2{(float)GetRandomValue(0,800),0};
            break;
        case KEY_RIGHT:
            spawnPoint = Vector2{(float) GetScreenWidth(), (float) GetRandomValue(0, GetScreenHeight())};
            break;
        case KEY_LEFT:
            spawnPoint = Vector2{0, (float) GetRandomValue(0, GetScreenHeight())};
            break;
        case KEY_DOWN:
            spawnPoint = Vector2{ (float) GetRandomValue(0,GetScreenWidth()), (float) GetScreenHeight()};
            break;
        default:
            break;
        }        
        direction = Vector2Normalize(Vector2Subtract( 
                Vector2{
                (float)/* X VALUE:*/ GetRandomValue((GetScreenWidth()/2)-50, (GetScreenWidth()/2)+50), 
                (float)/* Y VALUE:*/ GetRandomValue((GetScreenHeight()/2)-50, (GetScreenHeight()/2)+50)
                },
                spawnPoint));
        entt::entity projectile = registry.create();
        TransformComponent &pos_comp = registry.emplace<TransformComponent>(projectile);
        pos_comp.position = spawnPoint;
        CircleComponent &circ_comp = registry.emplace<CircleComponent>(projectile);
        circ_comp.radius = GetRandomValue(10, 15);
        circ_comp.color = RED;
        PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(projectile);
        ProjectileComponent &proj_comp = registry.emplace<ProjectileComponent>(projectile);
        proj_comp.ownedBy = proj_comp.enemy;
        phys_comp.velocity = Vector2Scale(direction, 200);
        CircleCollider2D &collider = registry.emplace<CircleCollider2D>(projectile);
        collider.position = pos_comp.position;
        collider.radius = circ_comp.radius;     

    }
}

entt::entity InitializePlayer(entt::registry &registry, Vector2 point)
{
    entt::entity Player = registry.create();
    TransformComponent &pos_comp = registry.emplace<TransformComponent>(Player);
    pos_comp.position = point;
    CircleComponent &circ_comp = registry.emplace<CircleComponent>(Player);
    circ_comp.radius = 25.0f;
    circ_comp.color = BLUE;
    PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(Player);
    phys_comp.mass = 1.0f;
    phys_comp.inverse_mass = 1 / phys_comp.mass;
    phys_comp.acceleration = Vector2Zero();
    phys_comp.velocity = Vector2Zero();
    phys_comp.forces = Vector2Zero();
    CircleCollider2D &collider = registry.emplace<CircleCollider2D>(Player);
    collider.position = pos_comp.position;
    collider.radius = circ_comp.radius;
    return Player;
}

entt::entity InitializeWeapon(entt::registry &registry, entt::entity &player)
{
    entt::entity weapon = registry.create();
    TransformComponent &pos_comp = registry.emplace<TransformComponent>(weapon);
    TransformComponent &player_transform = registry.get<TransformComponent>(player);
    pos_comp.position = player_transform.position;

    CircleComponent &circ_comp = registry.emplace<CircleComponent>(weapon);
    circ_comp.radius = 25.0f;
    circ_comp.color = WHITE;

    TextureComponent &text_comp = registry.emplace<TextureComponent>(weapon);
    text_comp.texture = ResourceManager::GetInstance()->GetTexture("Sword2.png");

    WeaponComponent &weap_comp = registry.emplace<WeaponComponent>(weapon);
    weap_comp.is_active = false;
    weap_comp.rotation = 0.0f;
    weap_comp.animation = 0.0f;


    return weapon;
}

class GameScene : public Scene
{
    entt::registry registry;

    entt::entity player = InitializePlayer(registry, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});
    entt::entity sword = InitializeWeapon(registry, player);

    Texture raylib_logo;
    Vector2 logo_position;
    float move_dir_x = 1;
    float move_dir_y = 1;

    // Texture sword;
    // bool sword_is_active;
    // float sword_rotation;
    // float sword_animation;

    Texture shield;
    Vector2 shield_position;
    bool shield_is_active;
    float shield_animation;

    // Player player;
    float accumulator;

    // Player components
    TransformComponent &player_transform = registry.get<TransformComponent>(player);
    CircleComponent &player_circle = registry.get<CircleComponent>(player);
    PhysicsComponent &player_physics = registry.get<PhysicsComponent>(player);
    CircleCollider2D &player_collider = registry.get<CircleCollider2D>(player);

    // Sword components
    WeaponComponent &sword_comp = registry.get<WeaponComponent>(sword);
    TextureComponent &sword_tex = registry.get<TextureComponent>(sword);

public:
    void
    Begin() override
    {

        raylib_logo = ResourceManager::GetInstance()->GetTexture("Raylib_logo.png");
        logo_position = {300, 100};

        // // Creates a player
        // player = InitializePlayer(registry, {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2});

        // sword = ResourceManager::GetInstance()->GetTexture("Sword2.png");
        // sword_position = player_transform.position;
        // sword_rotation = 0;
        // sword_is_active = false;

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

        auto allProjectiles = registry.view<TransformComponent, CircleComponent, PhysicsComponent, ProjectileComponent>();
        Vector2 forces = Vector2Zero(); // every frame set the forces to a 0 vector

        // testing inputs

        if (IsKeyPressed(KEY_UP)){
            InitializeEnemyProjectile(registry, 10, KEY_UP);
            std::cout << "Spawned" << std::endl;
        }

        // PLAYER INPUTS
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !sword_comp.is_active)
        {
            sword_comp.rotation = GetWeaponAngle(player_transform.position) - 120.0f;
            sword_comp.is_active = true;
        }
        // Enables sword swing, then disables after swinging a specific amount
        if (sword_comp.animation <= 120.0f && sword_comp.is_active)
        {
            sword_comp.animation += 5.0f;
        }
        else
        {
            sword_comp.is_active = false;
            sword_comp.rotation -= sword_comp.animation;
            sword_comp.animation = 0.0f;
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
        // PLAYER INPUTS

        // Does Vector - Scalar multiplication with the sum of all forces and the inverse mass of the ball1
        player_physics.acceleration = Vector2Scale(forces, player_physics.inverse_mass);

        // Physics step
        // TODO: Implement quadtrees
        // auto collision_circles = registry.view<CircleComponent>();
        // auto collision_physics = registry.view<PhysicsComponent>();

        auto collisionCircles = registry.view<TransformComponent, CircleComponent, PhysicsComponent, CircleCollider2D>();
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
            for (auto entity : collisionCircles)
            {
                TransformComponent &position = registry.get<TransformComponent>(entity);
                CircleComponent &circle = registry.get<CircleComponent>(entity);
                PhysicsComponent &physics = registry.get<PhysicsComponent>(entity);
                CircleCollider2D &collider = registry.get<CircleCollider2D>(entity);

                physics.velocity = Vector2Add(physics.velocity, Vector2Scale(physics.acceleration, TIMESTEP));
                position.position = Vector2Add(position.position, Vector2Scale(physics.velocity, TIMESTEP));
            }
           accumulator -= TIMESTEP;
        }
    }

    void Draw() override
    {
        DrawTexturePro(raylib_logo, {0, 0, 256, 256}, {logo_position.x, logo_position.y, 200, 200}, {0, 0}, 0.0f, WHITE);
        if (sword_comp.is_active) // Sword swing animation at left click
        {
            DrawTexturePro(sword_tex.texture, {0, 0, 50, 115}, {player_transform.position.x, player_transform.position.y, 25, 64}, {12.5, -32}, sword_comp.rotation + sword_comp.animation, WHITE);
        }
        BeginBlendMode(BLEND_ALPHA);
        DrawCircleV(player_transform.position, player_circle.radius, player_circle.color);
        if (shield_is_active) // Shield animation at right click
        {
            DrawTexturePro(shield, {0, 0, 269, 269}, {player_transform.position.x, player_transform.position.y, player_circle.radius * 3, player_circle.radius * 3}, {player_circle.radius * 1.5f, player_circle.radius * 1.5f}, 0.0f, BLUE);
        }

        auto allProjectiles = registry.view<TransformComponent, CircleComponent, PhysicsComponent, ProjectileComponent>();

        for (auto entity : allProjectiles)
        {
            TransformComponent &pos = registry.get<TransformComponent>(entity);
            CircleComponent &circle = registry.get<CircleComponent>(entity);
            DrawCircleV(pos.position, circle.radius, circle.color);
        }
        EndBlendMode();
    }
};

#endif