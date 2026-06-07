#pragma once
#include "raylib.h"

class Level;


class Player
{
public:
    void Init(Vector2 startPos);                                    //Initialize player state, texture, and animation vars
    void Update(float dt, Rectangle worldBounds, Level& level);    //Handle input, slide physics, snap-to-tile on wall hit, and animation
    void Draw();                                                    //Render with rotation and flip; origin pinned to the head centre so the body trails behind
    void DeInit();                                                  //Unload textures

    void Reset();                                                   //Teleport back to start position and flag death for this frame

    Vector2 GetCenter() const;
    Rectangle GetBounds() const;
    bool IsSliding() const { return sliding; }

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
