#include "Entities.h"
#include "Player.h"
#include "Level.h"
#include "CoinSystem.h"
#include <cmath>
#include <unordered_map>
#include <string>

static std::unordered_map<std::string, Texture2D> s_texCache;

static Texture2D GetOrLoadTexture(const char* path)
{
    auto it = s_texCache.find(path);
    if (it != s_texCache.end()) return it->second;
    if (!FileExists(path)) return Texture2D{};
    Texture2D t = LoadTexture(path);
    s_texCache[path] = t;
    return t;
}

void ClearEntityTextureCache()
{
    for (auto& p : s_texCache)
        if (p.second.id > 0) UnloadTexture(p.second);
    s_texCache.clear();
}

// --- Bullet ---
Bullet::Bullet(Vector2 pos, Vector2 dir, float speed)
{
    position = pos;
    direction = dir;
    this->speed = speed;
    tex = GetOrLoadTexture("resources/sprites/Traps/Arrow Trap/Arrow.png");
    texLoaded = (tex.id > 0);
}

Bullet::~Bullet() {}

void Bullet::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    position.x += direction.x * speed * dt;
    position.y += direction.y * speed * dt;

    Rectangle worldBounds = level.GetWorldBounds();
    if (position.x < worldBounds.x || position.y < worldBounds.y ||
        position.x > worldBounds.x + worldBounds.width || position.y > worldBounds.y + worldBounds.height)
    {
        active = false; return;
    }

    Rectangle b = GetBounds();
    if (level.IsWall(b.x + b.width * 0.5f, b.y + b.height * 0.5f))
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
    if (direction.x > 0) rotation = 0.0f;
    else if (direction.y > 0) rotation = 90.0f;
    else if (direction.x < 0) rotation = 180.0f;
    else if (direction.y < 0) rotation = 270.0f;

    float w = (texLoaded ? (float)tex.width : 16.0f) * 0.5f;
    float h = (texLoaded ? (float)tex.height : 8.0f) * 0.5f;
    float sw = texLoaded ? (float)tex.width : 16.0f;
    float sh = texLoaded ? (float)tex.height : 8.0f;
    Rectangle dest = { position.x, position.y, w, h };
    if (texLoaded)
        DrawTexturePro(tex, { 0, 0, sw, sh }, dest, { w / 2.0f, h / 2.0f }, rotation, WHITE);
    else DrawRectangleRec({ position.x - w * 0.5f, position.y - h * 0.5f, w, h }, YELLOW);
}

Rectangle Bullet::GetBounds() const
{
    float w  = (texLoaded ? (float)tex.width  : 16.0f) * 0.5f;
    float h  = (texLoaded ? (float)tex.height : 8.0f)  * 0.5f;
    float cx = position.x;
    float cy = position.y;
    bool vertical = fabsf(direction.y) > fabsf(direction.x);
    float bw = vertical ? h : w;
    float bh = vertical ? w : h;
    return { cx - bw * 0.5f, cy - bh * 0.5f, bw, bh };
}

// --- Ghost ---
Ghost::Ghost(Vector2 pos, bool vertical)
{
    position = pos;
    speed = 60.0f;
    if (vertical) dir = { 0,1 }; else dir = { 1,0 };
    tex = GetOrLoadTexture("resources/sprites/Enemy/Monster1.png");
    texLoaded = (tex.id > 0);
    frameIndex = 0;
    animTimer = 0.0f;
}

Ghost::~Ghost() {}

void Ghost::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
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
        hitWall = level.IsWall(newPos.x + 39, newPos.y + 2) || level.IsWall(newPos.x + 39, newPos.y + 37);
    else if (dir.x < 0)
        hitWall = level.IsWall(newPos.x, newPos.y + 2) || level.IsWall(newPos.x, newPos.y + 37);
    else if (dir.y > 0)
        hitWall = level.IsWall(newPos.x + 2, newPos.y + 39) || level.IsWall(newPos.x + 37, newPos.y + 39);
    else if (dir.y < 0)
        hitWall = level.IsWall(newPos.x + 2, newPos.y) || level.IsWall(newPos.x + 37, newPos.y);

    if (hitWall)
    {
        dir.x = -dir.x; dir.y = -dir.y;
    }
    else
        position = newPos;

    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void Ghost::Draw()
{
    Rectangle dest = { position.x, position.y, 40, 40 };
    if (texLoaded)
    {
        Rectangle src = { (float)(frameIndex * 40), 0, 40.0f, (float)tex.height };
        DrawTexturePro(tex, src, dest, Vector2{ 0,0 }, 0, WHITE);
    }
    else DrawRectangleRec(dest, MAGENTA);
}


