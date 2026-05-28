#pragma once
#include "raylib.h"
#include <vector>
#include <LDtkLoader/Project.hpp>


class Level
{
public:
    Level();
    void Init();     //Load the LDtk project file and star texture
    void DeInit();   //Unload textures

    bool Load(int levelNumber);   //Parse level N (1-indexed) from Level/level.ldtk and load its PNG

    void Draw() const;   //Draw the level PNG; falls back to colored rectangles if PNG is missing

    char GetTileAt(int tileX, int tileY) const;    //Return tile char at grid coords; returns '-' if out of bounds
    void SetTileAt(int tileX, int tileY, char v); //Overwrite a tile (used by IceBox)
    bool IsWall(float worldX, float worldY) const; //True if the 4px wall grid has a wall at the given world pixel position

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
    std::vector<std::string> rows;      // 16px entity grid
    std::vector<std::string> wallRows; // 4px collision grid
    int wallCellSize = 4;
    int wallGridWidth  = 0;
    int wallGridHeight = 0;

    ldtk::Project ldtkProject;
    bool projectLoaded;

    Texture2D m_levelTex;
    bool m_levelTexLoaded;

    Texture2D starTex;
    bool starLoaded;
};
