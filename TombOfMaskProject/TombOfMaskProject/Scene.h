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
    void Init();
    void Update(float dt);
    void DrawWorld();
    void DrawUI();
    void DeInit();

    bool HasPlayerWon() const;
    void LoadLevel(int levelNumber);

    Player player;
    GameCamera camera;
    Level level;
    std::vector<Entity*> entities;

private:
    
    Sound sound[4];
    Music music[2];
    Font font;
	bool soundLoaded[4] = {};
	bool musicLoaded[2] = {};
};
