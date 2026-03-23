#pragma once
#include "raylib.h"
#include "Scene.h"

#define FONT_SIZE       32
#define TEXT_SPACING    2.0f

enum GameState {
    TITLE,
    GAMEPLAY,
    GAMEOVER
};

class Game
{
public:
    void Init();
    void Update();
    void Draw();
    void DeInit();

    bool ShouldClose() const;

private:
    void UpdateTitle();
    void UpdateGameplay(float dt);
    void UpdateGameOver();

    void DrawTitle();
    void DrawGameplay();
    void DrawGameOver();

    GameState currentState;
    int playerLives;
    int score;

    int screenWidth;
    int screenHeight;

    Font font;
    Scene scene;
};
