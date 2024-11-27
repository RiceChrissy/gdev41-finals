#include <raylib.h>
#include <raymath.h>
#include "entt.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>

#include "scene_manager.hpp"

struct TransformComponent{
    bool isEnabled;
    Vector2 position;
};

struct CircleComponent{
    bool isEnabled;
    float radius;
    Color color;
};

struct BoxCollider2D{
    bool isEnabled;
    Vector2 size;
};

struct CircleCollider2D{
    bool isEnabled;
};

struct PhysicsComponent{
    bool isEnabled;
    float mass;
    float inverse_mass;
    Vector2 acceleration;
    Vector2 velocity;
};

struct SwordComponent{
    bool isRanged;
    bool isAoE;
    int numberOfProjectiles;
    float damage;
    
};

struct ProjectileComponent{
    enum owner{ 
        player, enemy
    };
    owner ownedBy;
    float damage;
};