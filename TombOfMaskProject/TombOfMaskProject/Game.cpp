#include "Game.h"

void Game::Init()
{
    screenWidth = 896;
    screenHeight = 630;

    InitWindow(screenWidth, screenHeight, "Tomb of Mask");
    SetTargetFPS(60);

    // Game Manager state
    currentState = TITLE;
    playerLives = 3;
    score = 0;

    // Main Menu
    if (FileExists("resources/sprites/MainMenu.png"))
        menu = LoadTexture("resources/sprites/MainMenu.png");
    else
    {
        // make a placeholder 1x1 texture
        Image img = GenImageColor(512, 360, WHITE);
        menu = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Scene
    scene.Init();
    
    // Font (owned by Game for state screens)
    font = LoadFontEx("resources/fonts/easvhs.ttf", FONT_SIZE, NULL, 0);
    if (font.texture.id == 0)
    {
        // Fallback to default font provided by raylib
        font = GetFontDefault();
    }
}

void Game::Update()
{
    float dt = GetFrameTime();

    scene.UpdateAudio();

    switch (currentState)
    {
    case TITLE:    UpdateTitle();        break;
    case GAMEPLAY: UpdateGameplay(dt);   break;
    case GAMEOVER: UpdateGameOver();     break;
    case WIN:      UpdateWin();          break;
    }
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);

    // World space (inside Camera2D)
    if (currentState == GAMEPLAY || currentState == GAMEOVER || currentState == WIN)
    {
        scene.DrawWorld();
    }

    // Screen space / Canvas (outside Camera2D)
    switch (currentState)
    {
    case TITLE:    DrawTitle();    break;
    case GAMEPLAY: DrawGameplay(); break;
    case GAMEOVER: DrawGameOver(); break;
    case WIN:      DrawWin();      break;
    }

    EndDrawing();
}

void Game::DeInit()
{
    scene.DeInit();
    UnloadFont(font);
    UnloadTexture(menu);
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
        playerLives = 3;
        score = 0;
        scene.player.Reset();
        scene.OnEnterGameplay();
    }
}

void Game::UpdateGameplay(float dt)
{
    scene.Update(dt);

    if (scene.HasPlayerWon())
    {
        currentState = WIN;
        scene.OnPlayerWon();
        return;
    }

    // Load level by number key
    if (IsKeyPressed(KEY_ONE))   scene.LoadLevel(1);
    if (IsKeyPressed(KEY_TWO))   scene.LoadLevel(2);
    if (IsKeyPressed(KEY_THREE)) scene.LoadLevel(3);
    if (IsKeyPressed(KEY_FOUR))  scene.LoadLevel(4);
    if (IsKeyPressed(KEY_FIVE))  scene.LoadLevel(5);

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

void Game::UpdateWin()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentState = TITLE;
        scene.OnEnterMenu();
    }
}

void Game::UpdateGameOver()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentState = TITLE;
        scene.OnEnterMenu();
    }
}


// --- State Draws (Canvas / Screen Space) ---

void Game::DrawTitle()
{
    // Dark overlay
	DrawTextureEx(menu, Vector2{ 0, 0 }, 0, 3.5f, Fade(WHITE, 0.8f));

    // Title
    const char* titleText = "Tomb of the Mask";
    Vector2 titleSize = MeasureTextEx(font, titleText, FONT_SIZE * 2.5f, TEXT_SPACING);
    Vector2 titlePos = { screenWidth / 2.0f - titleSize.x / 2.0f, screenHeight / 2.0f - titleSize.y / 2.0f - 170 };
    DrawTextEx(font, titleText, titlePos, FONT_SIZE * 2.5f, TEXT_SPACING, ORANGE);

    // Subtitle
    const char* subText = "Press ENTER to start";
    Vector2 subSize = MeasureTextEx(font, subText, FONT_SIZE, TEXT_SPACING);
    Vector2 subPos = { screenWidth / 2.0f - subSize.x / 2.0f, screenHeight / 2.0f + 160 };
    DrawTextEx(font, subText, subPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}

void Game::DrawGameplay()
{
    // HUD
    DrawText(TextFormat("Lives: %d", playerLives), 10, 10, 20, WHITE);
    DrawText(TextFormat("Score: %d", score), 10, 35, 20, WHITE);

    // Scene-level UI (placeholder)
    scene.DrawUI();

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

void Game::DrawWin()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GREEN, 0.3f));

    const char* winText = "YOU WIN!";
    Vector2 winSize = MeasureTextEx(font, winText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 winPos = {
        screenWidth / 2.0f - winSize.x / 2.0f,
        screenHeight / 2.0f - winSize.y / 2.0f - 40
    };
    DrawTextEx(font, winText, winPos, FONT_SIZE * 2, TEXT_SPACING, GREEN);

    const char* scoreText = TextFormat("Final Score: %d", score);
    Vector2 scoreSize = MeasureTextEx(font, scoreText, FONT_SIZE, TEXT_SPACING);
    Vector2 scorePos = {
        screenWidth / 2.0f - scoreSize.x / 2.0f,
        screenHeight / 2.0f + 10
    };
    DrawTextEx(font, scoreText, scorePos, FONT_SIZE, TEXT_SPACING, WHITE);

    const char* backText = "Press ENTER to return to menu";
    Vector2 backSize = MeasureTextEx(font, backText, FONT_SIZE, TEXT_SPACING);
    Vector2 backPos = {
        screenWidth / 2.0f - backSize.x / 2.0f,
        screenHeight / 2.0f + 50
    };
    DrawTextEx(font, backText, backPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}
