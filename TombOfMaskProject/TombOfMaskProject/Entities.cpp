#include "Entities.h"
#include "Player.h"
#include "Level.h"

// --- Bullet ---
Bullet::Bullet(Vector2 pos, Vector2 dir, float speed)
{
    position = pos;
    direction = dir;
    this->speed = speed;
    texLoaded = false;
    if (FileExists("resources/sprites/Traps/Arrow Trap/Arrow.png"))
    {
        tex = LoadTexture("resources/sprites/Traps/Arrow Trap/Arrow.png");
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

    Rectangle worldBounds = level.GetWorldBounds();
    if (position.x < worldBounds.x || position.y < worldBounds.y ||
        position.x > worldBounds.x + worldBounds.width || position.y > worldBounds.y + worldBounds.height)
    {
        active = false; return;
    }

    int bulletTileX = (int)((position.x + 4) / level.GetTileSize());
    int bulletTileY = (int)((position.y + 4) / level.GetTileSize());
    char bulletTile = level.GetTileAt(bulletTileX, bulletTileY);
    if (bulletTile == '1' || bulletTile == '2' || bulletTile == '-')
    {
        active = false; return;
    }

    Rectangle playerRect = player.GetBounds();
    Rectangle bulletRect = GetBounds();
    if (CheckCollisionRecs(playerRect, bulletRect))
    {
        player.Reset();
        active = false;
    }
}

void Bullet::Draw()
{
    Rectangle dest = { position.x, position.y, 8, 8 };
    if (texLoaded)
        DrawTexturePro(tex, Rectangle{0,0,16.0f,16.0f}, dest, Vector2{0,0}, 0, WHITE);
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
    if (FileExists("resources/sprites/Enemy/Monster1.png"))
    {
        tex = LoadTexture("resources/sprites/Enemy/Monster1.png");
        texLoaded = true;
    }
    frameIndex = 0;
    animTimer = 0.0f;
}

Ghost::~Ghost()
{
    if (texLoaded) UnloadTexture(tex);
}

void Ghost::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    animTimer += dt;
    if (animTimer >= 0.1f)
    {
        animTimer = 0.0f;
        int totalFrames = texLoaded ? (tex.width / 32) : 1;
        frameIndex = (frameIndex + 1) % totalFrames;
    }

    Vector2 newPos = { position.x + dir.x * speed * dt, position.y + dir.y * speed * dt };
    int tileX = (int)((newPos.x + 8) / level.GetTileSize());
    int tileY = (int)((newPos.y + 8) / level.GetTileSize());
    char tile = level.GetTileAt(tileX, tileY);
    if (tile == '1' || tile == 'i' || tile == '-')
    {
        dir.x = -dir.x; dir.y = -dir.y; //Reverse direction on wall hit
    }
    else
    {
        position = newPos;
    }

    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void Ghost::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
    {
        Rectangle src = { (float)(frameIndex * 32), 0, 32.0f, (float)tex.height };
        DrawTexturePro(tex, src, dest, Vector2{0,0}, 0, WHITE);
    }
    else DrawRectangleRec(dest, MAGENTA);
}

Rectangle Ghost::GetBounds() const { return Rectangle{ position.x + 3, position.y, 26, 32 }; }

// --- GhostPlus ---
GhostPlus::GhostPlus(Vector2 pos, bool vertical) : Ghost(pos, vertical)
{
    plusTexLoaded = false;
    if (FileExists("resources/sprites/Enemy/Monster2.png"))
    {
        plusTex = LoadTexture("resources/sprites/Enemy/Monster2.png");
        plusTexLoaded = true;
    }
    frameIndex = 0;
    animTimer = 0.0f;
}

GhostPlus::~GhostPlus()
{
    if (plusTexLoaded) UnloadTexture(plusTex);
}

void GhostPlus::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    animTimer += dt;
    if (animTimer >= 0.1f)
    {
        animTimer = 0.0f;
        int totalFrames = plusTexLoaded ? (plusTex.width / 32) : 1;
        frameIndex = (frameIndex + 1) % totalFrames;
    }

    Ghost::Update(dt, player, entities, level);
}

