#include "Entities.h"
#include "Player.h"
#include "Level.h"
#include <math.h>
#include <algorithm>

// --- Bullet ---
Bullet::Bullet(Vector2 pos, Vector2 dir, float speed)
{
    position = pos;
    direction = dir;
    this->speed = speed;
    texLoaded = false;
    if (FileExists("resources/sprites/Bullet.png"))
    {
        tex = LoadTexture("resources/sprites/Bullet.png");
        texLoaded = true;
    }
}

Bullet::~Bullet()
{
    if (texLoaded) UnloadTexture(tex);
}

void Bullet::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    position.x += direction.x * speed * dt;
    position.y += direction.y * speed * dt;
    // simple lifetime: deactivate if out of level bounds
    Rectangle bounds = level.GetWorldBounds();
    if (position.x < bounds.x || position.y < bounds.y || position.x > bounds.x + bounds.width || position.y > bounds.y + bounds.height)
    {
        active = false; return;
    }

    // deactivate if hits a wall tile
    int btx = (int)((position.x + 4) / level.GetTileSize());
    int bty = (int)((position.y + 4) / level.GetTileSize());
    char bt = level.GetTileAt(btx, bty);
    if (bt == '1' || bt == '2' || bt == (char)Empty)
    {
        active = false; return;
    }

    // collision with player
    Rectangle pr = player.GetBounds();
    Rectangle br = GetBounds();
    if (CheckCollisionRecs(pr, br))
    {
        // reset player
        player.Reset();
        active = false;
    }
}

void Bullet::Draw()
{
    Rectangle dest = { position.x, position.y, 8, 8 };
    if (texLoaded)
    {
        // draw first frame 8x8 or scale if sheet is larger
        DrawTexturePro(tex, Rectangle{0,0,8.0f,8.0f}, dest, Vector2{0,0}, 0, WHITE);
    }
    else DrawRectangleRec(dest, YELLOW);
}

Rectangle Bullet::GetBounds() const
{
    return Rectangle{ position.x, position.y, 8, 8 };
}

// --- Ghost ---
Ghost::Ghost(Vector2 pos, bool vertical)
{
    position = pos;
    speed = 60.0f;
    if (vertical) dir = {0,1}; else dir = {1,0};
    texLoaded = false;
    if (FileExists("resources/sprites/Ghost.png")) 
    { 
        tex = LoadTexture("resources/sprites/Ghost.png"); 
        texLoaded = true; 
    }
}

Ghost::~Ghost()
{
    if (texLoaded) UnloadTexture(tex);
}

void Ghost::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    // move and reverse on hitting wall or out of bounds
    Vector2 newPos = { position.x + dir.x * speed * dt, position.y + dir.y * speed * dt };
    // check tile at center
    int tx = (int)( (newPos.x + 8) / level.GetTileSize() );
    int ty = (int)( (newPos.y + 8) / level.GetTileSize() );
    char t = level.GetTileAt(tx, ty);
    if (t == (char)Wall1 || t == (char)Wall2 || t == (char)Empty)
    {
        // reverse
        dir.x = -dir.x; dir.y = -dir.y;
    }
    else
    {
        position = newPos;
    }

    // collision with player
    Rectangle pr = player.GetBounds();
    if (CheckCollisionRecs(pr, GetBounds()))
    {
        player.Reset();
    }
}

void Ghost::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,32.0f,32.0f}, dest, Vector2{0,0}, 0, WHITE);
    else DrawRectangleRec(dest, MAGENTA);
}

Rectangle Ghost::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

// --- GhostPlus ---
GhostPlus::GhostPlus(Vector2 pos, bool vertical) : Ghost(pos, vertical)
{
    blinkTimer = 0.0f;
    visible = true;
}

GhostPlus::~GhostPlus() {}

void GhostPlus::Draw()
{
    if (visible) Ghost::Draw();
}

void GhostPlus::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    blinkTimer += dt;
    if (blinkTimer >= 2.0f)
    {
        visible = !visible;
        blinkTimer = 0.0f;
    }
    if (visible)
    {
        Ghost::Update(dt, player, entities, level);
    }
}

// --- GunTrap (local implementation) ---
class GunTrap : public Entity {
public:
    GunTrap(Vector2 pos);
    ~GunTrap();
    void Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level) override;
    void Draw() override;
    Rectangle GetBounds() const override;
private:
    Vector2 position;
    Vector2 dir;
    float shootCooldown;
    Texture2D tex;
    bool texLoaded=false;
};

GunTrap::GunTrap(Vector2 pos)
{
    position = pos;
    shootCooldown = 2.0f; // initial delay before first shot
    dir = {0,0};
    texLoaded = false;
    if (FileExists("resources/sprites/GunTrap.png")) { tex = LoadTexture("resources/sprites/GunTrap.png"); texLoaded = true; }
}

