#include "Player.h"

void Player::Init(Vector2 startPos)
{
    startPosition = startPos;
    position = startPos;
    // speed is in pixels per second; multiplied by dt in Update
    speed = 200.0f;
    texture = LoadTexture("resources/sprites/Player.png");
    // If texture failed to load, create a simple placeholder (1x1 white texture)
    if (texture.width == 0 || texture.height == 0)
    {
        Image img = GenImageColor(1, 1, WHITE);
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    // animation defaults
    frameIndex = 0;
    frameCount = texture.width / 32; // assume horizontal spritesheet
    animTimer = 0.0f;
    animSpeed = 0.12f; // seconds per frame
    drawScale = 2.0f;
    drawRotation = 0.0f;
    flipX = false;
}

void Player::Update(float dt, Rectangle worldBounds)
{
    bool moved = false;
    Vector2 moveDir = {0,0};
    if (IsKeyDown(KEY_RIGHT)) { position.x += speed * dt; moved = true; moveDir = {1,0}; }
    if (IsKeyDown(KEY_LEFT))  { position.x -= speed * dt; moved = true; moveDir = {-1,0}; }
    if (IsKeyDown(KEY_DOWN))  { position.y += speed * dt; moved = true; moveDir = {0,1}; }
    if (IsKeyDown(KEY_UP))    { position.y -= speed * dt; moved = true; moveDir = {0,-1}; }

    // Clamp to provided world bounds. Draw scale is 2, so account for texture size * 2
    float drawW = 32.0f * drawScale; // use tile size
    float drawH = 32.0f * drawScale;

    if (position.x < worldBounds.x) position.x = worldBounds.x;
    if (position.x > worldBounds.x + worldBounds.width - drawW) position.x = worldBounds.x + worldBounds.width - drawW;
    if (position.y < worldBounds.y) position.y = worldBounds.y;
    if (position.y > worldBounds.y + worldBounds.height - drawH) position.y = worldBounds.y + worldBounds.height - drawH;

    // animation update
    if (!moved)
    {
        // idle
        frameIndex = 0;
        animTimer = 0.0f;
    }
    else
    {
        // decide rotation/flip based on moveDir
        if (moveDir.x > 0) { drawRotation = 0.0f; flipX = false; }
        else if (moveDir.x < 0) { drawRotation = 0.0f; flipX = true; }
        else if (moveDir.y < 0) { drawRotation = -90.0f; flipX = false; }
        else if (moveDir.y > 0) { drawRotation = 90.0f; flipX = false; }

        animTimer += dt;
        if (animTimer >= animSpeed)
        {
            animTimer = 0.0f;
            frameIndex++;
            if (frameIndex >= frameCount) frameIndex = 1; // keep frame 0 as idle
        }
    }
}

void Player::Draw()
{
    // source rect for current frame
    Rectangle src = { frameIndex * 32.0f, 0, 32.0f, 32.0f };
    Rectangle dest = { position.x, position.y, 32.0f * drawScale, 32.0f * drawScale };
    Vector2 origin = { 0,0 };
    DrawTexturePro(texture, src, dest, origin, drawRotation, WHITE);
}

void Player::DeInit()
{
    UnloadTexture(texture);
}

void Player::Reset()
{
    position = startPosition;
}

Vector2 Player::GetCenter() const
{
    // center based on drawn frame (32x32) and drawScale
    float w = 32.0f * drawScale;
    float h = 32.0f * drawScale;
    return { position.x + w * 0.5f, position.y + h * 0.5f };
}

Rectangle Player::GetBounds() const
{
    return Rectangle{ position.x, position.y, (float)texture.width * 2.0f, (float)texture.height * 2.0f };
}
