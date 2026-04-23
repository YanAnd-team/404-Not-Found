#include "Level.h"
#include <fstream>
#include <sstream>

Level::Level()
{
    width = height = 0;
    tileSize = 32;
    startPos = { 0, 0 };
    goalPos = { 0, 0 };
    spikeLoaded = trapLoaded = gunLoaded = false;
    wallLoaded[0] = wallLoaded[1] = false;
}

void Level::Init()
{
    // load common textures if present (optional)
    if (FileExists("resources/sprites/Spike.png"))
    {
        spikeTex = LoadTexture("resources/sprites/Spike.png");
        spikeLoaded = true;
    }
    if (FileExists("resources/sprites/SpikeTrap.png"))
    {
        trapSpikeTex = LoadTexture("resources/sprites/SpikeTrap.png");
        trapLoaded = true;
    }
    if (FileExists("resources/sprites/GunTrap.png"))
    {
        gunTrapTex = LoadTexture("resources/sprites/GunTrap.png");
        gunLoaded = true;
    }
	if (FileExists("resources/sprites/Wall1.png"))
    {
        wallTex[0] = LoadTexture("resources/sprites/Wall1.png");
        wallLoaded[0] = true;
    }
    if (FileExists("resources/sprites/Wall2.png"))
    {
        wallTex[1] = LoadTexture("resources/sprites/Wall2.png");
        wallLoaded[1] = true;
    }
}

void Level::Update()
{
    // placeholder for future updates
}

void Level::DeInit()
{
    if (spikeLoaded) UnloadTexture(spikeTex);
    if (trapLoaded) UnloadTexture(trapSpikeTex);
    if (gunLoaded) UnloadTexture(gunTrapTex);
	if (wallLoaded[0]) UnloadTexture(wallTex[0]);
    if (wallLoaded[1]) UnloadTexture(wallTex[1]);
}

bool Level::Load(int levelNumber)
{
    rows.clear();

    std::ostringstream path;
    path << levelNumber << ".txt";

    std::ifstream f(path.str());
    if (!f.is_open()) return false;

    std::string line;

    // First line: width,height
    if (!std::getline(f, line)) return false;
    {
        int w = 0, h = 0;
        char comma;
        std::istringstream ss(line);
        ss >> w >> comma >> h;
        if (w <= 0 || h <= 0) return false;
        width = w; height = h;
    }

    // Second line: player start x,y
    if (!std::getline(f, line)) return false;
    {
        int sx = 0, sy = 0; char comma;
        std::istringstream ss(line);
        ss >> sx >> comma >> sy;
        startPos = { (float)sx * tileSize, (float)sy * tileSize };
    }

    // Third line: goal x,y
    if (!std::getline(f, line)) return false;
    {
        int gx = 0, gy = 0; char comma;
        std::istringstream ss(line);
        ss >> gx >> comma >> gy;
        goalPos = { (float)gx * tileSize, (float)gy * tileSize };
    }

    // Remaining lines: tile rows (expected count = height)
    for (int r = 0; r < height; ++r)
    {
        if (!std::getline(f, line)) return false;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        rows.push_back(line);
    }

    return true;
}

void Level::Draw() const
{
    for (int y = 0; y < (int)rows.size(); ++y)
    {
        const std::string &row = rows[y];
        for (int x = 0; x < (int)row.size() && x < width; ++x)
        {
            char c = row[x];
            Rectangle dest = { x * tileSize, y * tileSize, (float)tileSize, (float)tileSize };
            switch (c)
            {
            case Wall1:
                DrawRectangleRec(dest, GRAY);
                break;
            case Wall2:
                DrawRectangleRec(dest, DARKGRAY);
                break;
            case Spike:
                if (spikeLoaded) DrawTexturePro(spikeTex, Rectangle{0,0,(float)spikeTex.width,(float)spikeTex.height}, dest, Vector2{0,0}, 0, WHITE);
                else DrawRectangleRec(dest, RED);
                break;
            case TrapSpike:
                if (trapLoaded) DrawTexturePro(trapSpikeTex, Rectangle{0,0,(float)trapSpikeTex.width,(float)trapSpikeTex.height}, dest, Vector2{0,0}, 0, WHITE);
                else DrawRectangleRec(dest, ORANGE);
                break;
            case GunTrap:
                if (gunLoaded) DrawTexturePro(gunTrapTex, Rectangle{0,0,(float)gunTrapTex.width,(float)gunTrapTex.height}, dest, Vector2{0,0}, 0, WHITE);
                else DrawRectangleRec(dest, PURPLE);
                break;
            case Empty:
            default:
                // empty
                break;
            }
        }
    }
}

Vector2 Level::GetStartPosition() const { return startPos; }
Vector2 Level::GetGoalPosition() const { return goalPos; }
Rectangle Level::GetWorldBounds() const { return Rectangle{ 0.0f, 0.0f, (float)width * tileSize, (float)height * tileSize }; }

