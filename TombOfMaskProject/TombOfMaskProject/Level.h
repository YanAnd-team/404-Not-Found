#pragma once
#include "raylib.h"
#include <vector>
#include <LDtkLoader/Project.hpp>

enum Map { Empty='-', Wall1='1', Wall2='2', Spike='S', TrapSpike='T', GunTrapTile='G', End='f', Star='s' };

class Level
{
public:
    Level();
    void Init();     //Load the LDtk project file and star texture
    void DeInit();   //Unload textures

    bool Load(int levelNumber);   //Parse level N (1-indexed) from Level/level.ldtk and load its PNG

    void Draw() const;   //Draw the level PNG; falls back to colored rectangles if PNG is missing

    char GetTileAt(int tileX, int tileY) const;   //Return tile char at grid coords; returns '-' (Empty) if out of bounds

    Vector2 GetStartPosition() const;
    Vector2 GetGoalPosition() const;
    Rectangle GetWorldBounds() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }
    Texture2D GetStarTex() const { return starTex; }
    bool IsStarLoaded() const { return starLoaded; }

private:
    int width;
    int height;
    int tileSize;
    Vector2 startPos;
    Vector2 goalPos;
    std::vector<std::string> rows;

    ldtk::Project ldtkProject;
    bool projectLoaded;

    Texture2D m_levelTex;
    bool m_levelTexLoaded;

    Texture2D starTex;
    bool starLoaded;
};
