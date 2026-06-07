#pragma once
#include "raylib.h"
#include <vector>

class Level;
class Player;
class CoinSystem;   // 新增

class Entity {
public:
    virtual ~Entity() {}
    virtual void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) = 0;
    virtual void Draw() = 0;
    virtual Rectangle GetBounds() const = 0;
    virtual bool IsActive() const { return active; }
protected:
    bool active = true;
};

// 工厂函数声明，增加 CoinSystem* 参数
Entity* CreateEntityFromTile(char tile, Vector2 pos, Level& level, int* starCountPtr = nullptr, CoinSystem* coinSys = nullptr);

class Bullet : public Entity {
public:
    Bullet(Vector2 pos, Vector2 dir, float speed);
    ~Bullet();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
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
    Ghost(Vector2 pos, bool vertical = false);
    ~Ghost();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x + 2, position.y, 36, 40 }; }
protected:
    Vector2 position;
private:
    Vector2 dir;
    float speed;
    Texture2D tex;
    bool texLoaded = false;
    int frameIndex;
    float animTimer;
};

class GhostPlus : public Ghost {
public:
    GhostPlus(Vector2 pos, bool vertical = false);
    ~GhostPlus();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x, position.y, 96, 96 }; }
private:
    Texture2D plusTex;
    bool plusTexLoaded;
    int frameIndex;
    float animTimer;
};

class GunTrap : public Entity {
public:
    GunTrap(Vector2 pos, Vector2 dir);
    ~GunTrap();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x, position.y, 32, 32 }; }
private:
    Vector2 position;
    Vector2 dir;
    float shootCooldown;
    Texture2D tex;
    bool texLoaded = false;
};

class TriggerTrap : public Entity {
public:
    TriggerTrap(Vector2 pos, bool horizontal, const char* spritePath, int w, int h);
    ~TriggerTrap();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x, position.y, (float)drawW, (float)drawH }; }
private:
    int drawW, drawH;
    Vector2 position;
    float timer;
    bool triggered;
    bool timerStarted;
    Texture2D tex;
    bool texLoaded = false;
    Texture2D spikeTex;
    bool spikeTexLoaded = false;
    int frameIndex;
    float animTimer;
    float retractTimer;
    bool retracting;
    float spikeRotation = 0.0f;
    bool horizontal;
};

class Decoration : public Entity {
public:
    Decoration(Vector2 pos, const char* spritePath, int frameWidth = 0, float drawSize = 16.0f);
    ~Decoration();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return { position.x, position.y, 16, 16 }; }
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
    int frameWidth;
    float drawSize;
    int frameIndex = 0;
    float animTimer = 0.0f;
};

class FixedTrap : public Entity {
public:
    FixedTrap(Vector2 pos, const char* spritePath = "resources/sprites/Traps/Sharp/Sharp1.png", int w = 24, int h = 38);
    ~FixedTrap();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x, position.y, (float)drawW, (float)drawH }; }
private:
    int drawW, drawH;
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
};

class CoinCollectible : public Entity {
public:
    CoinCollectible(Vector2 pos, const char* spritePath, CoinSystem* coinSys = nullptr);
    ~CoinCollectible();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
    CoinSystem* coinSystem;   // 新增
};

class IceBox : public Entity {
public:
    IceBox(Vector2 pos);
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
};

class StarCollectible : public Entity {
public:
    StarCollectible(Vector2 pos, int* countPtr);
    ~StarCollectible();
    void Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return Rectangle{ position.x, position.y, 32, 32 }; }
private:
    Vector2 position;
    int* countPtr;
    Texture2D tex;
    bool texLoaded = false;
};