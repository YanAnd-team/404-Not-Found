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
}

void Player::Update(float dt, Rectangle worldBounds)
{
    if (IsKeyDown(KEY_RIGHT)) position.x += speed * dt;
    if (IsKeyDown(KEY_LEFT))  position.x -= speed * dt;
    if (IsKeyDown(KEY_DOWN))  position.y += speed * dt;
    if (IsKeyDown(KEY_UP))    position.y -= speed * dt;

    // Clamp to provided world bounds. Draw scale is 2, so account for texture size * 2
    float drawW = texture.width * 2.0f;
    float drawH = texture.height * 2.0f;

    if (position.x < worldBounds.x) position.x = worldBounds.x;
    if (position.x > worldBounds.x + worldBounds.width - drawW) position.x = worldBounds.x + worldBounds.width - drawW;
    if (position.y < worldBounds.y) position.y = worldBounds.y;
    if (position.y > worldBounds.y + worldBounds.height - drawH) position.y = worldBounds.y + worldBounds.height - drawH;
}

void Player::Draw()
{
    DrawTextureEx(texture, position, 0, 2, WHITE);
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
    // Player drawn with scale 2, so center should account for that
    return { position.x + texture.width, position.y + texture.height };
}
