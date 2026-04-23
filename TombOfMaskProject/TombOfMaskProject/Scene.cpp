#include "Scene.h"
#include "Entities.h"

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
    // Create entities from level tiles
    int w = level.GetWidth();
    int h = level.GetHeight();
    int ts = level.GetTileSize();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            char t = level.GetTileAt(x, y);
            Vector2 pos = { (float)(x * ts), (float)(y * ts) };
            switch (t)
            {
            case '3': // trigger trap
                entities.push_back(new TriggerTrap(pos));
                break;
            case '4': // fixed trap
                entities.push_back(new FixedTrap(pos));
                break;
            case '5': // gun trap (placeholder uses TriggerTrap for now)
                entities.push_back(new TriggerTrap(pos));
                break;
            case '6': // ghost, decide orientation by neighbors
            {
                bool horiz = false;
                if (level.GetTileAt(x-1,y) == '0' || level.GetTileAt(x+1,y) == '0') horiz = true;
                entities.push_back(new Ghost(pos, !horiz ? true : false));
            }
                break;
            case '7':
            {
                bool horiz = false;
                if (level.GetTileAt(x-1,y) == '0' || level.GetTileAt(x+1,y) == '0') horiz = true;
                entities.push_back(new GhostPlus(pos, !horiz ? true : false));
            }
                break;
            default:
                break;
            }
        }
    }
}

void Scene::Update(float dt)
{
    // Provide world bounds to player (from level if loaded)
    Rectangle world = level.GetWorldBounds();
    player.Update(dt, world);
    camera.Update(player.GetCenter());
    // Update entities
    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (entities[i]) entities[i]->Update(dt, player, entities, level);
    }
    // remove inactive
    for (size_t i = 0; i < entities.size(); )
    {
        if (!entities[i] || !entities[i]->IsActive())
        {
            delete entities[i];
            entities.erase(entities.begin() + i);
        }
        else ++i;
    }

    UpdateMusicStream(music[0]);
}

void Scene::DrawWorld()
{
    camera.BeginWorld();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    level.Draw();
    // draw entities
    for (auto e : entities) if (e && e->IsActive()) e->Draw();
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
    // cleanup entities
    for (auto e : entities) { if (e) delete e; }
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
