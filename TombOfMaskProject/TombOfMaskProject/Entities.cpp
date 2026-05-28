#include "Entities.h"
#include "Player.h"
#include "Level.h"
#include <cmath>

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

    if (level.IsWall(position.x + 4, position.y + 4))
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
    float rotation = 0.0f;
    if      (direction.x > 0) rotation =   0.0f;
    else if (direction.y > 0) rotation =  90.0f;
    else if (direction.x < 0) rotation = 180.0f;
    else if (direction.y < 0) rotation = 270.0f;

    float w = (texLoaded ? (float)tex.width  : 16.0f) * 0.5f;
    float h = (texLoaded ? (float)tex.height : 8.0f)  * 0.5f;
    float sw = texLoaded ? (float)tex.width  : 16.0f;
    float sh = texLoaded ? (float)tex.height : 8.0f;
    Rectangle dest = { position.x + w / 2.0f, position.y + h / 2.0f, w, h };
    if (texLoaded)
        DrawTexturePro(tex, {0, 0, sw, sh}, dest, {w / 2.0f, h / 2.0f}, rotation, WHITE);
    else DrawRectangleRec({position.x, position.y, w, h}, YELLOW);
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
    if (animTimer >= 0.75f)
    {
        animTimer = 0.0f;
        int totalFrames = texLoaded ? (tex.width / 40) : 1;
        frameIndex = (frameIndex + 1) % totalFrames;
    }

    Vector2 newPos = { position.x + dir.x * speed * dt, position.y + dir.y * speed * dt };

    bool hitWall = false;
    if (dir.x > 0)
        hitWall = level.IsWall(newPos.x + 31, newPos.y + 2) || level.IsWall(newPos.x + 31, newPos.y + 29);
    else if (dir.x < 0)
        hitWall = level.IsWall(newPos.x,      newPos.y + 2) || level.IsWall(newPos.x,      newPos.y + 29);
    else if (dir.y > 0)
        hitWall = level.IsWall(newPos.x + 2,  newPos.y + 31) || level.IsWall(newPos.x + 29, newPos.y + 31);
    else if (dir.y < 0)
        hitWall = level.IsWall(newPos.x + 2,  newPos.y)      || level.IsWall(newPos.x + 29, newPos.y);

    if (hitWall)
        { dir.x = -dir.x; dir.y = -dir.y; }
    else
        position = newPos;

    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void Ghost::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
    {
        Rectangle src = { (float)(frameIndex * 40), 0, 40.0f, (float)tex.height };
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
    if (animTimer >= 0.5f)
    {
        animTimer = 0.0f;
        int totalFrames = plusTexLoaded ? (plusTex.width / 96) : 1;
        frameIndex = (frameIndex + 1) % totalFrames;
    }

    Ghost::Update(dt, player, entities, level);
}

