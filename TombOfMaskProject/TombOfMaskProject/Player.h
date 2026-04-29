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

    Vector2 GetCenter() const;      //Return the center point of the player sprite
    Rectangle GetBounds() const;    //Return the collision rectangle of the player
    bool IsSliding() const { return sliding; }  //Return true if the player is currently moving

    Vector2 position;
    float speed;
    bool diedThisFrame;

private:
    bool IsWallAhead(Vector2 dir, Level& level) const;  //Check two leading corners in the movement direction for walls

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
