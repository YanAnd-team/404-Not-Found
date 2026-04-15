#include "Camera00.h"

void GameCamera::Init(int screenWidth, int screenHeight)
{
    camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void GameCamera::Update(Vector2 target)
{
    camera.target = target;
}

void GameCamera::BeginWorld()
{
    BeginMode2D(camera);
}

void GameCamera::EndWorld()
{
    EndMode2D();
}
