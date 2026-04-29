#include "Player.h"

static const int framePositions[] = {
    PLAYER_IDLE,
    PLAYER_START_MOVE,
    PLAYER_MOVE_1,
    PLAYER_MOVE_2,
    PLAYER_MOVE_3,
    PLAYER_MOVE_4
};
static const int FRAME_COUNT_DEFINED = 6;

void Player::Init(Vector2 startPos)
{
    startPosition = startPos;
    position = startPos;
    speed = 700.0f;
    currentDir = { 0, 0 };
    sliding = false;

    texture = LoadTexture("resources/sprites/Player.png");
    if (texture.width == 0 || texture.height == 0) //Fallback to white square if texture missing
    {
        Image img = GenImageColor(32, 32, WHITE);
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    frameIndex = 0;
    animTimer = 0.0f;
    animSpeed = 0.12f;
    drawScale = 1.0f;
    drawRotation = 0.0f;
    flipX = false;
    diedThisFrame = false;
}

bool Player::IsWallAhead(Vector2 dir, Level& level) const
{
    int tileSize = level.GetTileSize();
    float drawWidth  = 32.0f * drawScale;
    float drawHeight = 32.0f * drawScale;

    float nextX = position.x + dir.x;
    float nextY = position.y + dir.y;

    auto isWall = [&](float px, float py) -> bool {
        int tileX = (int)(px / tileSize);
        int tileY = (int)(py / tileSize);
        char tile = level.GetTileAt(tileX, tileY);
        return tile == (char)Wall1 || tile == (char)Wall2 || tile == (char)Empty;
        };

    if (dir.x > 0)
        return isWall(nextX + drawWidth - 1, nextY) || isWall(nextX + drawWidth - 1, nextY + drawHeight - 1);
    if (dir.x < 0)
        return isWall(nextX, nextY) || isWall(nextX, nextY + drawHeight - 1);
    if (dir.y > 0)
        return isWall(nextX, nextY + drawHeight - 1) || isWall(nextX + drawWidth - 1, nextY + drawHeight - 1);
    if (dir.y < 0)
        return isWall(nextX, nextY) || isWall(nextX + drawWidth - 1, nextY);

    return false;
}

void Player::Update(float dt, Rectangle worldBounds, Level& level)
{
    diedThisFrame = false;
    if (!sliding)
    {
        if (IsKeyDown(KEY_RIGHT)) { currentDir = { 1,  0 }; sliding = true; }
        else if (IsKeyDown(KEY_LEFT)) { currentDir = { -1,  0 }; sliding = true; }
        else if (IsKeyDown(KEY_DOWN)) { currentDir = { 0,  1 }; sliding = true; }
        else if (IsKeyDown(KEY_UP)) { currentDir = { 0, -1 }; sliding = true; }
    }

    if (sliding)
    {
        if (IsWallAhead(currentDir, level))
        {
            // Snap to nearest tile boundary so player doesn't overlap the wall
            int tileSize = level.GetTileSize();
            if (currentDir.x > 0)
                position.x = (float)((int)(position.x / tileSize) * tileSize);
            else if (currentDir.x < 0)
                position.x = (float)(((int)((position.x - 1) / tileSize) + 1) * tileSize);
            if (currentDir.y > 0)
                position.y = (float)((int)(position.y / tileSize) * tileSize);
            else if (currentDir.y < 0)
                position.y = (float)(((int)((position.y - 1) / tileSize) + 1) * tileSize);

            sliding = false;
            currentDir = { 0, 0 };
        }
        else
        {
            position.x += currentDir.x * speed * dt;
            position.y += currentDir.y * speed * dt;

            float drawWidth  = 32.0f * drawScale;
            float drawHeight = 32.0f * drawScale;
            if (position.x < worldBounds.x) { position.x = worldBounds.x; sliding = false; currentDir = { 0,0 }; }
            if (position.x > worldBounds.x + worldBounds.width - drawWidth) { position.x = worldBounds.x + worldBounds.width - drawWidth; sliding = false; currentDir = { 0,0 }; }
            if (position.y < worldBounds.y) { position.y = worldBounds.y; sliding = false; currentDir = { 0,0 }; }
            if (position.y > worldBounds.y + worldBounds.height - drawHeight) { position.y = worldBounds.y + worldBounds.height - drawHeight; sliding = false; currentDir = { 0,0 }; }
        }
    }

    if (!sliding)
    {
        frameIndex = 0;
        animTimer = 0.0f;
    }
    else
    {
        // Set rotation and flip based on direction
        if (currentDir.x > 0) { drawRotation = 0.0f;   flipX = false; }
        else if (currentDir.x < 0) { drawRotation = 0.0f;   flipX = true; }
        else if (currentDir.y < 0) { drawRotation = -90.0f; flipX = false; }
        else if (currentDir.y > 0) { drawRotation = 90.0f;  flipX = false; }

        animTimer += dt;
        if (animTimer >= animSpeed)
        {
            animTimer = 0.0f;
            frameIndex++;
            if (frameIndex >= FRAME_COUNT_DEFINED) frameIndex = 1;
        }
    }
}

void Player::Draw()
{
    int srcFrameX = framePositions[frameIndex];
    float drawWidth  = 32.0f * drawScale;
    float drawHeight = 32.0f * drawScale;
    Rectangle src  = { (float)srcFrameX, 0, flipX ? -32.0f : 32.0f, 32.0f };
    Rectangle dest = { position.x + drawWidth / 2.0f, position.y + drawHeight / 2.0f, drawWidth, drawHeight };
    DrawTexturePro(texture, src, dest, { drawWidth / 2.0f, drawHeight / 2.0f }, drawRotation, WHITE);
}

void Player::DeInit()
{
    UnloadTexture(texture);
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
