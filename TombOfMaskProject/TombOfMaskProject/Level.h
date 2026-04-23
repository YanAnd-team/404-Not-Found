#pragma once
#include "raylib.h"
#include <string>
#include <vector>

enum Map { Empty='-', Wall1='1', Wall2='2', Spike='S', TrapSpike='T', GunTrapTile='G', End='f' };

class Level
{
public:
    Level();
    void Init();
    void Update();
    void DeInit();

    // Load level by number (loads file "Level/Level N.txt"). Returns true on success.
    bool Load(int levelNumber);
    // Alternate loader: load by filename inside Level/ folder (e.g. "1.txt").
    bool LoadFromFile(const char* filename);

    // Draw level tiles
    void Draw() const;

    // Query tile char at tile coordinates (tx,ty). Returns '-' if out of bounds.
    char GetTileAt(int tx, int ty) const;

    // Accessors
    Vector2 GetStartPosition() const;
    Vector2 GetGoalPosition() const;
    Rectangle GetWorldBounds() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }

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
    Texture2D endTex;

    bool spikeLoaded;
    bool trapLoaded;
    bool gunLoaded;
    bool wallLoaded[2];
    bool endLoaded;
};
