#include "Game.h"

void Game::Init()
{
    screenWidth = 896;
    screenHeight = 630;

    InitWindow(screenWidth, screenHeight, "Tomb of Mask");
    SetTargetFPS(60);

    if (FileExists("resources/sprites/Icons/icon.png"))
    {
        Image iconImg = LoadImage("resources/sprites/Icons/icon.png");
        SetWindowIcon(iconImg);
        UnloadImage(iconImg);
    }

    currentState = INITIAL;

    bgTex    = FileExists("resources/sprites/Background/main_background.png")
               ? LoadTexture("resources/sprites/Background/main_background.png") : Texture2D{};
    logoTex  = FileExists("resources/sprites/Background/logo_tomb.png")
               ? LoadTexture("resources/sprites/Background/logo_tomb.png")       : Texture2D{};
    enterTex = FileExists("resources/sprites/Background/Press_ENTER.png")
               ? LoadTexture("resources/sprites/Background/Press_ENTER.png")     : Texture2D{};

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
    if (bgTex.id)    UnloadTexture(bgTex);
    if (logoTex.id)  UnloadTexture(logoTex);
    if (enterTex.id) UnloadTexture(enterTex);
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
        scene.LoadLevel(1);
        scene.OnEnterGameplay();
    }
}

void Game::UpdateGameplay(float dt)
{
    scene.Update(dt);

    if (scene.HasPlayerWon())
    {
        // [UNLOCK SYSTEM DISABLED FOR TESTING]
        // int wonLevel = scene.GetCurrentLevelNumber();
        // if (wonLevel + 1 <= 5 && wonLevel + 1 > maxUnlockedLevel)
        //     maxUnlockedLevel = wonLevel + 1;

        currentState = WIN;
        scene.OnPlayerWon();
        return;
    }

    if (scene.player.diedThisFrame)
    {
        currentState = GAMEOVER;
        return;
    }

    if (IsKeyPressed(KEY_B)) scene.ToggleHitboxes();

    // [UNLOCK SYSTEM DISABLED FOR TESTING] R: reset unlock progress and restart from level 1
    // if (IsKeyPressed(KEY_R)) { maxUnlockedLevel = 1; scene.LoadLevel(1); return; }

    // Number keys: load any level freely (unlock check disabled for testing)
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
        int nextLevel = scene.GetCurrentLevelNumber() + 1;
        if (nextLevel <= 5)
        {
            scene.LoadLevel(nextLevel);
            currentState = GAMEPLAY;
        }
        else
        {
            currentState = TITLE;
            scene.OnEnterMenu();
        }
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
    // Background - stretch to fill screen
    if (bgTex.id)
        DrawTexturePro(bgTex,
            { 0, 0, (float)bgTex.width, (float)bgTex.height },
            { 0, 0, (float)screenWidth, (float)screenHeight },
            { 0, 0 }, 0, WHITE);

    // Logo - 80% of screen width, centered, near top
    if (logoTex.id)
    {
        float logoW = screenWidth * 0.80f;
        float logoH = logoTex.height * (logoW / logoTex.width);
        DrawTexturePro(logoTex,
            { 0, 0, (float)logoTex.width, (float)logoTex.height },
            { (screenWidth - logoW) / 2.0f, 55, logoW, logoH },
            { 0, 0 }, 0, WHITE);
    }

    // "Press ENTER to play" - 30% of screen width, centered, near bottom
    if (enterTex.id)
    {
        float enterW = screenWidth * 0.30f;
        float enterH = enterTex.height * (enterW / enterTex.width);
        DrawTexturePro(enterTex,
            { 0, 0, (float)enterTex.width, (float)enterTex.height },
            { (screenWidth - enterW) / 2.0f, screenHeight - enterH - 55, enterW, enterH },
            { 0, 0 }, 0, WHITE);
    }
}

void Game::DrawGameplay()
{
    scene.DrawStarHUD();

    DrawText("Arrows: Move", 10, screenHeight - 58, 15, GRAY);
    DrawText(TextFormat("1~%d: Load Level  |  R: Reset  |  B: Hitboxes", maxUnlockedLevel), 10, screenHeight - 40, 15, GRAY);
    DrawText(TextFormat("Level %d / 5  (Unlocked up to %d)", scene.GetCurrentLevelNumber(), maxUnlockedLevel), 10, screenHeight - 22, 15, LIGHTGRAY);
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

    bool hasNextLevel = (scene.GetCurrentLevelNumber() < 5);
    const char* backText = hasNextLevel ? "Press ENTER to continue" : "Press ENTER to return to menu";
    Vector2 backTextSize = MeasureTextEx(font, backText, FONT_SIZE, TEXT_SPACING);
    Vector2 backPos = {
        screenWidth / 2.0f - backTextSize.x / 2.0f,
        screenHeight / 2.0f + 70
    };
    DrawTextEx(font, backText, backPos, FONT_SIZE, TEXT_SPACING, YELLOW);
}