void GhostPlus::Draw()
{
    if (plusTexLoaded)
    {
        Rectangle src  = { (float)(frameIndex * 96), 0, 96.0f, (float)plusTex.height };
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
    float rotation = 0.0f;
    if      (dir.x < 0) rotation =   0.0f;
    else if (dir.y > 0) rotation =  90.0f;
    else if (dir.x > 0) rotation = 180.0f;
    else if (dir.y < 0) rotation = 270.0f;

    Rectangle dest = { position.x, position.y, 32, 32 };
    Vector2 origin = { 16, 16 };
    Rectangle centeredDest = { position.x + 16, position.y + 16, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{0,0,32.0f,32.0f}, centeredDest, origin, rotation, WHITE);
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
    const char* spikePath = "resources/sprites/Traps/Sharp/Spike-trap-spike.png";
    if (FileExists(spikePath)) { spikeTex = LoadTexture(spikePath); spikeTexLoaded = true; }
    frameIndex = 0;
    animTimer = 0.0f;
    retractTimer = 0.0f;
    retracting = false;
}

TriggerTrap::~TriggerTrap() { if (texLoaded) UnloadTexture(tex); if (spikeTexLoaded) UnloadTexture(spikeTex); }

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
        {
            timerStarted = true;
            // 锁定朝向玩家的方向（四方向），触发后不再改变
            float dx = player.position.x - position.x;
            float dy = player.position.y - position.y;
            if ((dx < 0 ? -dx : dx) >= (dy < 0 ? -dy : dy))
                spikeRotation = (dx >= 0) ? 90.0f : 270.0f;
            else
                spikeRotation = (dy >= 0) ? 180.0f : 0.0f;
        }

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
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
    {
        int totalFrames = tex.width / 32;
        if (totalFrames < 1) totalFrames = 1;
        int fi = frameIndex < totalFrames ? frameIndex : totalFrames - 1;
        Rectangle src = { (float)(fi * 32), 0, 32.0f, (float)tex.height };
        DrawTexturePro(tex, src, dest, Vector2{0,0}, 0, WHITE);
    }

    if (spikeTexLoaded && (timerStarted || triggered))
    {
        float sh = (float)spikeTex.height;
        float sw = (float)spikeTex.width;
        Vector2 center = { position.x + 16, position.y + 16 };
        Vector2 origin = { 16, 16 };
        if (timerStarted && !triggered)
        {
            // 半截突刺：裁取图片上半（刺尖），缩放到半格高显示
            Rectangle src   = { 0, 0, sw, sh / 2.0f };
            Rectangle sdest = { center.x, center.y, 32, 16 };
            DrawTexturePro(spikeTex, src, sdest, origin, spikeRotation, WHITE);
        }
        else if (triggered)
        {
            Rectangle src = { 0, 0, sw, sh };
            DrawTexturePro(spikeTex, src, { center.x, center.y, 32, 32 }, origin, spikeRotation, WHITE);
        }
    }
}

Rectangle TriggerTrap::GetBounds() const { return Rectangle{ position.x, position.y, 32, 32 }; }

// --- Decoration ---
Decoration::Decoration(Vector2 pos, const char* spritePath, int fw, float ds)
{
    position = pos;
    frameWidth = fw;
    drawSize = ds;
    texLoaded = false;
    if (FileExists(spritePath)) { tex = LoadTexture(spritePath); texLoaded = true; }
}
Decoration::~Decoration() { if (texLoaded) UnloadTexture(tex); }
void Decoration::Update(float dt, Player &player, std::vector<Entity*>&, Level&)
{
    if (frameWidth > 0 && texLoaded)
    {
        animTimer += dt;
        if (animTimer >= 0.1f)
        {
            animTimer = 0.0f;
            int fc = tex.width / frameWidth;
            frameIndex = (frameIndex + 1) % fc;
        }
    }
    if (CheckCollisionRecs(player.GetBounds(), GetBounds())) active = false;
}
void Decoration::Draw()
{
    float offset = (32.0f - drawSize) / 2.0f;
    Rectangle dest = { position.x + offset, position.y + offset, drawSize, drawSize };
    if (!texLoaded) return;
    if (frameWidth > 0)
    {
        Rectangle src = { (float)(frameIndex * frameWidth), 0, (float)frameWidth, (float)tex.height };
        DrawTexturePro(tex, src, dest, {0,0}, 0, WHITE);
    }
    else
        DrawTexturePro(tex, {0,0,(float)tex.width,(float)tex.height}, dest, {0,0}, 0, WHITE);
}

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
    if (FileExists("resources/sprites/Exit/Ice_box.png"))
    { tex = LoadTexture("resources/sprites/Exit/Ice_box.png"); texLoaded = true; }
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
    case 'P': return new Decoration(pos,  "resources/sprites/Stars and coins/Step.png");
    case 'S': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp1.png");
    case '2': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp2.png");
    case '3': return new TriggerTrap(pos, "resources/sprites/Traps/Sharp/Sharp3.png");
    case '4': return new FixedTrap(pos,   "resources/sprites/Traps/Sharp/Sharp4.png");
    case 'G': return new GunTrap(pos, {-1,  0});  // Arrow_trap  → left
    case 'g': return new GunTrap(pos, { 0,  1});  // Arrow_trap1 → down
    case 'u': return new GunTrap(pos, { 1,  0});  // Arrow_trap2 → right
    case 'd': return new GunTrap(pos, { 0, -1});  // Arrow_trap3 → up
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
    case 'f': return new Decoration(pos, "resources/sprites/Exit/Final.png", 40, 48.0f);
    default:  return nullptr;
    }
}
