#include "Game.h"

void Game::Init()
{
    screenWidth = 896;
    screenHeight = 630;

    InitWindow(screenWidth, screenHeight, "Tomb of Mask");
    SetTargetFPS(60);

    currentState = INITIAL;

    if (FileExists("resources/sprites/MainMenu.png"))
        menu = LoadTexture("resources/sprites/MainMenu.png");
    else
    {
        Image img = GenImageColor(512, 360, WHITE);
        menu = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    scene.Init();

    font = LoadFontEx("resources/fonts/easvhs.ttf", FONT_SIZE, NULL, 0);
    if (font.texture.id == 0)
        font = GetFontDefault();
}

void Game::Update()
{
    float dt = GetFrameTime();

    scene.UpdateAudio();

    switch (currentState)
    {
    case INITIAL:  UpdateInitial();      break;
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
        scene.DrawWorld();

    switch (currentState)
    {
    case INITIAL:  DrawInitial();  break;
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

void Game::UpdateInitial()
{
    if (IsKeyPressed(KEY_ENTER))
        currentState = TITLE;
}

void Game::UpdateTitle()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        currentState = GAMEPLAY;
        scene.ReloadLevel();
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

// --- State Draws ---

void Game::DrawInitial()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 5, 25, 255});

    float centerX = screenWidth * 0.5f;
    Vector2 textSize; //Reused to measure text width for center-alignment

    textSize = MeasureTextEx(font, "TOMB OF MASK", 40, TEXT_SPACING);
    DrawTextEx(font, "TOMB OF MASK", { centerX - textSize.x * 0.5f, 30 }, 40, TEXT_SPACING, ORANGE);
    textSize = MeasureTextEx(font, "Arcade-Style Sliding Maze Game", 19, TEXT_SPACING);
    DrawTextEx(font, "Arcade-Style Sliding Maze Game", { centerX - textSize.x * 0.5f, 82 }, 19, TEXT_SPACING, LIGHTGRAY);

    DrawLine(60, 115, screenWidth - 60, 115, Fade(WHITE, 0.35f));

    textSize = MeasureTextEx(font, "Subject:    Game Design and Development", 19, TEXT_SPACING);
    DrawTextEx(font, "Subject:    Game Design and Development", { centerX - textSize.x * 0.5f, 129 }, 19, TEXT_SPACING, WHITE);
    textSize = MeasureTextEx(font, "Degree:     Bachelor's Degree in Game Design and Development", 17, TEXT_SPACING);
    DrawTextEx(font, "Degree:     Bachelor's Degree in Game Design and Development", { centerX - textSize.x * 0.5f, 154 }, 17, TEXT_SPACING, WHITE);
    textSize = MeasureTextEx(font, "University: Universitat Politecnica de Catalunya", 19, TEXT_SPACING);
    DrawTextEx(font, "University: Universitat Politecnica de Catalunya", { centerX - textSize.x * 0.5f, 177 }, 19, TEXT_SPACING, WHITE);
    textSize = MeasureTextEx(font, "Tutors:     De Pedro Lombao Rodrigo", 19, TEXT_SPACING);
    DrawTextEx(font, "Tutors:     De Pedro Lombao Rodrigo", { centerX - textSize.x * 0.5f, 202 }, 19, TEXT_SPACING, WHITE);

    DrawLine(60, 234, screenWidth - 60, 234, Fade(WHITE, 0.35f));

    textSize = MeasureTextEx(font, "TEAM MEMBERS", 23, TEXT_SPACING);
    DrawTextEx(font, "TEAM MEMBERS", { centerX - textSize.x * 0.5f, 252 }, 23, TEXT_SPACING, YELLOW);
    textSize = MeasureTextEx(font, "WEIHAOYAN   -   Art / Design", 19, TEXT_SPACING);
    DrawTextEx(font, "WEIHAOYAN   -   Art / Design", { centerX - textSize.x * 0.5f, 290 }, 19, TEXT_SPACING, WHITE);
    textSize = MeasureTextEx(font, "YIXIANGLU   -   Programming / Audio", 19, TEXT_SPACING);
    DrawTextEx(font, "YIXIANGLU   -   Programming / Audio", { centerX - textSize.x * 0.5f, 318 }, 19, TEXT_SPACING, WHITE);

    DrawLine(60, 353, screenWidth - 60, 353, Fade(WHITE, 0.35f));

    textSize = MeasureTextEx(font, "GitHub:  github.com/YanAnd-team/404-Not-Found", 17, TEXT_SPACING);
    DrawTextEx(font, "GitHub:  github.com/YanAnd-team/404-Not-Found", { centerX - textSize.x * 0.5f, 371 }, 17, TEXT_SPACING, SKYBLUE);

    DrawLine(60, 403, screenWidth - 60, 403, Fade(WHITE, 0.35f));

    textSize = MeasureTextEx(font, "Slide through 5 levels of ancient tombs.", 17, TEXT_SPACING);
    DrawTextEx(font, "Slide through 5 levels of ancient tombs.", { centerX - textSize.x * 0.5f, 422 }, 17, TEXT_SPACING, LIGHTGRAY);
    textSize = MeasureTextEx(font, "Dodge traps and enemies, reach the exit to win.", 17, TEXT_SPACING);
    DrawTextEx(font, "Dodge traps and enemies, reach the exit to win.", { centerX - textSize.x * 0.5f, 446 }, 17, TEXT_SPACING, LIGHTGRAY);

    textSize = MeasureTextEx(font, "Press ENTER to continue", 22, TEXT_SPACING);
    DrawTextEx(font, "Press ENTER to continue", { centerX - textSize.x * 0.5f, screenHeight - 52.0f }, 22, TEXT_SPACING, YELLOW);
}

void Game::DrawTitle()
{
	DrawTextureEx(menu, Vector2{ 0, 0 }, 0, 3.5f, Fade(WHITE, 0.8f));

    const char* titleText = "Tomb of the Mask";
    Vector2 titleTextSize = MeasureTextEx(font, titleText, FONT_SIZE * 2.5f, TEXT_SPACING);
    Vector2 titlePos = { screenWidth / 2.0f - titleTextSize.x / 2.0f, screenHeight / 2.0f - titleTextSize.y / 2.0f - 170 };
    DrawTextEx(font, titleText, titlePos, FONT_SIZE * 2.5f, TEXT_SPACING, ORANGE);

    const char* promptText = "Press ENTER to start";
    Vector2 promptTextSize = MeasureTextEx(font, promptText, FONT_SIZE, TEXT_SPACING);
    Vector2 promptPos = { screenWidth / 2.0f - promptTextSize.x / 2.0f, screenHeight / 2.0f + 160 };
    DrawTextEx(font, promptText, promptPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}

void Game::DrawGameplay()
{
    scene.DrawUI();
    scene.DrawStarHUD();

    DrawText("Arrows: Move", 10, screenHeight - 40, 15, GRAY);
    DrawText("1 ~ 5: Load Level", 10, screenHeight - 22, 15, GRAY);
}

void Game::DrawGameOver()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.3f));

    const char* gameOverText = "GAME OVER";
    Vector2 gameOverTextSize = MeasureTextEx(font, gameOverText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 gameOverPos = {
        screenWidth / 2.0f - gameOverTextSize.x / 2.0f,
        screenHeight / 2.0f - gameOverTextSize.y / 2.0f - 20
    };
    DrawTextEx(font, gameOverText, gameOverPos, FONT_SIZE * 2, TEXT_SPACING, RED);

    const char* restartText = "Press ENTER to return to menu";
    Vector2 restartTextSize = MeasureTextEx(font, restartText, FONT_SIZE, TEXT_SPACING);
    Vector2 restartPos = {
        screenWidth / 2.0f - restartTextSize.x / 2.0f,
        screenHeight / 2.0f + 30
    };
    DrawTextEx(font, restartText, restartPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}

void Game::DrawWin()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GREEN, 0.3f));

    const char* winText = "YOU WIN!";
    Vector2 winTextSize = MeasureTextEx(font, winText, FONT_SIZE * 2, TEXT_SPACING);
    Vector2 winPos = {
        screenWidth / 2.0f - winTextSize.x / 2.0f,
        screenHeight / 2.0f - winTextSize.y / 2.0f - 80
    };
    DrawTextEx(font, winText, winPos, FONT_SIZE * 2, TEXT_SPACING, GREEN);

    scene.DrawWinStars();

    const char* backText = "Press ENTER to return to menu";
    Vector2 backTextSize = MeasureTextEx(font, backText, FONT_SIZE, TEXT_SPACING);
    Vector2 backPos = {
        screenWidth / 2.0f - backTextSize.x / 2.0f,
        screenHeight / 2.0f + 70
    };
    DrawTextEx(font, backText, backPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}
