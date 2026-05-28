#include "Scene.h"

void Scene::Init()
{
    InitAudioDevice();
    if (FileExists("resources/SFX/Star-pick-up.mp3"))
    {
        sound[0] = LoadSound("resources/SFX/Star-pick-up.mp3");
        soundLoaded[0] = true;
    }
    if (FileExists("resources/SFX/Player-movement.mp3"))
    {
        sound[1] = LoadSound("resources/SFX/Player-movement.mp3");
        soundLoaded[1] = true;
    }
    if (FileExists("resources/SFX/Death.mp3"))
    {
        sound[2] = LoadSound("resources/SFX/Death.mp3");
        soundLoaded[2] = true;
    }
    if (FileExists("resources/SFX/Win.mp3"))
    {
        sound[3] = LoadSound("resources/SFX/Win.mp3");
        soundLoaded[3] = true;
    }
    if (FileExists("resources/SFX/1-star.mp3"))
    {
        sound[4] = LoadSound("resources/SFX/1-star.mp3");
        soundLoaded[4] = true;
    }
    if (FileExists("resources/SFX/2-star.mp3"))
    {
        sound[5] = LoadSound("resources/SFX/2-star.mp3");
        soundLoaded[5] = true;
    }
    if (FileExists("resources/SFX/3-star.mp3"))
    {
        sound[6] = LoadSound("resources/SFX/3-star.mp3");
        soundLoaded[6] = true;
    }

    if (FileExists("resources/Music/Soundtrack.mp3"))
    {
        music[0] = LoadMusicStream("resources/Music/Soundtrack.mp3");
        music[0].looping = true;
        PlayMusicStream(music[0]);
        musicLoaded[0] = true;
    }
    if (FileExists("resources/Music/InGame-soundtrack.mp3"))
    {
        music[1] = LoadMusicStream("resources/Music/InGame-soundtrack.mp3");
        music[1].looping = true;
        musicLoaded[1] = true;
    }

    if (FileExists("resources/sprites/StarToComp.png"))
    {
        starToCompTex = LoadTexture("resources/sprites/StarToComp.png");
        starToCompLoaded = true;
    }

    font = LoadFontEx("resources/fonts/easvhs.ttf", 32, NULL, 0);

    camera.Init(GetScreenWidth(), GetScreenHeight());

    level.Init();
    if (!level.Load(1))
        player.Init({ 400, 200 });
    else
        player.Init(level.GetStartPosition());

    // Iterate all tiles and create matching entities
    int levelWidth  = level.GetWidth();
    int levelHeight = level.GetHeight();
    int tileSize    = level.GetTileSize();
    for (int y = 0; y < levelHeight; ++y)
    {
        for (int x = 0; x < levelWidth; ++x)
        {
            char tile = level.GetTileAt(x, y);
            Vector2 pos = { (float)(x * tileSize), (float)(y * tileSize) };
            Entity* entity = CreateEntityFromTile(tile, pos, level, &starCount);
            if (entity) entities.push_back(entity);
        }
    }
}

void Scene::Update(float dt)
{
    Rectangle worldBounds = level.GetWorldBounds();
    player.Update(dt, worldBounds, level);
    camera.Update(player.GetCenter());

    int prevStarCount = starCount;
    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (entities[i]) entities[i]->Update(dt, player, entities, level);
    }
    if (starCount > prevStarCount && soundLoaded[0])
        PlaySound(sound[0]);

    // Remove inactive entities and free memory
    for (size_t i = 0; i < entities.size(); )
    {
        if (!entities[i] || !entities[i]->IsActive())
        {
            delete entities[i];
            entities.erase(entities.begin() + i);
        }
        else ++i;
    }

    // Play movement sound on slide start
    bool nowSliding = player.IsSliding();
    if (nowSliding && !wasSliding && soundLoaded[1])
        PlaySound(sound[1]);
    wasSliding = nowSliding;

    if (player.diedThisFrame && soundLoaded[2])
        PlaySound(sound[2]);
}

void Scene::DrawWorld()
{
    camera.BeginWorld();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.7f));
    level.Draw();
    for (auto entity : entities) if (entity && entity->IsActive()) entity->Draw();
    player.Draw();
    if (showHitboxes) DrawHitboxes();

    camera.EndWorld();
}

void Scene::DrawHitboxes() const
{
    int ts = level.GetTileSize();

    // Walls
    for (int y = 0; y < level.GetHeight(); ++y)
        for (int x = 0; x < level.GetWidth(); ++x)
        {
            char t = level.GetTileAt(x, y);
            if (t == '1')
                DrawRectangleLinesEx({ (float)(x * ts), (float)(y * ts), (float)ts, (float)ts }, 1.0f, RED);
        }

    // Entities
    for (auto e : entities)
        if (e && e->IsActive())
            DrawRectangleLinesEx(e->GetBounds(), 1.5f, ORANGE);

    // Goal
    Vector2 gp = level.GetGoalPosition();
    DrawRectangleLinesEx({ gp.x, gp.y, (float)ts, (float)ts }, 2.0f, YELLOW);

    // Player (drawn last so it's always on top)
    DrawRectangleLinesEx(player.GetBounds(), 2.0f, GREEN);
}

