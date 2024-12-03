#ifndef GAME_SCENE
#define GAME_SCENE

#include <raylib.h>
#include <raymath.h>
#include "entt.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>
#include <iostream>
#include <fstream>

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
        proj_comp.isAoE = false;
        phys_comp.velocity = Vector2Scale(direction, 200);
        proj_comp.destroyOnContact = false;
        CircleCollider2D &collider = registry.emplace<CircleCollider2D>(projectile);
        collider.position = pos_comp.position;
        collider.radius = circ_comp.radius;
    }
}

void InitializePlayerProjectile(entt::registry &registry, float numberOfProjectiles, entt::entity &player)
{
    Vector2 spawnPoint;
    Vector2 direction;
    

    for (int x = 0; x < numberOfProjectiles; x++)
    {
        spawnPoint = registry.get<TransformComponent>(player).position;
        if(numberOfProjectiles > 1){
            direction = Vector2Normalize(Vector2Subtract( 
                                {(float) GetRandomValue( Vector2SubtractValue(GetMousePosition(),30).x  , Vector2AddValue(GetMousePosition(),30).x), 
                                 (float) GetRandomValue( Vector2SubtractValue(GetMousePosition(),30).y  , Vector2AddValue(GetMousePosition(),30).y)}
                                ,
                                spawnPoint));
        }
        else{
            direction = Vector2Normalize(Vector2Subtract( 
                                GetMousePosition(),
                                spawnPoint));
        }
        
        entt::entity projectile = registry.create();
        TransformComponent &pos_comp = registry.emplace<TransformComponent>(projectile);
        pos_comp.position = spawnPoint;
        CircleComponent &circ_comp = registry.emplace<CircleComponent>(projectile);
        circ_comp.radius = GetRandomValue(10, 15);
        circ_comp.color = PURPLE;
        PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(projectile);
        ProjectileComponent &proj_comp = registry.emplace<ProjectileComponent>(projectile);
        proj_comp.isAoE = false;
        phys_comp.velocity = Vector2Scale(direction, 200);
        proj_comp.destroyOnContact = false;
        CircleCollider2D &collider = registry.emplace<CircleCollider2D>(projectile);
        collider.position = pos_comp.position;
        collider.radius = circ_comp.radius;
    }
}

void InitializeOrbitProjectile(entt::registry &registry, float numberOfProjectiles, entt::entity &player)
{
    Vector2 spawnPoint;
    Vector2 direction;
    

    for (int x = 0; x < numberOfProjectiles; x++)
    {
        entt::entity projectile = registry.create();
        OrbitComponent &orbit = registry.emplace<OrbitComponent>(projectile);
        orbit.orbitRadius = 100;
        orbit.angle = 0.0f;
        spawnPoint = Vector2{registry.get<TransformComponent>(player).position.x, registry.get<TransformComponent>(player).position.y};
        TransformComponent &pos_comp = registry.emplace<TransformComponent>(projectile);
        pos_comp.position = spawnPoint;
        CircleComponent &circ_comp = registry.emplace<CircleComponent>(projectile);
        circ_comp.radius = GetRandomValue(10, 15);
        circ_comp.color = RED;
        PhysicsComponent &phys_comp = registry.emplace<PhysicsComponent>(projectile);
        ProjectileComponent &proj_comp = registry.emplace<ProjectileComponent>(projectile);
        proj_comp.isAoE = false;
        //phys_comp.velocity = Vector2Scale(direction, 200);
        proj_comp.destroyOnContact = false;
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

    TextureComponent &text_comp = registry.emplace<TextureComponent>(weapon);
    text_comp.texture = ResourceManager::GetInstance()->GetTexture("Sword2.png");

    // @Christian
    /*
        For UI every x number of points, we should pause the game and then give the player a choice of 3 upgrades.
    */

    WeaponComponent &weap_comp = registry.emplace<WeaponComponent>(weapon);
    weap_comp.is_active = false;
    weap_comp.rotation = 0.0f;
    weap_comp.animation = 0.0f;
    weap_comp.sizeUpgradeTier = 5; // @Christian sizeUpgradeTier determins weapon size.

    BoxCollider2D &boxCollider = registry.emplace<BoxCollider2D>(weapon);
    boxCollider.colliderBounds.height = 50;
    boxCollider.colliderBounds.width = 20;
    boxCollider.colliderBounds.x = pos_comp.position.x;
    boxCollider.colliderBounds.y = pos_comp.position.y;
    boxCollider.isEnabled = true;

    return weapon;
}

void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint) // Yoinked From Raylib website
{
    if ((texture.id <= 0) || (scale <= 0.0f)) return;
    if ((source.width == 0) || (source.height == 0)) return;

    int tileWidth = (int)(source.width*scale), tileHeight = (int)(source.height*scale);
    if ((dest.width < tileWidth) && (dest.height < tileHeight))
    {
        DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                    (Rectangle){dest.x, dest.y, dest.width, dest.height}, origin, rotation, tint);
    }
    else if (dest.width <= tileWidth)
    {
        int dy = 0;
        for (;dy+tileHeight < dest.height; dy += tileHeight)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, source.height}, (Rectangle){dest.x, dest.y + dy, dest.width, (float)tileHeight}, origin, rotation, tint);
        }
        if (dy < dest.height)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)dest.width/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                        (Rectangle){dest.x, dest.y + dy, dest.width, dest.height - dy}, origin, rotation, tint);
        }
    }
    else if (dest.height <= tileHeight)
    {
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)dest.height/tileHeight)*source.height}, (Rectangle){dest.x + dx, dest.y, (float)tileWidth, dest.height}, origin, rotation, tint);
        }

        if (dx < dest.width)
        {
            DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)dest.height/tileHeight)*source.height},
                        (Rectangle){dest.x + dx, dest.y, dest.width - dx, dest.height}, origin, rotation, tint);
        }
    }
    else
    {
        int dx = 0;
        for (;dx+tileWidth < dest.width; dx += tileWidth)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, source, (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, (float)tileHeight}, origin, rotation, tint);
            }

            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth, dest.height - dy}, origin, rotation, tint);
            }
        }
        if (dx < dest.width)
        {
            int dy = 0;
            for (;dy+tileHeight < dest.height; dy += tileHeight)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, source.height},
                        (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, (float)tileHeight}, origin, rotation, tint);
            }

            if (dy < dest.height)
            {
                DrawTexturePro(texture, (Rectangle){source.x, source.y, ((float)(dest.width - dx)/tileWidth)*source.width, ((float)(dest.height - dy)/tileHeight)*source.height},
                    (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx, dest.height - dy}, origin, rotation, tint);
            }
        }
    }
}