void GhostPlus::Draw()
{
    if (plusTexLoaded)
    {
        Rectangle src  = { (float)(frameIndex * 32), 0, 32.0f, (float)plusTex.height };
        Rectangle dest = { position.x, position.y, 32, 32 };
        DrawTexturePro(plusTex, src, dest, Vector2{0,0}, 0, WHITE);
    }
    else Ghost::Draw();
}

// --- StarCollectible ---
StarCollectible::StarCollectible(Vector2 pos, int* countPtr)
{
    position = pos;
    this->countPtr = countPtr;
    texLoaded = false;
    if (FileExists("resources/sprites/Stars and coins/Star.png"))
    {
        tex = LoadTexture("resources/sprites/Stars and coins/Star.png");
        texLoaded = true;
    }
}

StarCollectible::~StarCollectible()
{
    if (texLoaded) UnloadTexture(tex);
}

void StarCollectible::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
    {
        if (countPtr && *countPtr < 3) (*countPtr)++;
        active = false;
    }
}

void StarCollectible::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
        DrawTexturePro(tex, Rectangle{0,0,(float)tex.width,(float)tex.height}, dest, Vector2{0,0}, 0, WHITE);
    else
        DrawRectangleRec(dest, YELLOW);
}

Rectangle StarCollectible::GetBounds() const
{
    return Rectangle{ position.x, position.y, 32, 32 };
}

// --- GunTrap ---
GunTrap::GunTrap(Vector2 pos, Vector2 initialDir)
{
    position = pos;
    shootCooldown = 2.0f;
    dir = initialDir;
    texLoaded = false;
    if (FileExists("resources/sprites/Traps/Arrow Trap/Arrow_trap.png")) { tex = LoadTexture("resources/sprites/Traps/Arrow Trap/Arrow_trap.png"); texLoaded = true; }
}

GunTrap::~GunTrap() { if (texLoaded) UnloadTexture(tex); }

void GunTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    shootCooldown -= dt;
    if (shootCooldown <= 0.0f)
    {
        shootCooldown = 2.0f;
        if (dir.x != 0 || dir.y != 0)
        {
            Vector2 bulletPos = { position.x + dir.x * 8.0f, position.y + dir.y * 8.0f };
            entities.push_back(new Bullet(bulletPos, dir, 200.0f));
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

// --- TriggerTrap ---
TriggerTrap::TriggerTrap(Vector2 pos, const char* spritePath)
{
    position = pos;
    timer = 0.0f;
    triggered = false;
    timerStarted = false;
    texLoaded = false;
    if (FileExists(spritePath)) { tex = LoadTexture(spritePath); texLoaded = true; }
    frameIndex = 0;
    animTimer = 0.0f;
    retractTimer = 0.0f;
    retracting = false;
}

TriggerTrap::~TriggerTrap() { if (texLoaded) UnloadTexture(tex); }

void TriggerTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
//Activate when player steps on this tile; animate spike out after 0.6s delay, hold 1.5s, then retract
{
    int tileX = (int)(position.x / level.GetTileSize());
    int tileY = (int)(position.y / level.GetTileSize());
    int playerTileX = (int)(player.position.x / level.GetTileSize());
    int playerTileY = (int)(player.position.y / level.GetTileSize());

    if (!triggered)
    {
        if (!timerStarted && playerTileX == tileX && playerTileY == tileY)
            timerStarted = true;

        if (timerStarted)
        {
            timer += dt;
            if (timer >= 0.6f) triggered = true;
        }
    }

    if (triggered)
    {
        if (!retracting && frameIndex < 4)
        {
            animTimer += dt;
            if (animTimer >= 0.05f) { animTimer = 0.0f; frameIndex++; }
        }
        else if (!retracting && frameIndex == 4)
        {
            retractTimer += dt;
            if (retractTimer >= 1.5f) { retractTimer = 0.0f; retracting = true; }
        }
        else if (retracting && frameIndex > 0)
        {
            animTimer += dt;
            if (animTimer >= 0.05f) { animTimer = 0.0f; frameIndex--; }
        }
        else if (retracting && frameIndex == 0)
        {
            triggered = false;
            retracting = false;
            timer = 0.0f;
            timerStarted = false;
        }

        if (!retracting)
        {
            if (CheckCollisionRecs(player.GetBounds(), GetBounds())) player.Reset();
        }
    }
}

void TriggerTrap::Draw()
{
    if (frameIndex == 0 && !triggered) return; // hidden when dormant
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
    {
        int totalFrames = tex.width / 32;
        if (totalFrames < 1) totalFrames = 1;
        int fi = frameIndex < totalFrames ? frameIndex : totalFrames - 1;
        Rectangle src = { (float)(fi * 32), 0, 32.0f, (float)tex.height };
        DrawTexturePro(tex, src, dest, Vector2{0,0}, 0, WHITE);
    }
    else DrawRectangleRec(dest, RED);
}

Rectangle TriggerTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

// --- FixedTrap ---
FixedTrap::FixedTrap(Vector2 pos, const char* spritePath)
{
    position = pos;
    texLoaded = false;
    if (FileExists(spritePath)) { tex = LoadTexture(spritePath); texLoaded = true; }
}

FixedTrap::~FixedTrap() { if (texLoaded) UnloadTexture(tex); }

void FixedTrap::Update(float dt, Player &player, std::vector<Entity*> &entities, Level &level)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void FixedTrap::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,(float)tex.width,(float)tex.height}, dest, Vector2{0,0}, 0, WHITE);
    else DrawRectangleRec(dest, ORANGE);
}

