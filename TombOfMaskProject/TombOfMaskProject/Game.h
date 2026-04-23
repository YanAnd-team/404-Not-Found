#pragma once
#include "raylib.h"
#include "Scene.h"
#include "Level.h"

#define FONT_SIZE       32
#define TEXT_SPACING    2.0f

enum GameState {
    INITIAL,
    TITLE,
    GAMEPLAY,
    GAMEOVER,
    WIN
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
    void UpdateInitial();
    void UpdateTitle();
    void UpdateGameplay(float dt);
    void UpdateGameOver();
    void UpdateWin();

    void DrawInitial();
    void DrawTitle();
    void DrawGameplay();
    void DrawGameOver();
    void DrawWin();

    GameState currentState;

    int screenWidth;
    int screenHeight;

    Font font;
    Scene scene;
    Texture2D menu;
};