#pragma once
#include "raylib.h"
#include <vector>

class Level;
class Player;

class Entity {
public:
    virtual ~Entity() {}
    virtual void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) = 0;  //Update logic each frame
    virtual void Draw() = 0;                        //Draw the entity
    virtual Rectangle GetBounds() const = 0;        //Return collision rectangle
    virtual bool IsActive() const { return active; }//Return false when the entity should be removed
protected:
    bool active = true;
};

Entity* CreateEntityFromTile(char tile, Vector2 pos, Level &level, int* starCountPtr = nullptr); //Factory: map tile char to the correct entity type

class Bullet : public Entity {
public:
    Bullet(Vector2 pos, Vector2 dir, float speed);  //Spawned by GunTrap; moves in one direction until hitting a wall or the player
    ~Bullet();                                       //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Move bullet; deactivate on wall hit or player collision
    void Draw() override;                            //Draw bullet sprite
    Rectangle GetBounds() const override;            //Return 8x8 collision rectangle
private:
    Vector2 position;
    Vector2 direction;
    float speed;
    Texture2D tex;
    bool texLoaded = false;
};

class Ghost : public Entity {
public:
    Ghost(Vector2 pos, bool vertical=false);    //Patrol enemy; bounces off walls, kills on contact
    ~Ghost();                                   //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Move and animate; reverse on wall hit, reset player on contact
    void Draw() override;                       //Draw current animation frame
    Rectangle GetBounds() const override;       //Return collision rectangle with slight horizontal inset
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
    GhostPlus(Vector2 pos, bool vertical=false);    //Ghost variant with a different sprite; always visible
    ~GhostPlus();                                   //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
private:
    Texture2D plusTex;
    bool plusTexLoaded;
    int frameIndex;
    float animTimer;
};

class GunTrap : public Entity {
public:
    GunTrap(Vector2 pos, Vector2 dir);  //Fires a bullet every 2s in the given direction
    ~GunTrap();                         //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Detect fire direction once, then spawn bullets on cooldown
    void Draw() override;                   //Draw trap sprite
    Rectangle GetBounds() const override;   //Return 32x32 collision rectangle
private:
    Vector2 position;
    Vector2 dir;
    float shootCooldown;
    Texture2D tex;
    bool texLoaded = false;
};

class TriggerTrap : public Entity {
public:
    TriggerTrap(Vector2 pos, const char* spritePath = "resources/sprites/Traps/Sharp/Spike-trap-spike.png"); //Activates when player steps on this tile; spikes out after 0.6s, holds, then retracts
    ~TriggerTrap();             //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Check if player is on this tile, run spike animation and player kill
    void Draw() override;                   //Draw current spike animation frame
    Rectangle GetBounds() const override;   //Return 32x32 collision rectangle
private:
    Vector2 position;
    float timer;
    bool triggered;
    bool timerStarted;
    Texture2D tex;
    bool texLoaded = false;
    int frameIndex;
    float animTimer;
    float retractTimer;
    bool retracting;
};

class Decoration : public Entity {
public:
    Decoration(Vector2 pos, const char* spritePath, int frameWidth = 0);
    ~Decoration();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override { return {position.x + 8, position.y + 8, 16, 16}; }
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
    int frameWidth;
    int frameIndex = 0;
    float animTimer = 0.0f;
};

class FixedTrap : public Entity {
public:
    FixedTrap(Vector2 pos, const char* spritePath = "resources/sprites/Traps/Sharp/Sharp1.png"); //Static spike; kills on contact
    ~FixedTrap();           //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Reset player on contact
    void Draw() override;                   //Draw spike sprite
    Rectangle GetBounds() const override;   //Return 32x32 collision rectangle
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
};

class CoinCollectible : public Entity {
public:
    CoinCollectible(Vector2 pos, const char* spritePath); //Coin; disappears when collected
    ~CoinCollectible();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
};

class IceBox : public Entity {
public:
    IceBox(Vector2 pos);
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Texture2D tex;
    bool texLoaded = false;
};

class StarCollectible : public Entity {
public:
    StarCollectible(Vector2 pos, int* countPtr);    //Collected on contact; increments star count up to 3
    ~StarCollectible();                             //Unload texture
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;  //Deactivate and increment count when player overlaps
    void Draw() override;                   //Draw star sprite
    Rectangle GetBounds() const override;   //Return 32x32 collision rectangle
private:
    Vector2 position;
    int* countPtr;
    Texture2D tex;
    bool texLoaded = false;
};
