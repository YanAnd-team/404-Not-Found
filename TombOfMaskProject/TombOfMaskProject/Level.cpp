#include "Level.h"
#include <fstream>
#include <sstream>

Level::Level()
{
    width = height = 0;
    tileSize = 32;
    startPos = { 0, 0 };
    goalPos = { 0, 0 };
    spikeLoaded = trapLoaded = gunLoaded = endLoaded = starLoaded = false;
    wallLoaded[0] = wallLoaded[1] = false;
}

void Level::Init()
{
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
    if (FileExists("resources/sprites/END.png"))
    {
        endTex = LoadTexture("resources/sprites/END.png");
        endLoaded = true;
    }
    if (FileExists("resources/sprites/Star.png"))
    {
        starTex = LoadTexture("resources/sprites/Star.png");
        starLoaded = true;
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
}

void Level::DeInit()
{
    if (spikeLoaded) UnloadTexture(spikeTex);
    if (trapLoaded) UnloadTexture(trapSpikeTex);
    if (gunLoaded) UnloadTexture(gunTrapTex);
    if (wallLoaded[0]) UnloadTexture(wallTex[0]);
    if (wallLoaded[1]) UnloadTexture(wallTex[1]);
    if (endLoaded) UnloadTexture(endTex);
    if (starLoaded) UnloadTexture(starTex);
}

bool Level::Load(int levelNumber)
{
    rows.clear();
    std::ostringstream primaryPath;  primaryPath  << "Level/" << levelNumber << ".txt";
    std::ostringstream fallbackPath; fallbackPath << levelNumber << ".txt";

    if (FileExists(primaryPath.str().c_str()))  return LoadFromFile(primaryPath.str().c_str());
    if (FileExists(fallbackPath.str().c_str())) return LoadFromFile(fallbackPath.str().c_str());
    return false;
}

bool Level::LoadFromFile(const char* filename)
{
    rows.clear();

    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::vector<std::string> allLines;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        allLines.push_back(line);
    }
    if (allLines.empty()) return false;

    bool hasHeader = (allLines[0].find(',') != std::string::npos);
    if (hasHeader)
    {
        std::istringstream headerStream(allLines[0]);
        int mapWidth = 0, mapHeight = 0; char comma;
        headerStream >> mapWidth >> comma >> mapHeight;
        if (mapWidth <= 0 || mapHeight <= 0) return false;
        width = mapWidth; height = mapHeight;

        std::istringstream startStream(allLines[1]);
        int startTileX = 0, startTileY = 0;
        startStream >> startTileX >> comma >> startTileY;
        startPos = { (float)startTileX * tileSize, (float)startTileY * tileSize };

        std::istringstream goalStream(allLines[2]);
        int goalTileX = 0, goalTileY = 0;
        goalStream >> goalTileX >> comma >> goalTileY;
        goalPos = { (float)goalTileX * tileSize, (float)goalTileY * tileSize };

        rows.clear();
        for (size_t i = 3; i < allLines.size(); ++i) rows.push_back(allLines[i]);
        height = (int)rows.size();
        width  = rows.empty() ? 0 : (int)rows[0].size();
    }
    else
    {
        rows = allLines;
        height = (int)rows.size();
        width = 0;
        for (const auto& row : rows) if ((int)row.size() > width) width = (int)row.size();

        bool foundStart = false, foundGoal = false;
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < (int)rows[y].size(); ++x)
            {
                char tileChar = rows[y][x];
                if (tileChar == 'x' && !foundStart)
                {
                    startPos = { (float)x * tileSize, (float)y * tileSize };
                    foundStart = true;
                }
                if (tileChar == 'f' && !foundGoal)
                {
                    goalPos = { (float)x * tileSize, (float)y * tileSize };
                    foundGoal = true;
                }
            }
        }
    }

    return true;
}

char Level::GetTileAt(int tileX, int tileY) const
{
    if (tileY < 0 || tileY >= (int)rows.size()) return (char)Empty;
    const std::string& row = rows[tileY];
    if (tileX < 0 || tileX >= (int)row.size()) return (char)Empty;
    return row[tileX];
}

void Level::Draw() const
{
    for (int y = 0; y < (int)rows.size(); ++y)
    {
        const std::string& row = rows[y];
        for (int x = 0; x < (int)row.size() && x < width; ++x)
        {
            char tileChar = row[x];
            Rectangle dest = { (float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize };
            switch (tileChar)
            {
            case Wall1:
                if (wallLoaded[0])
                    DrawTexturePro(wallTex[0], Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, GRAY);
                break;
            case Wall2:
                if (wallLoaded[1])
                    DrawTexturePro(wallTex[1], Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, DARKGRAY);
                break;
            case Spike:
                if (spikeLoaded)
                    DrawTexturePro(spikeTex, Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else DrawRectangleRec(dest, RED);
                break;
            case TrapSpike:
                if (trapLoaded) DrawTexturePro(trapSpikeTex, Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else DrawRectangleRec(dest, ORANGE);
                break;
            case GunTrapTile:
                if (gunLoaded) DrawTexturePro(gunTrapTex, Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else DrawRectangleRec(dest, PURPLE);
                break;
            case End:
                if (endLoaded)
                    DrawTexturePro(endTex, Rectangle{ 0,0,(float)endTex.width,(float)endTex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, GOLD);
                break;
            case Empty:
            default:
                break;
            }
        }
    }
}

Vector2 Level::GetStartPosition() const { return startPos; }
Vector2 Level::GetGoalPosition() const { return goalPos; }
Rectangle Level::GetWorldBounds() const { return Rectangle{ 0.0f, 0.0f, (float)width * tileSize, (float)height * tileSize }; }
