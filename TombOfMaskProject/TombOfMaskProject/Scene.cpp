#include "Scene.h"

void Scene::Init()
{
    // Audio
    // Initialize audio device if not already initialized
    InitAudioDevice();
    if (FileExists("resources/SFX/Coins.wav"))
    {
        sound[0] = LoadSound("resources/SFX/Coins.wav");
		soundLoaded[0] = true;
    }
    if (FileExists("resources/SFX/Movement.wav"))
    {
        sound[1] = LoadSound("resources/SFX/Movement.wav");
		soundLoaded[1] = true;
    }
    if(FileExists("resources/SFX/PlayerDie.wav"))
    {
        sound[2] = LoadSound("resources/SFX/PlayerDie.wav");
		soundLoaded[2] = true;
    }
    if(FileExists("resources/SFX/Win.wav"))
    {
        sound[3] = LoadSound("resources/SFX/Win.wav");
		soundLoaded[3] = true;
    }

    if(FileExists("resources/Music/MenuSong.mp3"))
    {
        music[0] = LoadMusicStream("resources/Music/MenuSong.mp3");
		music[0].looping = true;
        PlayMusicStream(music[0]);
		musicLoaded[0] = true;
	}
    if(FileExists("resources/Music/BattleSong.mp3"))
    {
		music[1] = LoadMusicStream("resources/Music/BattleSong.mp3");
        music[1].looping = true;
		musicLoaded[1] = true;
	}

    // Font
    font = LoadFontEx("resources/fonts/easvhs.ttf", 32, NULL, 0);

    // Camera
    camera.Init(GetScreenWidth(), GetScreenHeight());

    // Level
    level.Init();
    if (!level.Load(1))
    {
        // failed to load level 1, keep player at default
        player.Init({ 400, 200 });
    }
    else
    {
        // place player at level start
        Vector2 start = level.GetStartPosition();
        player.Init(start);
    }
}

void Scene::Update(float dt)
{
    // Provide world bounds to player (from level if loaded)
    Rectangle world = level.GetWorldBounds();
    player.Update(dt, world);
    camera.Update(player.GetCenter());
    UpdateMusicStream(music[0]);
}

void Scene::DrawWorld()
{
    camera.BeginWorld();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    level.Draw();
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
    level.DeInit();
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
    if (soundLoaded[0])
    {
        StopSound(sound[0]);
        UnloadSound(sound[0]);
    }
    if (soundLoaded[1])
    {
        StopSound(sound[1]);
        UnloadSound(sound[1]);
    }
    if (soundLoaded[2])
    {
        StopSound(sound[2]);
        UnloadSound(sound[2]);
    }
    if (soundLoaded[3])
    {
        StopSound(sound[3]);
		UnloadSound(sound[3]);
    }
    UnloadFont(font);
    CloseAudioDevice();
}