// --- StarCollectible ---
StarCollectible::StarCollectible(Vector2 pos, int* countPtr)
{
    position = pos;
    this->countPtr = countPtr;
    tex = GetOrLoadTexture("resources/sprites/Stars and coins/Star.png");
    texLoaded = (tex.id > 0);
}

StarCollectible::~StarCollectible() {}

void StarCollectible::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
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
        DrawTexturePro(tex, Rectangle{ 0,0,(float)tex.width,(float)tex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
    else
        DrawRectangleRec(dest, YELLOW);
}

// --- GunTrap ---
GunTrap::GunTrap(Vector2 pos, Vector2 initialDir)
{
    position = pos;
    shootCooldown = 2.0f;
    dir = initialDir;
    tex = GetOrLoadTexture("resources/sprites/Traps/Arrow Trap/Arrow_trap.png");
    texLoaded = (tex.id > 0);
}

GunTrap::~GunTrap() {}

void GunTrap::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    shootCooldown -= dt;
    if (shootCooldown <= 0.0f)
    {
        shootCooldown = 2.0f;
        if (dir.x != 0 || dir.y != 0)
        {
            Vector2 bulletPos = { position.x + 16.0f + dir.x * 16.0f, position.y + 16.0f + dir.y * 16.0f };
            entities.push_back(new Bullet(bulletPos, dir, 200.0f));
        }
    }
}

void GunTrap::Draw()
{
    float rotation = 0.0f;
    if (dir.x < 0) rotation = 0.0f;
    else if (dir.y > 0) rotation = 90.0f;
    else if (dir.x > 0) rotation = 180.0f;
    else if (dir.y < 0) rotation = 270.0f;

    Rectangle dest = { position.x, position.y, 32, 32 };
    Vector2 origin = { 16, 16 };
    Rectangle centeredDest = { position.x + 16, position.y + 16, 32, 32 };
    if (texLoaded) DrawTexturePro(tex, Rectangle{ 0,0,(float)tex.width,(float)tex.height }, centeredDest, origin, rotation, WHITE);
    else DrawRectangleRec(dest, BROWN);
}

// --- TriggerTrap ---
TriggerTrap::TriggerTrap(Vector2 pos, bool horizontal, const char* spritePath, int w, int h)
{
    drawW = w; drawH = h;
    this->horizontal = horizontal;
    position = pos;
    timer = 0.0f;
    triggered = false;
    timerStarted = false;
    tex = GetOrLoadTexture(spritePath);
    texLoaded = (tex.id > 0);
    spikeTex = GetOrLoadTexture("resources/sprites/Traps/Sharp/Spike-trap-spike.png");
    spikeTexLoaded = (spikeTex.id > 0);
    frameIndex = 0;
    animTimer = 0.0f;
    retractTimer = 0.0f;
    retracting = false;
}

TriggerTrap::~TriggerTrap() {}

void TriggerTrap::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    Rectangle triggerZone = horizontal
        ? Rectangle{ position.x,      position.y - 32,   (float)drawW,      (float)drawH + 64 }
        : Rectangle{ position.x - 32, position.y,        (float)drawW + 64, (float)drawH };

    if (!triggered)
    {
        if (!timerStarted && CheckCollisionRecs(player.GetBounds(), triggerZone))
        {
            timerStarted = true;
            if (horizontal)
                spikeRotation = (player.position.y >= position.y) ? 180.0f : 0.0f;
            else
                spikeRotation = (player.position.x >= position.x) ? 90.0f : 270.0f;
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

        Rectangle spikeHit;
        if (GetSpikeHitbox(spikeHit) && CheckCollisionRecs(player.GetBounds(), spikeHit))
            player.Reset();
    }
}

