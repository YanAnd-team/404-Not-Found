#include "Scene.h"

void Scene::Init()
{
    // Audio
    // Initialize audio device if not already initialized
    InitAudioDevice();
    soundLoaded = false;
    musicLoaded = false;
    if (FileExists("resources/raylib_audio_resources/sound.wav"))
    {
        sound = LoadSound("resources/raylib_audio_resources/sound.wav");
        soundLoaded = true;
    }
    if (FileExists("resources/raylib_audio_resources/country.mp3"))
    {
        music = LoadMusicStream("resources/raylib_audio_resources/country.mp3");
        music.looping = true;
        PlayMusicStream(music);
        musicLoaded = true;
    }

    // Font
    font = LoadFontEx("resources/fonts/gomarice_game_continue_03.ttf", 32, NULL, 0);

    // Background
    if (FileExists("resources/sprites/space_background.png"))
        background = LoadTexture("resources/sprites/space_background.png");
    else
    {
        // make a placeholder 1x1 texture
        Image img = GenImageColor(1, 1, BLACK);
        background = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Camera
    camera.Init(GetScreenWidth(), GetScreenHeight());

    // Player
    player.Init({ 400, 200 });
}

void Scene::Update(float dt)
{
    // Provide world bounds to player (use full screen for now)
    Rectangle world = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    player.Update(dt, world);
    camera.Update(player.GetCenter());
    UpdateMusicStream(music);
}

void Scene::DrawWorld()
{
    camera.BeginWorld();

    DrawTextureEx(background, Vector2{ 0, 0 }, 0, 1, WHITE);
    player.Draw();

    camera.EndWorld();
}

void Scene::DrawUI()
{
    // Scene-level UI could go here (e.g. minimap, objective markers)
}

void Scene::DeInit()
{
    player.DeInit();
    if (musicLoaded)
    {
        StopMusicStream(music);
        UnloadMusicStream(music);
    }
    if (soundLoaded)
    {
        StopSound(sound);
        UnloadSound(sound);
    }
    UnloadFont(font);
    UnloadTexture(background);
    CloseAudioDevice();
}
