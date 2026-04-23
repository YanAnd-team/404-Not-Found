#pragma once
#include "raylib.h"
#include <vector>

class Level;
class Player;

class Entity {
public:
    virtual ~Entity() {}
    virtual void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) = 0;
    virtual void Draw() = 0;
    virtual Rectangle GetBounds() const = 0;
    virtual bool IsActive() const { return active; }
protected:
    bool active = true;
};

// Factory: create entity from tile char
Entity* CreateEntityFromTile(char tile, Vector2 pos, Level &level);

// Simple bullet entity
class Bullet : public Entity {
public:
    Bullet(Vector2 pos, Vector2 dir, float speed);
    ~Bullet();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Vector2 direction;
    float speed;
    Texture2D tex;
    bool texLoaded = false;
};

class Ghost : public Entity {
public:
    Ghost(Vector2 pos, bool vertical=false);
    ~Ghost();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Vector2 dir;
    float speed;
    Texture2D tex;
    bool texLoaded = false;
};

class GhostPlus : public Ghost {
public:
    GhostPlus(Vector2 pos, bool vertical=false);
    ~GhostPlus();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
private:
    float blinkTimer;
    bool visible;
};

// GunTrap is implemented in Entities.cpp (concrete definition not exposed here)

class TriggerTrap : public Entity {
public:
    TriggerTrap(Vector2 pos);
    ~TriggerTrap();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    float timer;
    bool triggered;
    Texture2D tex;
    bool texLoaded=false;
};

class FixedTrap : public Entity {
public:
    FixedTrap(Vector2 pos);
    ~FixedTrap();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded=false;
};

// GunTrap is implemented privately in Entities.cpp
