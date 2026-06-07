#include "Player.h"
#include "Level.h"

static const int moveFrameX[]     = {   0,  32, 128, 288, 480 };
static const int moveFrameWidth[] = {  32,  96, 160, 192, 192 };
static const int moveHeadW[]      = {  32,  32,  36,  36,  36 };
static const int MOVE_FRAME_COUNT = 5;

void Player::Init(Vector2 startPos)
{
    startPosition = startPos;
    position = startPos;
    speed = 1400.0f;
    currentDir   = { 0, 0 };
    lastSlideDir = { 0, 0 };
    sliding = false;

    if (FileExists("resources/sprites/Player/Player Idle.png"))
        idleTex = LoadTexture("resources/sprites/Player/Player Idle.png");
    else { Image img = GenImageColor(32, 32, WHITE);  idleTex = LoadTextureFromImage(img); UnloadImage(img); }

    if (FileExists("resources/sprites/Player/Player Move.png"))
        moveTex = LoadTexture("resources/sprites/Player/Player Move.png");
    else { Image img = GenImageColor(32, 32, YELLOW); moveTex = LoadTextureFromImage(img); UnloadImage(img); }

    frameIndex    = 0;
    idleFrameIdx  = 0;
    animTimer     = 0.0f;
    idleAnimTimer = 0.0f;
    animSpeed     = 0.02667f;
    drawScale = 1.0f;
    drawRotation = 0.0f;
    flipX = false;
    diedThisFrame = false;
}


void Player::Update(float dt, Rectangle worldBounds, Level& level)
{
    diedThisFrame = false;
    if (!sliding)
    {
        if      (IsKeyDown(KEY_RIGHT)) { currentDir = { 1,  0 }; lastSlideDir = currentDir; sliding = true; }
        else if (IsKeyDown(KEY_LEFT))  { currentDir = {-1,  0 }; lastSlideDir = currentDir; sliding = true; }
        else if (IsKeyDown(KEY_DOWN))  { currentDir = { 0,  1 }; lastSlideDir = currentDir; sliding = true; }
        else if (IsKeyDown(KEY_UP))    { currentDir = { 0, -1 }; lastSlideDir = currentDir; sliding = true; }
    }

    if (sliding)
    {
        const float dw   = 32.0f * drawScale;
        const float dh   = 32.0f * drawScale;
        const float move = speed * dt;
        bool hitWall = false;

        // Scan from current leading edge to future leading edge (1px steps).
        // When a wall cell is found, snap the player flush against it and stop.
        // This catches walls thinner than one frame of movement (e.g., 4px walls).
        if (currentDir.x > 0)
        {
            float s = position.x + dw;
            for (float x = s; x <= s + move + 1.0f; x += 1.0f)
            {
                if (level.IsWall(x, position.y) || level.IsWall(x, position.y + dh * 0.5f) || level.IsWall(x, position.y + dh - 1))
                { position.x = floorf(x / 4.0f) * 4.0f - dw; hitWall = true; break; }
            }
            if (!hitWall) position.x += move;
        }
        else if (currentDir.x < 0)
        {
            float s = position.x;
            for (float x = s; x >= s - move - 1.0f; x -= 1.0f)
            {
                if (level.IsWall(x, position.y) || level.IsWall(x, position.y + dh * 0.5f) || level.IsWall(x, position.y + dh - 1))
                { position.x = (floorf(x / 4.0f) + 1.0f) * 4.0f; hitWall = true; break; }
            }
            if (!hitWall) position.x -= move;
        }
        else if (currentDir.y > 0)
        {
            float s = position.y + dh;
            for (float y = s; y <= s + move + 1.0f; y += 1.0f)
            {
                if (level.IsWall(position.x, y) || level.IsWall(position.x + dw * 0.5f, y) || level.IsWall(position.x + dw - 1, y))
                { position.y = floorf(y / 4.0f) * 4.0f - dh; hitWall = true; break; }
            }
            if (!hitWall) position.y += move;
        }
        else if (currentDir.y < 0)
        {
            float s = position.y;
            for (float y = s; y >= s - move - 1.0f; y -= 1.0f)
            {
                if (level.IsWall(position.x, y) || level.IsWall(position.x + dw * 0.5f, y) || level.IsWall(position.x + dw - 1, y))
                { position.y = (floorf(y / 4.0f) + 1.0f) * 4.0f; hitWall = true; break; }
            }
            if (!hitWall) position.y -= move;
        }

        if (hitWall)
        {
            sliding = false;
            currentDir = {0, 0};
        }
        else
        {
            if (position.x < worldBounds.x)                               { position.x = worldBounds.x;                               sliding = false; currentDir = {0,0}; }
            if (position.x > worldBounds.x + worldBounds.width  - dw)    { position.x = worldBounds.x + worldBounds.width  - dw;     sliding = false; currentDir = {0,0}; }
            if (position.y < worldBounds.y)                               { position.y = worldBounds.y;                               sliding = false; currentDir = {0,0}; }
            if (position.y > worldBounds.y + worldBounds.height - dh)    { position.y = worldBounds.y + worldBounds.height - dh;     sliding = false; currentDir = {0,0}; }
        }
    }

    if (!sliding)
    {
        idleAnimTimer += dt;
        if (idleAnimTimer >= 0.15f)
        {
            idleAnimTimer = 0.0f;
            int fc = idleTex.width > 0 ? (idleTex.width / 32) : 1;
            idleFrameIdx = (idleFrameIdx + 1) % fc;
        }
        frameIndex = 0;
        animTimer = 0.0f;
    }
    else
    {
        idleFrameIdx = 0;

        if (currentDir.x > 0) { drawRotation = 0.0f;   flipX = true; }
        else if (currentDir.x < 0) { drawRotation = 0.0f;   flipX = false; }
        else if (currentDir.y < 0) { drawRotation = 90.0f;  flipX = false; }
        else if (currentDir.y > 0) { drawRotation = -90.0f; flipX = false; }

        animTimer += dt;
        if (animTimer >= animSpeed)
        {
            animTimer = 0.0f;
            frameIndex++;
            if (frameIndex > MOVE_FRAME_COUNT) frameIndex = 2;
        }
    }
}

