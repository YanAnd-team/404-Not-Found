#include "Game.h"

void Game::Init()
{
    screenWidth = 800;
    screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Raylib Handout - Game Structure");
    SetTargetFPS(60);

    // Game Manager state
    currentState = TITLE;
    playerLives = 3;
    score = 0;

    // Scene
    scene.Init();

    // Font (owned by Game for state screens)
    font = LoadFontEx("resources/fonts/gomarice_game_continue_03.ttf", FONT_SIZE, NULL, 0);
}

void Game::Update()
{
    float dt = GetFrameTime();

    switch (currentState)
    {
        case TITLE:    UpdateTitle();        break;
        case GAMEPLAY: UpdateGameplay(dt);   break;
        case GAMEOVER: UpdateGameOver();     break;
    }
}

void Game::Draw()
{
    BeginDrawing();                                                                          
    ClearBackground(BLACK);

    // World space (inside Camera2D)
    if (currentState == GAMEPLAY || currentState == GAMEOVER)
    {
        scene.DrawWorld();
    }

    // Screen space / Canvas (outside Camera2D)
    switch (currentState)
    {
        case TITLE:    DrawTitle();    break;
        case GAMEPLAY: DrawGameplay(); break;
        case GAMEOVER: DrawGameOver(); break;
    }

    EndDrawing();
}

void Game::DeInit()
{
    scene.DeInit();
    UnloadFont(font);
    CloseWindow();
}

bool Game::ShouldClose() const
{
    return WindowShouldClose();
}

// --- State Updates ---

void Game::UpdateTitle()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentState = GAMEPLAY;
        playerLives = 5;
        score = 0;
        scene.player.Reset();
    }
}

void Game::UpdateGameplay(float dt)
{
    scene.Update(dt);

    // Placeholder: score on SPACE
    if (IsKeyPressed(KEY_SPACE))
    {
        score += 100;
    }

    // Placeholder: lose life on BACKSPACE
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        playerLives--;
        if (playerLives <= 0)
        {
            currentState = GAMEOVER;
        }
    }
}

void Game::UpdateGameOver()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentState = TITLE;
    }
}

// --- State Draws (Canvas / Screen Space) ---

void Game::DrawTitle()
{
    // Dark overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

    // Title
    const char* titleText = "SPACE SHOOTER";
    Vector2 titleSize = MeasureTextEx(font, titleText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 titlePos = {
        screenWidth / 2.0f - titleSize.x / 2.0f,
        screenHeight / 2.0f - titleSize.y / 2.0f - 40
    };
    DrawTextEx(font, titleText, titlePos, FONT_SIZE * 2, TEXT_SPACING, WHITE);

    // Subtitle
    const char* subText = "Press ENTER to start";
    Vector2 subSize = MeasureTextEx(font, subText, FONT_SIZE, TEXT_SPACING);
    Vector2 subPos = {
        screenWidth / 2.0f - subSize.x / 2.0f,
        screenHeight / 2.0f + 20
    };
    DrawTextEx(font, subText, subPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}

void Game::DrawGameplay()
{
    // HUD
    DrawText(TextFormat("Lives: %d", playerLives), 10, 10, 20, WHITE);
    DrawText(TextFormat("Score: %d", score), 10, 35, 20, WHITE);

    // Controls hint
    DrawText("Arrows: Move | SPACE: Score | BACKSPACE: Lose life", 10, screenHeight - 25, 15, GRAY);
}

void Game::DrawGameOver()
{
    // Red overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.3f));

    // Game Over text
    const char* gameOverText = "GAME OVER";
    Vector2 goSize = MeasureTextEx(font, gameOverText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 goPos = {
        screenWidth / 2.0f - goSize.x / 2.0f,
        screenHeight / 2.0f - goSize.y / 2.0f - 40
    };
    DrawTextEx(font, gameOverText, goPos, FONT_SIZE * 2, TEXT_SPACING, RED);

    // Final score
    const char* scoreText = TextFormat("Final Score: %d", score);
    Vector2 scoreSize = MeasureTextEx(font, scoreText, FONT_SIZE, TEXT_SPACING);
    Vector2 scorePos = {
        screenWidth / 2.0f - scoreSize.x / 2.0f,
        screenHeight / 2.0f + 10
    };
    DrawTextEx(font, scoreText, scorePos, FONT_SIZE, TEXT_SPACING, WHITE);

    // Restart prompt
    const char* restartText = "Press ENTER to restart";
    Vector2 restartSize = MeasureTextEx(font, restartText, FONT_SIZE, TEXT_SPACING);
    Vector2 restartPos = {
        screenWidth / 2.0f - restartSize.x / 2.0f,
        screenHeight / 2.0f + 50
    };
    DrawTextEx(font, restartText, restartPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}