void TriggerTrap::Draw()
{
    float cx = position.x + drawW * 0.5f;
    float cy = position.y + drawH * 0.5f;
    Rectangle dest = { position.x, position.y, (float)drawW, (float)drawH };
    if (texLoaded)
    {
        int totalFrames = tex.width / drawW;
        if (totalFrames < 1) totalFrames = 1;
        int fi = frameIndex < totalFrames ? frameIndex : totalFrames - 1;
        Rectangle src = { (float)(fi * drawW), 0, (float)drawW, (float)tex.height };
        DrawTexturePro(tex, src, dest, Vector2{ 0,0 }, 0, WHITE);
    }

    if (spikeTexLoaded && timerStarted && !triggered)
    {
        const float peekLen = 4.0f;
        float sw      = (float)spikeTex.width;
        float sh      = (float)spikeTex.height;
        float rad     = spikeRotation * DEG2RAD;
        float dirX    = sinf(rad);
        float dirY    = -cosf(rad);
        float edgeX   = cx + dirX * drawW * 0.5f;
        float edgeY   = cy + dirY * drawH * 0.5f;
        float spikeCX = edgeX + dirX * peekLen * 0.5f;
        float spikeCY = edgeY + dirY * peekLen * 0.5f;
        DrawTexturePro(spikeTex, { 0, 0, sw, sh },
                       { spikeCX, spikeCY, sw, peekLen },
                       { sw * 0.5f, peekLen * 0.5f }, spikeRotation, WHITE);
    }

    if (spikeTexLoaded && frameIndex > 0)
    {
        float progress  = frameIndex / 4.0f;
        float extendLen = progress * 32.0f;
        float sw        = (float)spikeTex.width;
        float sh        = (float)spikeTex.height;
        float rad       = spikeRotation * DEG2RAD;
        float dirX      = sinf(rad);
        float dirY      = -cosf(rad);
        float edgeX     = cx + dirX * drawW * 0.5f;
        float edgeY     = cy + dirY * drawH * 0.5f;
        float spikeCX   = edgeX + dirX * extendLen * 0.5f;
        float spikeCY   = edgeY + dirY * extendLen * 0.5f;
        DrawTexturePro(spikeTex, { 0, 0, sw, sh },
                       { spikeCX, spikeCY, sw, extendLen },
                       { sw * 0.5f, extendLen * 0.5f }, spikeRotation, WHITE);
    }
}

bool TriggerTrap::GetSpikeHitbox(Rectangle& out) const
{
    if (frameIndex <= 0 || retracting) return false;
    float progress  = frameIndex / 4.0f;
    float extendLen = progress * 32.0f;
    float cx    = position.x + drawW * 0.5f;
    float cy    = position.y + drawH * 0.5f;
    float rad   = spikeRotation * DEG2RAD;
    float dirX  = sinf(rad);
    float dirY  = -cosf(rad);
    float edgeX = cx + dirX * drawW * 0.5f;
    float edgeY = cy + dirY * drawH * 0.5f;
    if (fabsf(dirX) > fabsf(dirY))
    {
        float x0 = dirX > 0 ? edgeX : edgeX - extendLen;
        out = { x0, edgeY - 6.0f, extendLen, 12.0f };
    }
    else
    {
        float y0 = dirY > 0 ? edgeY : edgeY - extendLen;
        out = { edgeX - 6.0f, y0, 12.0f, extendLen };
    }
    return true;
}

// --- Decoration ---
Decoration::Decoration(Vector2 pos, const char* spritePath, int fw, float ds)
{
    position = pos;
    frameWidth = fw;
    drawSize = ds;
    tex = GetOrLoadTexture(spritePath);
    texLoaded = (tex.id > 0);
}
Decoration::~Decoration() {}
void Decoration::Update(float dt, Player& player, std::vector<Entity*>&, Level&)
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
    float offset = (16.0f - drawSize) / 2.0f;
    Rectangle dest = { position.x + offset, position.y + offset, drawSize, drawSize };
    if (!texLoaded) return;
    if (frameWidth > 0)
    {
        Rectangle src = { (float)(frameIndex * frameWidth), 0, (float)frameWidth, (float)tex.height };
        DrawTexturePro(tex, src, dest, { 0,0 }, 0, WHITE);
    }
    else
        DrawTexturePro(tex, { 0,0,(float)tex.width,(float)tex.height }, dest, { 0,0 }, 0, WHITE);
}

