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
    void Init(Vector2 startPos);                                    //Initialize player state, texture, and animation vars
    void Update(float dt, Rectangle worldBounds, Level& level);    //Handle input, slide physics, snap-to-tile on wall hit, and animation
    void Draw();                                                    //Render with rotation and flip; pivot pinned to sprite center
    void DeInit();                                                  //Unload texture

    void Reset();                                                   //Teleport back to start position and flag death for this frame

    Vector2 GetCenter() const;
    Rectangle GetBounds() const;
    bool IsSliding() const { return sliding; }
    Vector2 GetLastSlideDir() const { return lastSlideDir; } //Last non-zero slide direction (kept after stopping)

    Vector2 position;
    float speed;
    bool diedThisFrame;

private:

    Vector2 startPosition;
    Vector2 currentDir;
    Vector2 lastSlideDir;
    bool sliding;

    Texture2D idleTex;
    Texture2D moveTex;
    int frameIndex;
    int idleFrameIdx;
    float animTimer;
    float idleAnimTimer;
    float animSpeed;
    float drawScale;
    float drawRotation;
    bool flipX;
};
