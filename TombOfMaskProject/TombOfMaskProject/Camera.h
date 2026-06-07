#pragma once
#include "raylib.h"

class GameCamera
{
public:
    void Init(int screenWidth, int screenHeight);   //Set camera offset to (screenWidth/3, screenHeight/3); the target is drawn at that point in screen space
    void Update(Vector2 target);                    //Set the camera target to follow the given position
    void BeginWorld();                              //Begin 2D camera mode; all subsequent draws are in world space
    void EndWorld();                                //End 2D camera mode; return to screen space

private:
    Camera2D camera;
};