// --- FixedTrap ---
FixedTrap::FixedTrap(Vector2 pos, const char* spritePath, int w, int h)
{
    drawW = w; drawH = h;
    position = pos;
    tex = GetOrLoadTexture(spritePath);
    texLoaded = (tex.id > 0);
}

FixedTrap::~FixedTrap() {}

void FixedTrap::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void FixedTrap::Draw()
{
    Rectangle dest = { position.x, position.y, (float)drawW, (float)drawH };
    if (texLoaded) DrawTexturePro(tex, Rectangle{ 0,0,(float)tex.width,(float)tex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
    else DrawRectangleRec(dest, ORANGE);
}

// --- CoinCollectible ---
CoinCollectible::CoinCollectible(Vector2 pos, const char* spritePath, CoinSystem* coinSys)
    : position(pos), coinSystem(coinSys)
{
    tex = GetOrLoadTexture(spritePath);
    texLoaded = (tex.id > 0);
}

CoinCollectible::~CoinCollectible() {}

void CoinCollectible::Update(float dt, Player& player, std::vector<Entity*>&, Level&)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
    {
        if (coinSystem != nullptr) {
            coinSystem->addCoins(1);
        }
        active = false;
    }
}

void CoinCollectible::Draw()
{
    Rectangle dest = { position.x, position.y, 32, 32 };
    if (texLoaded)
        DrawTexturePro(tex, { 0,0,(float)tex.width,(float)tex.height }, dest, { 0,0 }, 0, WHITE);
    else DrawRectangleRec(dest, GOLD);
}

Rectangle CoinCollectible::GetBounds() const { return { position.x, position.y, 32, 32 }; }

// --- IceBox ---
IceBox::IceBox(Vector2 pos)
{
    position = pos;
    tex = GetOrLoadTexture("resources/sprites/Exit/Ice_box.png");
    texLoaded = (tex.id > 0);
}

void IceBox::Update(float dt, Player& player, std::vector<Entity*>&, Level& level)
{
    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
    {
        int ts = level.GetTileSize();
        level.SetTileAt((int)(position.x / ts), (int)(position.y / ts), '-');
        active = false;
    }
}

void IceBox::Draw()
{
    Rectangle dest = { position.x, position.y, 40, 40 };
    if (texLoaded)
        DrawTexturePro(tex, { 0,0,(float)tex.width,(float)tex.height }, dest, { 0,0 }, 0, WHITE);
    else DrawRectangleRec(dest, SKYBLUE);
}

Rectangle IceBox::GetBounds() const { return { position.x, position.y, 40, 40 }; }

// --- Monster2Ball ---
Monster2Ball::Monster2Ball(Vector2 pos)
{
    position = pos;
    tex = GetOrLoadTexture("resources/sprites/Enemy/Monster2_Ball.png");
    texLoaded = (tex.id > 0);
}

void Monster2Ball::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    const float FALL_SPEED = 360.0f;
    position.y += FALL_SPEED * dt;

    Vector2 pc  = player.GetCenter();
    float camBottom = pc.y - GetScreenHeight() / 3.0f + GetScreenHeight();
    if (position.y > camBottom + 300.0f) { active = false; return; }

    if (CheckCollisionRecs(player.GetBounds(), GetBounds()))
        player.Reset();
}

void Monster2Ball::Draw()
{
    float w = texLoaded ? (float)tex.width  : 32.0f;
    float h = texLoaded ? (float)tex.height : 32.0f;
    Rectangle dest = { position.x, position.y, w, h };
    if (texLoaded)
        DrawTexturePro(tex, { 0, 0, w, h }, dest, { 0, 0 }, 0, WHITE);
    else
        DrawRectangleRec(dest, RED);
}

Rectangle Monster2Ball::GetBounds() const
{
    float w = texLoaded ? (float)tex.width  : 32.0f;
    float h = texLoaded ? (float)tex.height : 32.0f;
    return { position.x, position.y, w, h };
}

// --- Monster2 ---
Monster2::Monster2(Vector2 pos)
    : position(pos), tex{}, texLoaded(false), assetsLoaded(false),
      state(State::Waiting), stateTimer(0.0f), frameIndex(0), animTimer(0.0f), ballDropped(false)
{
}