void drawWeapon(entt::registry &registry, entt::entity &player, entt::entity &sword, int scale){
    TransformComponent &player_transform = registry.get<TransformComponent>(player);
    TextureComponent &sword_tex = registry.get<TextureComponent>(sword);
    WeaponComponent &sword_comp = registry.get<WeaponComponent>(sword);
    if (sword_comp.is_active) // Sword swing animation at left click
    {
        float sizeIncrease;
        // @Christian I couldn't figure out how to do sword size because the texture looks funky
        DrawTexturePro(sword_tex.texture, {0, 0, 50, 115}, {player_transform.position.x, player_transform.position.y, 25, 64}, {12.5, -32}, sword_comp.rotation + sword_comp.animation, WHITE);
        //DrawTextureTiled(sword_tex.texture, (Rectangle) {0, 0, 50, 115}, {player_transform.position.x*2, player_transform.position.y*2, 25*2, 64*2}, {12.5, -32}, sword_comp.rotation + sword_comp.animation, 2, WHITE);
    }

}

int ToInt(std::string x){
    char y = x[0];
    return y - '0' + 48;
}

void parseSaveData(int &currentHighestScore , bool &SpeedUpgradeTier, bool &ProjectileUpgradeTier, bool &orbitUpgradeTier){
    std::fstream saveFile;
    saveFile.open("save.txt", std::ios::in);
    std::string line;
    if (saveFile.is_open())
    {
        while (std::getline(saveFile, line)){
            // std::cout << line << std::endl;
            std::string searchTarget;
            if (line.find("HighestScore = ") == 0){
                searchTarget = "HighestScore = ";
                currentHighestScore = std::stoi(line.erase(0, searchTarget.length()));
            }
            if (line.find("SpeedUpgradeUnlocked = ") == 0){
                searchTarget = "SpeedUpgradeUnlocked = ";
                SpeedUpgradeTier = std::stoi(line.erase(0, searchTarget.length()));
            }
            if (line.find("ProjectileUpgradeUnlocked = ") == 0){
                searchTarget = "ProjectileUpgradeUnlocked = ";
                ProjectileUpgradeTier =  std::stoi(line.erase(0, searchTarget.length()));
            }
            if (line.find("orbitUpgradeUnlocked = ") == 0){
                searchTarget = "orbitUpgradeUnlocked = ";
                orbitUpgradeTier =  std::stoi(line.erase(0, searchTarget.length()));
            }
        }
    }
}

