#pragma once
#include "raylib.h"

class GameCamera
{
public:
    void Init(int screenWidth, int screenHeight);   //Set offset to screen center so the target stays centered during world rendering
    void Update(Vector2 target);                    //Set the camera target to follow the given position
    void BeginWorld();                              //Begin 2D camera mode; all subsequent draws are in world space
    void EndWorld();                                //End 2D camera mode; return to screen space

private:
    Camera2D camera;
};
