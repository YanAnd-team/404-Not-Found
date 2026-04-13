#include "Player.h"

void Player::Init(Vector2 startPos)
{
    startPosition = startPos;
    position = startPos;
    // speed is in pixels per second; multiplied by dt in Update
    speed = 200.0f;
    texture = LoadTexture("resources/sprites/Ship_1.png");
}

void Player::Update(float dt)
{
    if (IsKeyDown(KEY_RIGHT)) position.x += speed * dt;
    if (IsKeyDown(KEY_LEFT))  position.x -= speed * dt;
    if (IsKeyDown(KEY_DOWN))  position.y += speed * dt;
    if (IsKeyDown(KEY_UP))    position.y -= speed * dt;

    // Clamp to screen bounds
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    if (position.x < 0) position.x = 0;
    if (position.x > screenW - texture.width * 2) position.x = screenW - texture.width * 2;
    if (position.y < 0) position.y = 0;
    if (position.y > screenH - texture.height * 2) position.y = screenH - texture.height * 2;
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
