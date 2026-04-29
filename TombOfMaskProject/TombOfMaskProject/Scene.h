#pragma once
#include "raylib.h"
#include "Player.h"
#include "Camera.h"
#include "Level.h"
#include "Entities.h"
#include <vector>

class Scene
{
public:
    void Init();                        //Load audio, font, camera, level 1, and spawn all tile entities
    void Update(float dt);              //Update player, camera, entities, and audio state
    void DrawWorld();                   //Draw level, entities, and player through the camera
    void DrawUI();                      //Draw in-game UI overlay (currently unused)
    void DeInit();                      //Unload all assets, entities, audio, and close audio device

    bool HasPlayerWon() const;          //Win when player bounds overlap the goal tile
    void LoadLevel(int levelNumber);    //Destroy all entities, reload level, respawn player and entities
    void ReloadLevel();                 //Reload the current level from scratch (resets stars, entities, and player)

    void UpdateAudio();                 //Stream music updates; must be called every frame
    void OnEnterMenu();                 //Switch music from gameplay track to menu track
    void OnEnterGameplay();             //Switch music from menu track to gameplay track
    void OnPlayerWon();                 //Play win sound effect
    void DrawStarHUD();                 //Draw collected stars in the top-left corner
    void DrawWinStars();                //Draw 3 star slots centered on screen; filled = collected, empty icon = not collected

    Player player;
    GameCamera camera;
    Level level;
    std::vector<Entity*> entities;
    int starCount = 0;

private:
    Sound sound[4];
    Music music[2];
    Font font;
	bool soundLoaded[4] = {};
	bool musicLoaded[2] = {};
    int currentLevelNumber = 1;
    bool wasSliding = false;
    Texture2D starToCompTex;
    bool starToCompLoaded = false;
};
