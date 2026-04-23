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
    speed = 200.0f;
    currentDir = { 0, 0 };
    sliding = false;

    texture = LoadTexture("resources/sprites/Player.png");
    if (texture.width == 0 || texture.height == 0)
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
}

// Comprueba si hay pared (o límite del mundo) en la dirección dada
bool Player::IsWallAhead(Vector2 dir, Level& level) const
{
    int ts = level.GetTileSize();
    float drawW = 32.0f * drawScale;
    float drawH = 32.0f * drawScale;

    // Siguiente posición un píxel adelante
    float nx = position.x + dir.x;
    float ny = position.y + dir.y;

    auto isWall = [&](float px, float py) -> bool {
        int tx = (int)(px / ts);
        int ty = (int)(py / ts);
        char t = level.GetTileAt(tx, ty);
        return t == (char)Wall1 || t == (char)Wall2 || t == (char)Empty;
        };

    // Comprueba las dos esquinas frontales según la dirección
    if (dir.x > 0) // derecha
        return isWall(nx + drawW - 1, ny) || isWall(nx + drawW - 1, ny + drawH - 1);
    if (dir.x < 0) // izquierda
        return isWall(nx, ny) || isWall(nx, ny + drawH - 1);
    if (dir.y > 0) // abajo
        return isWall(nx, ny + drawH - 1) || isWall(nx + drawW - 1, ny + drawH - 1);
    if (dir.y < 0) // arriba
        return isWall(nx, ny) || isWall(nx + drawW - 1, ny);

    return false;
}

void Player::Update(float dt, Rectangle worldBounds, Level& level)
{
    // Solo se acepta nueva dirección si el jugador no está deslizándose,
    // o si ya chocó con la pared (sliding == false)
    if (!sliding)
    {
        if (IsKeyDown(KEY_RIGHT)) { currentDir = { 1,  0 }; sliding = true; }
        else if (IsKeyDown(KEY_LEFT)) { currentDir = { -1,  0 }; sliding = true; }
        else if (IsKeyDown(KEY_DOWN)) { currentDir = { 0,  1 }; sliding = true; }
        else if (IsKeyDown(KEY_UP)) { currentDir = { 0, -1 }; sliding = true; }
    }

    if (sliding)
    {
        // Comprobar si hay pared justo delante antes de mover
        if (IsWallAhead(currentDir, level))
        {
            // Se detiene: snap al tile más cercano para no quedar solapado
            int ts = level.GetTileSize();
            if (currentDir.x > 0)
                position.x = (float)((int)(position.x / ts) * ts);
            else if (currentDir.x < 0)
                position.x = (float)(((int)((position.x - 1) / ts) + 1) * ts);
            if (currentDir.y > 0)
                position.y = (float)((int)(position.y / ts) * ts);
            else if (currentDir.y < 0)
                position.y = (float)(((int)((position.y - 1) / ts) + 1) * ts);

            sliding = false;
            currentDir = { 0, 0 };
        }
        else
        {
            position.x += currentDir.x * speed * dt;
            position.y += currentDir.y * speed * dt;

            // Clamp a los límites del mundo
            float drawW = 32.0f * drawScale;
            float drawH = 32.0f * drawScale;
            if (position.x < worldBounds.x) { position.x = worldBounds.x; sliding = false; currentDir = { 0,0 }; }
            if (position.x > worldBounds.x + worldBounds.width - drawW) { position.x = worldBounds.x + worldBounds.width - drawW; sliding = false; currentDir = { 0,0 }; }
            if (position.y < worldBounds.y) { position.y = worldBounds.y; sliding = false; currentDir = { 0,0 }; }
            if (position.y > worldBounds.y + worldBounds.height - drawH) { position.y = worldBounds.y + worldBounds.height - drawH; sliding = false; currentDir = { 0,0 }; }
        }
    }

    // Animación
    if (!sliding)
    {
        frameIndex = 0;
        animTimer = 0.0f;
    }
    else
    {
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
    int srcX = framePositions[frameIndex];
    float drawW = 32.0f * drawScale;
    float drawH = 32.0f * drawScale;
    Rectangle src = { (float)srcX, 0, flipX ? -32.0f : 32.0f, 32.0f };
    // Pivot at center: dest origin is the center point, so offset by half size
    Rectangle dest = { position.x + drawW / 2.0f, position.y + drawH / 2.0f, drawW, drawH };
    DrawTexturePro(texture, src, dest, { drawW / 2.0f, drawH / 2.0f }, drawRotation, WHITE);
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
}

Vector2 Player::GetCenter() const
{
    float w = 32.0f * drawScale;
    float h = 32.0f * drawScale;
    return { position.x + w * 0.5f, position.y + h * 0.5f };
}

Rectangle Player::GetBounds() const
{
    return Rectangle{ position.x, position.y, 32.0f * drawScale, 32.0f * drawScale };
}