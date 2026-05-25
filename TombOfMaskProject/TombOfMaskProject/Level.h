#pragma once
#include "raylib.h"
#include <unordered_map>
#include <vector>
#include <LDtkLoader/Project.hpp>

enum Map { Empty='-', Wall1='1', Wall2='2', Spike='S', TrapSpike='T', GunTrapTile='G', End='f', Star='s' };

struct TileDrawCmd {
    int tilesetUid;
    Rectangle src;
    Rectangle dst;
    bool flipX;
    bool flipY;
};

class Level
{
public:
    Level();
    void Init();     //Load all tile textures and the LDtk project file
    void DeInit();   //Unload all tile textures

    bool Load(int levelNumber);   //Parse level N (1-indexed) from Level/Levels.ldtk

    void Draw() const;   //Draw all tiles using their corresponding textures

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

    Texture2D spikeTex;
    Texture2D trapSpikeTex;
    Texture2D gunTrapTex;
    Texture2D wallTex[2];
    Texture2D endTex;
    Texture2D starTex;

    bool spikeLoaded;
    bool trapLoaded;
    bool gunLoaded;
    bool wallLoaded[2];
    bool endLoaded;
    bool starLoaded;

    std::vector<TileDrawCmd> m_tileCmds;
    std::unordered_map<int, Texture2D> m_tilesetTextures;
};
