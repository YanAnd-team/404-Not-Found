#pragma once
#include "raylib.h"
#include <string>
#include <vector>

enum Map { Empty='-', Wall1='1', Wall2='2', Spike='S', TrapSpike='T', GunTrap='G' };

class Level
{
public:
    Level();
    void Init();
    void Update();
    void DeInit();

    // Load level by number (loads file "Level/Level N.txt"). Returns true on success.
    bool Load(int levelNumber);

    // Draw level tiles
    void Draw() const;

    // Accessors
    Vector2 GetStartPosition() const;
    Vector2 GetGoalPosition() const;
    Rectangle GetWorldBounds() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    int width;
    int height;
    int tileSize; // pixels per tile
    Vector2 startPos; // in pixels
    Vector2 goalPos;  // in pixels
    std::vector<std::string> rows; // raw tile chars per row

    Texture2D spikeTex;
    Texture2D trapSpikeTex;
    Texture2D gunTrapTex;
	Texture2D wallTex[2];

    bool spikeLoaded;
    bool trapLoaded;
    bool gunLoaded;
    bool wallLoaded[2];
};
