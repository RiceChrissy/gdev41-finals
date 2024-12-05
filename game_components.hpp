#ifndef GAME_COMPONENTS_HPP
#define GAME_COMPONENTS_HPP

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
struct EnemyProjectileComponent
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

struct PlayerProjectileComponent
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

// UI

int width = 800, height = 600;

struct UIComponent
{
    Rectangle bounds;

    virtual void Draw() = 0;

    virtual bool HandleClick(Vector2 click_position) = 0;
};

struct UIContainer : public UIComponent
{
    std::vector<UIComponent *> children;

    void AddChild(UIComponent *child)
    {
        children.push_back(child);
    }

    void Draw() override
    {

        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->Draw();
        }
    }
    bool HandleClick(Vector2 click_position) override
    {
        for (size_t i = children.size(); i > 0; --i)
        {
            if (children[i - 1]->HandleClick(click_position))
            {
                return true;
            }
        }

        return false;
    }
};

struct UILibrary
{
    // Root container
    UIContainer root_container;

    // Updates the current UI state
    void Update()
    {
        // If the left mouse button was released, we handle the click from the root container
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            root_container.HandleClick(GetMousePosition());
        }
    }

    // Draw
    void Draw()
    {
        root_container.Draw();
    }
};

struct BasicButton : public UIComponent
{
    std::string text;
    bool isActive = true;
    bool isClicked = false;

    void Draw()
    {
        DrawRectangleRec(bounds, GRAY);
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    bool HandleClick(Vector2 click_position) override
    {
        if (CheckCollisionPointRec(click_position, bounds) && isActive)
        {
            isClicked = true;
            return true;
        }
        isClicked = false;
        return false;
    }
};

struct Button : public UIComponent
{
    std::string text;
    bool isActive = true;
    bool isClicked = false;
    bool hasBorder = false;
    Color color;
    Color textColor;

    void Draw()
    {
        if (isActive)
        {
            if (hasBorder)
            {
                DrawRectangle(bounds.x - 5, bounds.y - 5, bounds.width + 10, bounds.height + 10, BLACK);
            }
            DrawRectangleRec(bounds, color);
            DrawText(text.c_str(), bounds.x, bounds.y, 14, textColor);
        }
    }

    bool HandleClick(Vector2 click_position) override
    {
        if (CheckCollisionPointRec(click_position, bounds) && isActive)
        {
            isClicked = true;
            return true;
        }
        isClicked = false;
        return false;
    }
};

struct CheckBox : public UIComponent
{
    bool isChecked = false;
    std::string text;

    void Draw() override
    {
        DrawRectangleRec(bounds, GRAY);
        if (isChecked == true)
        {
            DrawRectangleRec(bounds, BLACK);
        }
        DrawText(text.c_str(), bounds.x + (bounds.width) + 10, bounds.y, 14, BLACK);
    }

    void Check()
    {
        isChecked = !isChecked;
    }
    bool HandleClick(Vector2 click_position) override
    {
        if (CheckCollisionPointRec(click_position, bounds))
        {
            Check();
            return true;
        }
        return false;
    }
};

struct Label : public UIComponent
{
    std::string text;

    // Draw
    void Draw() override
    {
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }
    bool HandleClick(Vector2 click_position) override
    {
        return false;
    }
};

#endif