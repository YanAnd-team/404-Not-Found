#include "Game.h"

void Game::Init()
{
    screenWidth = 896;
    screenHeight = 630;

    InitWindow(screenWidth, screenHeight, "Tomb of Mask");
    SetTargetFPS(60);

    currentState = TITLE;

    // Main Menu
    if (FileExists("resources/sprites/MainMenu.png"))
        menu = LoadTexture("resources/sprites/MainMenu.png");
    else
    {
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

    if (currentState == GAMEPLAY || currentState == GAMEOVER || currentState == WIN)
    {
        scene.DrawWorld();
    }

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

    if (scene.player.diedThisFrame)
    {
        currentState = GAMEOVER;
        return;
    }

    // Load level by number key
    if (IsKeyPressed(KEY_ONE))   scene.LoadLevel(1);
    if (IsKeyPressed(KEY_TWO))   scene.LoadLevel(2);
    if (IsKeyPressed(KEY_THREE)) scene.LoadLevel(3);
    if (IsKeyPressed(KEY_FOUR))  scene.LoadLevel(4);
    if (IsKeyPressed(KEY_FIVE))  scene.LoadLevel(5);
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
	DrawTextureEx(menu, Vector2{ 0, 0 }, 0, 3.5f, Fade(WHITE, 0.8f));

    const char* titleText = "Tomb of the Mask";
    Vector2 titleSize = MeasureTextEx(font, titleText, FONT_SIZE * 2.5f, TEXT_SPACING);
    Vector2 titlePos = { screenWidth / 2.0f - titleSize.x / 2.0f, screenHeight / 2.0f - titleSize.y / 2.0f - 170 };
    DrawTextEx(font, titleText, titlePos, FONT_SIZE * 2.5f, TEXT_SPACING, ORANGE);

    const char* subText = "Press ENTER to start";
    Vector2 subSize = MeasureTextEx(font, subText, FONT_SIZE, TEXT_SPACING);
    Vector2 subPos = { screenWidth / 2.0f - subSize.x / 2.0f, screenHeight / 2.0f + 160 };
    DrawTextEx(font, subText, subPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}

void Game::DrawGameplay()
{
    scene.DrawUI();
    scene.DrawStarHUD();

    // Controls hint
    DrawText("Arrows: Move", 10, screenHeight - 25, 15, GRAY);
}

void Game::DrawGameOver()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.3f));

    const char* gameOverText = "GAME OVER";
    Vector2 goSize = MeasureTextEx(font, gameOverText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 goPos = {
        screenWidth / 2.0f - goSize.x / 2.0f,
        screenHeight / 2.0f - goSize.y / 2.0f - 20
    };
    DrawTextEx(font, gameOverText, goPos, FONT_SIZE * 2, TEXT_SPACING, RED);

    const char* restartText = "Press ENTER to return to menu";
    Vector2 restartSize = MeasureTextEx(font, restartText, FONT_SIZE, TEXT_SPACING);
    Vector2 restartPos = {
        screenWidth / 2.0f - restartSize.x / 2.0f,
        screenHeight / 2.0f + 30
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
        screenHeight / 2.0f - winSize.y / 2.0f - 80
    };
    DrawTextEx(font, winText, winPos, FONT_SIZE * 2, TEXT_SPACING, GREEN);

    // star slots drawn centered vertically in the gap between the two texts
    scene.DrawWinStars();

    const char* backText = "Press ENTER to return to menu";
    Vector2 backSize = MeasureTextEx(font, backText, FONT_SIZE, TEXT_SPACING);
    Vector2 backPos = {
        screenWidth / 2.0f - backSize.x / 2.0f,
        screenHeight / 2.0f + 70
    };
    DrawTextEx(font, backText, backPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}