void saveData(int &currentHighestScore , bool &SpeedUpgradeTier, bool &ProjectileUpgradeTier, bool &orbitUpgradeTier){
    std::ifstream saveFile;
    saveFile.open("save.txt");
    std::string line;

    std::string updatedContent;
    
    if (saveFile.is_open())
    {
        while (std::getline(saveFile, line)){
            if (line.find("HighestScore = ") == 0){
                line = "HighestScore = " + std::to_string(currentHighestScore);
            }
            else if (line.find("SpeedUpgradeUnlocked = ") == 0){
                line = "SpeedUpgradeUnlocked = " + std::to_string(SpeedUpgradeTier);
            }
            else if (line.find("ProjectileUpgradeUnlocked = ") == 0){
                line = "ProjectileUpgradeUnlocked = " + std::to_string(ProjectileUpgradeTier);
            }
            else if (line.find("orbitUpgradeUnlocked = ") == 0){
                line = "orbitUpgradeUnlocked = " + std::to_string(orbitUpgradeTier);
            }
            updatedContent += line + "\n";
        }
        saveFile.close();

        std::ofstream saveFileOut("save.txt");
        if (!saveFileOut.is_open()) {
            std::cout << "bro you broke it O_O" << std::endl;
            return;
        }
        saveFileOut << updatedContent;
        saveFileOut.close();
    }
    
}