GunTrap::~GunTrap() { if (texLoaded) UnloadTexture(tex); }

void GunTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    // decide direction on creation if unknown
    if (dir.x==0 && dir.y==0)
    {
        // check four directions for empty tile
        int tx = (int)(position.x / level.GetTileSize());
        int ty = (int)(position.y / level.GetTileSize());
        if (level.GetTileAt(tx+1, ty) == '0') dir = {1,0};
        else if (level.GetTileAt(tx-1, ty) == '0') dir = {-1,0};
        else if (level.GetTileAt(tx, ty+1) == '0') dir = {0,1};
        else if (level.GetTileAt(tx, ty-1) == '0') dir = {0,-1};
    }

    shootCooldown -= dt;
    if (shootCooldown <= 0.0f)
    {
        shootCooldown = 2.0f; // fire every 2.0s
        if (dir.x!=0 || dir.y!=0)
        {
            Vector2 bpos = { position.x + dir.x * 8.0f, position.y + dir.y * 8.0f };
            Vector2 d = dir;
            Bullet *b = new Bullet(bpos, d, 200.0f);
            entities.push_back(b);
        }
    }
}

void GunTrap::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,32.0f,32.0f}, dest, Vector2{0,0}, 0, WHITE);
    else DrawRectangleRec(dest, BROWN);
}

Rectangle GunTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

Entity* CreateGunTrap(Vector2 pos, Level &level)
{
    return new GunTrap(pos);
}

// --- TriggerTrap ---
TriggerTrap::TriggerTrap(Vector2 pos)
{
    position = pos;
    timer = 0.0f;
    triggered = false;
    texLoaded = false;
    if (FileExists("resources/sprites/SpikeTrap.png")) { tex = LoadTexture("resources/sprites/SpikeTrap.png"); texLoaded = true; }
}

TriggerTrap::~TriggerTrap() { if (texLoaded) UnloadTexture(tex); }

void TriggerTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    // if player adjacent (4-neighbors), start timer
    int tx = (int)(position.x / level.GetTileSize());
    int ty = (int)(position.y / level.GetTileSize());
    int px = (int)(player.position.x / level.GetTileSize());
    int py = (int)(player.position.y / level.GetTileSize());
    int dx = abs(px - tx);
    int dy = abs(py - ty);
    if ((dx==1 && dy==0) || (dx==0 && dy==1))
    {
        timer += dt;
        if (timer >= 1.0f) triggered = true;
    }
    else
    {
        timer = 0.0f;
    }

    if (triggered)
    {
        // dangerous: check collision with player
        Rectangle pr = player.GetBounds();
        if (CheckCollisionRecs(pr, GetBounds()))
        {
            player.Reset();
        }
    }
}

void TriggerTrap::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,(float)tex.width,(float)tex.height}, dest, Vector2{0,0}, 0, WHITE);
    else DrawRectangleRec(dest, RED);
}

Rectangle TriggerTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

// --- FixedTrap ---
FixedTrap::FixedTrap(Vector2 pos)
{
    position = pos;
    texLoaded = false;
    if (FileExists("resources/sprites/Spike.png")) { tex = LoadTexture("resources/sprites/Spike.png"); texLoaded = true; }
}

FixedTrap::~FixedTrap() { if (texLoaded) UnloadTexture(tex); }

void FixedTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    Rectangle pr = player.GetBounds();
    if (CheckCollisionRecs(pr, GetBounds()))
    {
        // touching counts
        player.Reset();
    }
}

void FixedTrap::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,(float)tex.width,(float)tex.height}, dest, Vector2{0,0}, 0, WHITE);
    else DrawRectangleRec(dest, ORANGE);
}

Rectangle FixedTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }
// forward declaration for helper
Entity* CreateGunTrap(Vector2 pos, Level &level);

// Factory implementation
Entity* CreateEntityFromTile(char tile, Vector2 pos, Level &level)
{
    switch (tile)
    {
    case '3': return new TriggerTrap(pos);
    case '4': return new FixedTrap(pos);
    case '5': return CreateGunTrap(pos, level);
    case '6':
    {
        bool horiz = false;
        int tx = (int)(pos.x / level.GetTileSize());
        int ty = (int)(pos.y / level.GetTileSize());
        if (level.GetTileAt(tx-1,ty) == '0' || level.GetTileAt(tx+1,ty) == '0') horiz = true;
        return new Ghost(pos, !horiz ? true : false);
    }
    case '7':
    {
        bool horiz = false;
        int tx = (int)(pos.x / level.GetTileSize());
        int ty = (int)(pos.y / level.GetTileSize());
        if (level.GetTileAt(tx-1,ty) == '0' || level.GetTileAt(tx+1,ty) == '0') horiz = true;
        return new GhostPlus(pos, !horiz ? true : false);
    }
    default: return nullptr;
    }
}