void Player::Draw()
{
    float drawWidth  = 32.0f * drawScale;
    float drawHeight = 32.0f * drawScale;
    float cx = position.x + drawWidth * 0.5f;
    float cy = position.y + drawHeight * 0.5f;

    if (frameIndex == 0)
    {
        int fc = idleTex.width > 0 ? (idleTex.width / 32) : 1;
        int fi = idleFrameIdx % fc;
        float h = idleTex.height > 0 ? (float)idleTex.height : 32.0f;
        Rectangle src  = { (float)(fi * 32), 0, flipX ? -32.0f : 32.0f, h };
        Rectangle dest = { cx, cy, drawWidth, drawHeight };
        DrawTexturePro(idleTex, src, dest, { drawWidth * 0.5f, drawHeight * 0.5f }, drawRotation, WHITE);
    }
    else
    {
        int fi   = (frameIndex - 1) % MOVE_FRAME_COUNT;
        float fw = (float)moveFrameWidth[fi] * drawScale;
        float h  = moveTex.height > 0 ? (float)moveTex.height : 32.0f;
        Rectangle src  = { (float)moveFrameX[fi], 0, flipX ? -(float)moveFrameWidth[fi] : (float)moveFrameWidth[fi], h };
        Rectangle dest = { cx, cy, fw, drawHeight };
        float hw = (float)moveHeadW[fi] * 0.5f * drawScale;
        float ow = flipX ? (fw - hw) : hw;
        DrawTexturePro(moveTex, src, dest, { ow, drawHeight * 0.5f }, drawRotation, WHITE);
    }
}

void Player::DeInit()
{
    if (idleTex.id) UnloadTexture(idleTex);
    if (moveTex.id) UnloadTexture(moveTex);
}

void Player::Reset()
{
    position = startPosition;
    currentDir = { 0, 0 };
    sliding = false;
    diedThisFrame = true;
}

Vector2 Player::GetCenter() const
{
    float drawWidth  = 32.0f * drawScale;
    float drawHeight = 32.0f * drawScale;
    return { position.x + drawWidth * 0.5f, position.y + drawHeight * 0.5f };
}

Rectangle Player::GetBounds() const
{
    return Rectangle{ position.x, position.y, 32.0f * drawScale, 32.0f * drawScale };
}
