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
    Rectangle colliderBounds;
    bool isEnabled;
};

struct CircleCollider2D
{
    bool isEnabled;
    float radius;
    Vector2 position;

    float getDistance(CircleCollider2D collider1, CircleCollider2D collider2)
    {
        Vector2 dist = Vector2Subtract(collider1.position, collider2.position);
        return std::abs(Vector2Length(dist));
    }

    float getDistanceToPoint(CircleCollider2D collider1, Vector2 pos)
    {
        Vector2 dist = Vector2Subtract(collider1.position, pos);
        return std::abs(Vector2Length(dist));
    }

    bool isCircleCircleColliding(CircleCollider2D collider1, CircleCollider2D collider2)
    {
        float sumOfRadii = collider1.radius + collider2.radius;
        float distance = getDistance(collider1, collider2);
        if (distance <= sumOfRadii)
        {
            return true;
        }
        return false;
    }
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
    int orbitUpgradeTier;
    int projectileUpgradeTier;
    int sizeUpgradeTier;
    int speedUpgradeTier;

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
    bool destroyOnContact;
};

struct OrbitComponent
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
    bool destroyOnContact;

    float orbitRadius;
    float orbitSpeed;
    float angle = 0.0f;
};