void Scene::UpdateAudio()
{
    if (musicLoaded[0]) UpdateMusicStream(music[0]);
    if (musicLoaded[1]) UpdateMusicStream(music[1]);
}

void Scene::OnEnterMenu()
{
    if (musicLoaded[1]) StopMusicStream(music[1]);
    if (musicLoaded[0])
    {
        StopMusicStream(music[0]);
        PlayMusicStream(music[0]);
    }
}

void Scene::OnEnterGameplay()
{
    if (musicLoaded[0]) StopMusicStream(music[0]);
    if (musicLoaded[1])
    {
        StopMusicStream(music[1]);
        PlayMusicStream(music[1]);
    }
}

void Scene::OnPlayerWon()
{
    if (soundLoaded[3]) PlaySound(sound[3]);
    if      (starCount >= 3 && soundLoaded[6]) PlaySound(sound[6]);
    else if (starCount >= 2 && soundLoaded[5]) PlaySound(sound[5]);
    else if (starCount >= 1 && soundLoaded[4]) PlaySound(sound[4]);
}

void Scene::ReloadLevel()
{
    LoadLevel(currentLevelNumber);
}

void Scene::LoadLevel(int levelNumber)
{
    currentLevelNumber = levelNumber;
    for (auto entity : entities) { if (entity) delete entity; }
    entities.clear();
    starCount = 0;

    if (!level.Load(levelNumber))
        return;

    player.Init(level.GetStartPosition());

    int levelWidth  = level.GetWidth();
    int levelHeight = level.GetHeight();
    int tileSize    = level.GetTileSize();
    for (int y = 0; y < levelHeight; ++y)
    {
        for (int x = 0; x < levelWidth; ++x)
        {
            char tile = level.GetTileAt(x, y);
            Vector2 pos = { (float)(x * tileSize), (float)(y * tileSize) };
            Entity* entity = CreateEntityFromTile(tile, pos, level, &starCount);
            if (entity) entities.push_back(entity);
        }
    }
}

int Scene::GetCurrentLevelNumber() const { return currentLevelNumber; }

bool Scene::HasPlayerWon() const
{
    Vector2 goalPos = level.GetGoalPosition();
    Rectangle goalRect = { goalPos.x, goalPos.y, (float)level.GetTileSize(), (float)level.GetTileSize() };
    return CheckCollisionRecs(player.GetBounds(), goalRect);
}

void Scene::DrawStarHUD()
{
    if (!level.IsStarLoaded()) return;
    Texture2D starTex = level.GetStarTex();
    for (int i = 0; i < starCount && i < 3; ++i)
    {
        Rectangle dest = { 10.0f + i * 42.0f, 10.0f, 32.0f, 32.0f };
        DrawTexturePro(starTex, Rectangle{0,0,(float)starTex.width,(float)starTex.height}, dest, Vector2{0,0}, 0, WHITE);
    }
}

void Scene::DrawWinStars()
{
    const float slotSize = 48.0f;
    const float gap = 16.0f;
    float totalWidth = 3 * slotSize + 2 * gap;
    float startX = GetScreenWidth() / 2.0f - totalWidth / 2.0f;
    float startY = GetScreenHeight() / 2.0f - slotSize / 2.0f;

    bool hasStar = level.IsStarLoaded();
    Texture2D starTex = hasStar ? level.GetStarTex() : Texture2D{};

    for (int i = 0; i < 3; ++i)
    {
        Rectangle dest = { startX + i * (slotSize + gap), startY, slotSize, slotSize };
        if (i < starCount && hasStar)
            DrawTexturePro(starTex, Rectangle{0,0,(float)starTex.width,(float)starTex.height}, dest, Vector2{0,0}, 0, WHITE);
        else if (starToCompLoaded)
            DrawTexturePro(starToCompTex, Rectangle{0,0,(float)starToCompTex.width,(float)starToCompTex.height}, dest, Vector2{0,0}, 0, WHITE);
        else
            DrawRectangleRec(dest, Fade(WHITE, 0.3f));
    }
}

void Scene::DeInit()
{
    player.DeInit();
    level.DeInit();
    for (auto entity : entities) { if (entity) delete entity; }
    entities.clear();
    if (musicLoaded[0])
    {
        StopMusicStream(music[0]);
        UnloadMusicStream(music[0]);
    }
	if (musicLoaded[1])
    {
        StopMusicStream(music[1]);
        UnloadMusicStream(music[1]);
    }
    for (int i = 0; i < 7; ++i)
        if (soundLoaded[i]) { StopSound(sound[i]); UnloadSound(sound[i]); }
    if (starToCompLoaded) UnloadTexture(starToCompTex);
    UnloadFont(font);
    CloseAudioDevice();
}
