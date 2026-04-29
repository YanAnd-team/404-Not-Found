#pragma once
#include "raylib.h"
#include <string>
#include <vector>

enum Map { Empty='-', Wall1='1', Wall2='2', Spike='S', TrapSpike='T', GunTrapTile='G', End='f', Star='s' };

class Level
{
public:
    Level();                                                        //Set default values; textures are not loaded here
    void Init();                                                    //Load all tile textures; missing files are handled gracefully
    void Update();                                                  //Placeholder for future tile logic
    void DeInit();                                                  //Unload all tile textures

    bool Load(int levelNumber);                                     //Try "Level/{N}.txt" then "{N}.txt" as fallback
    bool LoadFromFile(const char* filename);                        //Support two formats: header-based (w,h + start/goal lines) or plain grid (scans for 'x' and 'f')

    void Draw() const;                                              //Draw all tiles using their corresponding textures

    char GetTileAt(int tileX, int tileY) const;                    //Return tile char at grid coords; returns '-' (Empty) if out of bounds

    Vector2 GetStartPosition() const;       //Return player start position in pixels
    Vector2 GetGoalPosition() const;        //Return goal tile position in pixels
    Rectangle GetWorldBounds() const;       //Return the full level area as a rectangle in pixels
    int GetWidth() const { return width; }              //Return level width in tiles
    int GetHeight() const { return height; }            //Return level height in tiles
    int GetTileSize() const { return tileSize; }        //Return tile size in pixels
    Texture2D GetStarTex() const { return starTex; }    //Return the star texture
    bool IsStarLoaded() const { return starLoaded; }    //Return true if the star texture is loaded

private:
    int width;
    int height;
    int tileSize;
    Vector2 startPos;
    Vector2 goalPos;
    std::vector<std::string> rows;

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
};
