#pragma once
#include "raylib.h"
#include "Level.h"

#define PLAYER_IDLE 0
#define PLAYER_START_MOVE 32
#define PLAYER_MOVE_1 64
#define PLAYER_MOVE_2 128
#define PLAYER_MOVE_3 224
#define PLAYER_MOVE_4 352

class Player
{
public:
    void Init(Vector2 startPos);
    void Update(float dt, Rectangle worldBounds);
    void Draw();
    void DeInit();

    void Reset();

    Vector2 GetCenter() const;
    Rectangle GetBounds() const;

    Vector2 position;
    float speed;

private:
    Vector2 startPosition;
    Texture2D texture;
    // animation
    int frameIndex;
    int frameCount;
    float animTimer;
    float animSpeed; // seconds per frame
    float drawScale;
    float drawRotation; // degrees
    bool flipX;
};
