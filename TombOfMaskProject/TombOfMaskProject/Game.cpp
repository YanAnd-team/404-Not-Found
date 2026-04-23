#include "Game.h"

void Game::Init()
{
    screenWidth = 896;
    screenHeight = 630;

    InitWindow(screenWidth, screenHeight, "Tomb of Mask");
    SetTargetFPS(60);

    currentState = INITIAL;

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
    {
        scene.DrawWorld();
    }

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

void Game::DrawInitial()
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{5, 5, 25, 255});

    float cx = screenWidth * 0.5f;

    auto dc = [&](const char* text, float y, float size, Color col)
    {
        Vector2 sz = MeasureTextEx(font, text, size, TEXT_SPACING);
        DrawTextEx(font, text, {cx - sz.x * 0.5f, y}, size, TEXT_SPACING, col);
    };

    // Title + subtitle
    dc("TOMB OF MASK",                       30,  40, ORANGE);
    dc("Arcade-Style Sliding Maze Game",     82,  19, LIGHTGRAY);

    DrawLine(60, 115, screenWidth - 60, 115, Fade(WHITE, 0.35f));

    // Project info
    dc("Subject:    Game Design and Development",                      129, 19, WHITE);
    dc("Degree:     Bachelor's Degree in Game Design and Development", 154, 17, WHITE);
    dc("University: Universitat Politecnica de Catalunya",             177, 19, WHITE);
    dc("Tutors:     De Pedro Lombao Rodrigo",                         202, 19, WHITE);

    DrawLine(60, 234, screenWidth - 60, 234, Fade(WHITE, 0.35f));

    // Team
    dc("TEAM MEMBERS",                          252, 23, YELLOW);
    dc("WEIHAOYAN   -   Art / Design",           290, 19, WHITE);
    dc("YIXIANGLU   -   Programming / Audio",    318, 19, WHITE);

    DrawLine(60, 353, screenWidth - 60, 353, Fade(WHITE, 0.35f));

    // GitHub
    dc("GitHub:  github.com/YanAnd-team/404-Not-Found", 371, 17, SKYBLUE);

    DrawLine(60, 403, screenWidth - 60, 403, Fade(WHITE, 0.35f));

    // Short description
    dc("Slide through 5 levels of ancient tombs.",         422, 17, LIGHTGRAY);
    dc("Dodge traps and enemies, reach the exit to win.",  446, 17, LIGHTGRAY);

    // Prompt
    dc("Press ENTER to continue", screenHeight - 52.0f, 22, YELLOW);
}

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
    DrawText("Arrows: Move", 10, screenHeight - 40, 15, GRAY);
    DrawText("1 ~ 5: Load Level", 10, screenHeight - 22, 15, GRAY);
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
