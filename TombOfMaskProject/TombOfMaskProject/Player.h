#pragma once
#include "raylib.h"
#include "Level.h"

class Player
{
public:
    void Init(Vector2 startPos);
    void Update(float dt, Rectangle worldBounds);
    void Draw();
    void DeInit();

    void Reset();

    Vector2 GetCenter() const;

    Vector2 position;
    float speed;

private:
    Vector2 startPosition;
    Texture2D texture;
};
