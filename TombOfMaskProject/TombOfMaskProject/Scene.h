#pragma once
#include "raylib.h"
#include "Player.h"
#include "Camera.h"
#include "Level.h"
#include "Entities.h"
#include "CoinSystem.h"

class Scene
{
public:
    void InitAudio();
    void InitGameplay();
    void Update(float dt);
    void DrawWorld();
    void DeInit();

    bool HasPlayerWon() const;
    void LoadLevel(int levelNumber);
    void ReloadLevel();
    int  GetCurrentLevelNumber() const;

    void UpdateAudio();
    void OnEnterMenu();
    void OnEnterGameplay();
    void OnPlayerWon();
    void UpdateWinStars(float dt);
    void DrawStarHUD();
    void DrawWinStars();
    void ToggleHitboxes() { showHitboxes = !showHitboxes; }

    CoinSystem& getCoinSystem() { return coinSystem; }

    Player player;
    GameCamera camera;
    Level level;
    std::vector<Entity*> entities;
    int starCount = 0;

private:
    void DrawHitboxes() const;

    Sound sound[7];
    Music music[2];
    bool soundLoaded[7] = {};
    bool musicLoaded[2] = {};
    int currentLevelNumber = 1;
    bool wasSliding = false;
    bool showHitboxes = false;
    int starsRevealed = 0;
    float starRevealTimer = 0.0f;

    CoinSystem coinSystem;
};