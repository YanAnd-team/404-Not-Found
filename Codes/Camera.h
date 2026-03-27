#pragma once
#include "raylib.h"

class GameCamera
{
public:
    void Init(int screenWidth, int screenHeight);
    void Update(Vector2 target);
    void BeginWorld();
    void EndWorld();

private:
    Camera2D camera;
};