void Monster2::Update(float dt, Player& player, std::vector<Entity*>& entities, Level& level)
{
    Vector2 pc   = player.GetCenter();
    float camX   = pc.x - GetScreenWidth()  / 3.0f;
    float camY   = pc.y - GetScreenHeight() / 3.0f;
    Rectangle camView = { camX, camY, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Rectangle myBounds = GetBounds();

    if (state == State::Waiting)
    {
        if (!CheckCollisionRecs(myBounds, camView)) return;
        if (!assetsLoaded)
        {
            tex = GetOrLoadTexture("resources/sprites/Enemy/Monster2.png");
            texLoaded   = (tex.id > 0);
            assetsLoaded = true;
        }
        state      = State::Floating;
        stateTimer = 0.0f;
        frameIndex = 0;
        animTimer  = 0.0f;
    }

    if (state == State::Floating)
    {
        stateTimer += dt;
        animTimer  += dt;
        if (animTimer >= 0.4f) { animTimer = 0.0f; frameIndex = (frameIndex + 1) % 2; }

        if (stateTimer >= 3.0f)
        {
            state      = State::Launching;
            stateTimer = 0.0f;
            frameIndex = 2;
            animTimer  = 0.0f;
        }
    }
    else if (state == State::Launching)
    {
        if (!ballDropped)
        {
            ballDropped = true;
            entities.push_back(new Monster2Ball({ position.x, position.y + 96.0f }));
        }

        if (frameIndex < 4)
        {
            animTimer += dt;
            if (animTimer >= 0.1f) { animTimer = 0.0f; frameIndex++; }
        }

        const float RISE_SPEED = 60.0f;
        position.y -= RISE_SPEED * dt;
        if (position.y + 96.0f < camView.y) { active = false; return; }
    }

    if (CheckCollisionRecs(player.GetBounds(), myBounds))
        player.Reset();
}

void Monster2::Draw()
{
    if (state == State::Waiting || !texLoaded) return;

    float bobY = (state == State::Floating) ? sinf(stateTimer * 2.0f) * 10.0f : 0.0f;
    float fw   = (float)tex.width / 5.0f;
    float fh   = (float)tex.height;
    Rectangle src  = { (float)frameIndex * fw, 0, fw, fh };
    Rectangle dest = { position.x, position.y + bobY, 96.0f, 96.0f };
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0, WHITE);
}

// --- Entity factory ---
Entity* CreateEntityFromTile(char tile, Vector2 pos, Level& level, int* starCountPtr, CoinSystem* coinSys)
{
    int tx = (int)(pos.x / level.GetTileSize());
    int ty = (int)(pos.y / level.GetTileSize());
    switch (tile)
    {
    case 'P': return new Decoration(pos, "resources/sprites/Stars and coins/Step.png");
    case '2': return new TriggerTrap(pos, true, "resources/sprites/Traps/Sharp/Sharp2.png", 24, 15);
    case '3': return new TriggerTrap(pos, false, "resources/sprites/Traps/Sharp/Sharp3.png", 16, 25);
    case '4': return new FixedTrap(pos, "resources/sprites/Traps/Sharp/Sharp4.png", 24, 38);
    case 'G': return new GunTrap(pos, { -1,  0 });
    case 'g': return new GunTrap(pos, { 0,  1 });
    case 'u': return new GunTrap(pos, { 1,  0 });
    case 'd': return new GunTrap(pos, { 0, -1 });
    case '6':
    {
        bool horiz = level.GetTileAt(tx - 1, ty) == '-' || level.GetTileAt(tx + 1, ty) == '-';
        return new Ghost(pos, !horiz);
    }
    case '7': return new Monster2(pos);
    case 's': return new StarCollectible(pos, starCountPtr);
    case 'c': return new CoinCollectible(pos, "resources/sprites/Stars and coins/Coin.png", coinSys);
    case 'k': return new CoinCollectible(pos, "resources/sprites/Stars and coins/Coin1.png", coinSys);
    case 'i': return new IceBox(pos);
    case 'f': return new Decoration(pos, "resources/sprites/Exit/Final.png", 40, 46.0f);
    default:  return nullptr;
    }
}