void ScoreManager(int& score, int &currentHighestScore , bool &SpeedUpgradeTier, bool &ProjectileUpgradeTier, bool &orbitUpgradeTier){
    if(score > currentHighestScore){
        currentHighestScore = score;
    }
    if(score >= 100){
        SpeedUpgradeTier = true;
        std::cout << "Speed Upgrade Unlocked!" << std::endl;
    }
    if(score >= 300){
        ProjectileUpgradeTier = true;
        std::cout << "Projectile Upgrade Unlocked!" << std::endl;
    }
    if(score >= 500){
        orbitUpgradeTier = true;
        std::cout << "Orbit Upgrade Unlocked!" << std::endl;
    }
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

    int currentHighestScore = 0;

    bool isSpeedUpgradeUnlocked;
    bool isProjectileUpgradeUnlocked;
    bool isOrbitUpgradeUnlocked;

    int SpeedUpgradeTier = 0;
    int ProjectileUpgradeTier = 0;
    int SizeUpgradeTier = 0;
    int orbitUpgradeTier = 0;

    // Player player;
    float accumulator;
    int score = 0;
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
        
        // Unlocks: 
        

        parseSaveData(currentHighestScore, isSpeedUpgradeUnlocked, isProjectileUpgradeUnlocked, isOrbitUpgradeUnlocked);
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

    void End() override {
        saveData(currentHighestScore, isSpeedUpgradeUnlocked, isProjectileUpgradeUnlocked, isOrbitUpgradeUnlocked);
    }

    void Update() override
    {
        // @Christian put a function to obtain an upgrade. (Not here but somewher lol)
        // std::cout << currentHighestScore << std::endl;
        float delta_time = GetFrameTime();

        ScoreManager(score, currentHighestScore, isSpeedUpgradeUnlocked, isProjectileUpgradeUnlocked, isOrbitUpgradeUnlocked);

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
        auto allOrbitingProjectiles = registry.view<OrbitComponent>();
        Vector2 forces = Vector2Zero(); // every frame set the forces to a 0 vector

        // testing inputs

        if (IsKeyPressed(KEY_UP)){
            score += 100;
        }
        if (IsKeyPressed(KEY_DOWN)){
            score -= 20;
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

        if (IsKeyPressed(KEY_BACKSLASH)){
            saveData(currentHighestScore, isSpeedUpgradeUnlocked, isProjectileUpgradeUnlocked, isOrbitUpgradeUnlocked);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !sword_comp.is_active)
        {
            sword_comp.rotation = GetWeaponAngle(player_transform.position) - 120.0f;
            sword_comp.is_active = true;
            sword_comp.isRanged = true;
            if (sword_comp.isRanged){ // @CHRISTIAN this is the projectile initialization. You can use any of the following initializes as necessary.
            /*
            HOW IT WORKS:
            1. Reach a current score threshold, give the player the choice of upgrades. (if no others are unlocked then size upgrade is default)
            2. Picking an upgrade increases it's tier by 1 each time
            3. Each tier strengthen's the power up (i.e. more projectiles per tier, more orbiting projectiles per tier)
            4. 0 means we don't have that upgrade hence it wont work straight up.
            
            */
                if(ProjectileUpgradeTier > 0){
                    InitializePlayerProjectile(registry, ProjectileUpgradeTier, player);
                }
            }
        }


        auto allOrbits = registry.view<OrbitComponent>();

        if(IsKeyPressed(1)){
            if(orbitUpgradeTier > 0 && allOrbits.size() < orbitUpgradeTier){
                InitializeOrbitProjectile(registry, 1, player);
            }
        }

        // Enables sword swing, then disables after swinging a specific amount
        
        if (sword_comp.animation <= 120.0f && sword_comp.is_active)
        {
            sword_comp.animation += 5.0f + SpeedUpgradeTier;
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
            // @CHRISTIAN:
            /*
                PUT ALL CODE related to collision here in the physics step. On the destruction of any enemy projectile, gain points.
            */
            for (int i = 0; i < 2; i++)
            {
                player_physics.velocity = Vector2Add(player_physics.velocity, Vector2Scale(player_physics.acceleration, TIMESTEP));
                player_physics.velocity = Vector2Subtract(player_physics.velocity, Vector2Scale(player_physics.velocity, FRICTION * player_physics.inverse_mass * TIMESTEP));
                player_transform.position = Vector2Add(player_transform.position, Vector2Scale(player_physics.velocity, TIMESTEP));
                // Negates the velocity at x and y if the object hits a wall. (Basic Collision Detection)
                ////////////////////////////////////////////////////////
                if (player_transform.position.x - player_circle.radius <= 0)
                {
                    player_transform.position.x = player_circle.radius;
                    player_physics.velocity.x *= -1;
                }
                if (player_transform.position.x + player_circle.radius >= WINDOW_WIDTH)
                {
                    player_transform.position.x = WINDOW_WIDTH - player_circle.radius;
                    player_physics.velocity.x *= -1;
                }
                if (player_transform.position.y - player_circle.radius <= 0)
                {
                    player_transform.position.y = player_circle.radius;
                    player_physics.velocity.y *= -1;
                }
                if (player_transform.position.y + player_circle.radius >= WINDOW_HEIGHT)
                {
                    player_transform.position.y = WINDOW_HEIGHT - player_circle.radius;
                    player_physics.velocity.y *= -1;
                }
            }
            for (auto entity : collisionCircles)
            {
                TransformComponent &transform = registry.get<TransformComponent>(entity);
                CircleComponent &circle = registry.get<CircleComponent>(entity);
                PhysicsComponent &physics = registry.get<PhysicsComponent>(entity);
                CircleCollider2D &collider = registry.get<CircleCollider2D>(entity);

                collider.position = transform.position;

                physics.velocity = Vector2Add(physics.velocity, Vector2Scale(physics.acceleration, TIMESTEP));
                transform.position = Vector2Add(transform.position, Vector2Scale(physics.velocity, TIMESTEP));
                // std::cout << "collider position: " << collider.position.x << " " << collider.position.y << std::endl;
            }

            for (auto entity : allOrbitingProjectiles){
                TransformComponent &transform = registry.get<TransformComponent>(entity);
                CircleComponent &circle = registry.get<CircleComponent>(entity);
                OrbitComponent &orbit = registry.get<OrbitComponent>(entity);

                orbit.angle += 4;
                if(orbit.angle >= 360){
                    orbit.angle = 0;
                }
                transform.position = {
                    player_transform.position.x + orbit.orbitRadius * cosf(DEG2RAD * orbit.angle),  
                    player_transform.position.y + orbit.orbitRadius * sinf(DEG2RAD * orbit.angle)   
                };
            }
           accumulator -= TIMESTEP;
        }
    }

    void Draw() override
    {
        //DrawTexturePro(raylib_logo, {0, 0, 256, 256}, {logo_position.x, logo_position.y, 200, 200}, {0, 0}, 0.0f, WHITE);
        drawWeapon(registry, player, sword, 0);
        BeginBlendMode(BLEND_ALPHA);
        DrawCircleV(player_transform.position, player_circle.radius, player_circle.color);
        if (shield_is_active) // Shield animation at right click
        {
            DrawTexturePro(shield, {0, 0, 269, 269}, {player_transform.position.x, player_transform.position.y, player_circle.radius * 3, player_circle.radius * 3}, {player_circle.radius * 1.5f, player_circle.radius * 1.5f}, 0.0f, BLUE);
        }

        auto allProjectiles = registry.view<TransformComponent, CircleComponent, PhysicsComponent, ProjectileComponent>();
        auto allOrbit = registry.view<TransformComponent, CircleComponent, OrbitComponent>();

        for (auto entity : allProjectiles)
        {
            TransformComponent &pos = registry.get<TransformComponent>(entity);
            CircleComponent &circle = registry.get<CircleComponent>(entity);
            DrawCircleV(pos.position, circle.radius, circle.color);
        }

        for (auto entity : allOrbit)
        {
            TransformComponent &pos = registry.get<TransformComponent>(entity);
            CircleComponent &circle = registry.get<CircleComponent>(entity);
            DrawCircleV(pos.position, circle.radius, circle.color);
        }
        
        const char* totalScore = std::to_string(score).c_str();
        DrawText(totalScore, 5, 0, 30, BLACK);


        EndBlendMode();
    }
};

#endif