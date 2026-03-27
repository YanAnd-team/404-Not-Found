#pragma once
#include "raylib.h"
#include "Player.h"
#include "Camera.h"

class Scene
{
public:
    void Init();
    void Update(float dt);
    void DrawWorld();
    void DrawUI();
    void DeInit();

    Player player;
    GameCamera camera;

private:
    Texture2D background;
    Sound sound;
    Music music;
    Font font;
};
