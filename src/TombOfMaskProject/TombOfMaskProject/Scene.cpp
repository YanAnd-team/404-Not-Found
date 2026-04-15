#include "Scene00.h"

void Scene::Init()
{
    // Audio
    InitAudioDevice();
    sound = LoadSound("resources/raylib_audio_resources/sound.wav");
    music = LoadMusicStream("resources/raylib_audio_resources/country.mp3");
    music.looping = true;
    PlayMusicStream(music);

    // Font
    font = LoadFontEx("resources/fonts/gomarice_game_continue_03.ttf", 32, NULL, 0);

    // Background
    background = LoadTexture("resources/sprites/space_background.png");

    // Camera
    camera.Init(GetScreenWidth(), GetScreenHeight());

    // Player
    player.Init({ 400, 200 });
}

void Scene::Update(float dt)
{
    player.Update(dt);
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
    UnloadFont(font);
    UnloadTexture(background);
    UnloadSound(sound);
    UnloadMusicStream(music);
    CloseAudioDevice();
}
