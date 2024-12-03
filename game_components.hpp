#include <raylib.h>
#include <raymath.h>
#include "entt.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>

#include "scene_manager.hpp"

struct TransformComponent
{
    bool isEnabled;
    Vector2 position;
};

struct CircleComponent
{
    bool isEnabled;
    float radius;
    Color color;
};

struct BoxCollider2D
{
    bool isEnabled;
    float width;
    float height;
    Vector2 size;
    Vector2 position;
};

struct CircleCollider2D
{
    bool isEnabled;
    float radius;
    Vector2 position;
};

struct PhysicsComponent
{
    bool isEnabled;
    float mass;
    float inverse_mass;
    Vector2 forces;
    Vector2 acceleration;
    Vector2 velocity;
};

struct WeaponComponent
{
    bool isRanged;
    bool isAoE;
    int numberOfProjectiles;
    float damage;

    bool is_active;
    float rotation;
    float animation;
};

struct TextureComponent
{
    Texture texture;
};
struct ProjectileComponent
{
    enum owner
    {
        player,
        enemy
    };
    float health;
    owner ownedBy;
    bool isAoE;
    float damage;
};