Rectangle FixedTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

// --- CoinCollectible ---
CoinCollectible::CoinCollectible(Vector2 pos, const char* spritePath)
{
    position = pos;
    if (FileExists(spritePath)) { tex = LoadTexture(spritePath); texLoaded = true; }
}
CoinCollectible::~CoinCollectible() { if (texLoaded) UnloadTexture(tex); }
void CoinCollectible::Update(float dt, Player &player, std::vector<Entity*>&, Level&)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        active = false;
}
void CoinCollectible::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
        DrawTexturePro(tex, {0,0,(float)tex.width,(float)tex.height}, dest, {0,0}, 0, WHITE);
    else DrawRectangleRec(dest, GOLD);
}
Rectangle CoinCollectible::GetBounds() const { return {position.x, position.y, 32, 32}; }

// --- IceBox ---
IceBox::IceBox(Vector2 pos)
{
    position = pos;
    if (FileExists("resources/sprites/Final and IceBox/Ice_box.png"))
    { tex = LoadTexture("resources/sprites/Final and IceBox/Ice_box.png"); texLoaded = true; }
}
void IceBox::Update(float dt, Player &player, std::vector<Entity*>&, Level &level)
{
    // Player walks through: IceBox disappears and tile is cleared
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
    {
        int ts = level.GetTileSize();
        level.SetTileAt((int)(position.x / ts), (int)(position.y / ts), '-');
        active = false;
    }
}
void IceBox::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
        DrawTexturePro(tex, {0,0,(float)tex.width,(float)tex.height}, dest, {0,0}, 0, WHITE);
    else DrawRectangleRec(dest, SKYBLUE);
}
Rectangle IceBox::GetBounds() const { return {position.x, position.y, 32, 32}; }

Entity* CreateEntityFromTile(char tile, Vector2 pos, Level &level, int* starCountPtr)
{
    int tx = (int)(pos.x / level.GetTileSize());
    int ty = (int)(pos.y / level.GetTileSize());
    switch (tile)
    {
    case 'S': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp1.png");
    case '2': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp2.png");
    case 'Y': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp6.png");
    case '4': return new FixedTrap(pos,   "resources/sprites/Traps/Sharp/Sharp4.png");
    case 'G': return new GunTrap(pos, { 1,  0});
    case 'g': return new GunTrap(pos, {-1,  0});
    case 'u': return new GunTrap(pos, { 0, -1});
    case 'd': return new GunTrap(pos, { 0,  1});
    case '6':
    {
        bool horiz = level.GetTileAt(tx-1, ty) == '-' || level.GetTileAt(tx+1, ty) == '-';
        return new Ghost(pos, !horiz);
    }
    case '7':
    {
        bool horiz = level.GetTileAt(tx-1, ty) == '-' || level.GetTileAt(tx+1, ty) == '-';
        return new GhostPlus(pos, !horiz);
    }
    case 's': return new StarCollectible(pos, starCountPtr);
    case 'c': return new CoinCollectible(pos, "resources/sprites/Stars and coins/Coin.png");
    case 'k': return new CoinCollectible(pos, "resources/sprites/Stars and coins/Coin1.png");
    case 'i': return new IceBox(pos);
    default:  return nullptr;
    }
}
