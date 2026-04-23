#pragma once
#include "raylib.h"
#include "Level.h"

//Player animation frames
#define PLAYER_IDLE       0
#define PLAYER_START_MOVE 32
#define PLAYER_MOVE_1     64
#define PLAYER_MOVE_2     128
#define PLAYER_MOVE_3     224
#define PLAYER_MOVE_4     352

class Player
{
public:
    void Init(Vector2 startPos);
    void Update(float dt, Rectangle worldBounds, Level& level);
    void Draw();
    void DeInit();

    void Reset();

    Vector2 GetCenter() const;
    Rectangle GetBounds() const;
    bool IsSliding() const { return sliding; }

    Vector2 position;
    float speed;
    bool diedThisFrame;

private:
    bool IsWallAhead(Vector2 dir, Level& level) const;

    Vector2 startPosition;
    Vector2 currentDir;
    bool sliding;

    Texture2D texture;
    int frameIndex;
    float animTimer;
    float animSpeed;
    float drawScale;
    float drawRotation;
    bool flipX;
};