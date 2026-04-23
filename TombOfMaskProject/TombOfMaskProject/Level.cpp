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
    ghostLoaded = ghostPlusLoaded = false;
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
    if (FileExists("resources/sprites/Ghost.png"))
    {
        ghostTex = LoadTexture("resources/sprites/Ghost.png");
        ghostLoaded = true;
    }
    if (FileExists("resources/sprites/GhostPlus.png"))
    {
        ghostPlusTex = LoadTexture("resources/sprites/GhostPlus.png");
        ghostPlusLoaded = true;
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
    if (ghostLoaded) UnloadTexture(ghostTex);
    if (ghostPlusLoaded) UnloadTexture(ghostPlusTex);
}

bool Level::Load(int levelNumber)
{
    rows.clear();
    // Try preferred file locations: Level/{N}.txt then {N}.txt
    std::ostringstream p1; p1 << "Level/" << levelNumber << ".txt";
    std::ostringstream p3; p3 << levelNumber << ".txt";

    if (FileExists(p1.str().c_str())) return LoadFromFile(p1.str().c_str());
    if (FileExists(p3.str().c_str())) return LoadFromFile(p3.str().c_str());
    return false;
}

bool Level::LoadFromFile(const char* filename)
{
    rows.clear();

    std::ifstream f(filename);
    if (!f.is_open()) return false;

    std::string line;
    std::vector<std::string> allLines;
    while (std::getline(f, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // skip empty lines
        if (line.empty()) continue;
        allLines.push_back(line);
    }
    if (allLines.empty()) return false;

    // Detect if file is header-based (first line contains comma) or plain grid
    bool header = (allLines[0].find(',') != std::string::npos);
    if (header)
    {
        // reuse previous parsing logic expecting at least 3 header lines
        std::istringstream ss0(allLines[0]);
        int w=0,h=0; char comma;
        ss0 >> w >> comma >> h;
        if (w<=0 || h<=0) return false;
        width = w; height = h;

        // parse start
        std::istringstream ss1(allLines[1]); int sx=0, sy=0; ss1 >> sx >> comma >> sy;
        startPos = { (float)sx * tileSize, (float)sy * tileSize };
        // parse goal
        std::istringstream ss2(allLines[2]); int gx=0, gy=0; ss2 >> gx >> comma >> gy;
        goalPos = { (float)gx * tileSize, (float)gy * tileSize };

        // remaining lines are rows
        rows.clear();
        for (size_t i=3;i<allLines.size();++i) rows.push_back(allLines[i]);
        // ensure height consistency
        height = (int)rows.size();
        width = rows.empty()?0:(int)rows[0].size();
    }
    else
    {
        // plain grid: each line is a row; determine width/height and find start/goal
        rows = allLines;
        height = (int)rows.size();
        width = 0;
        for (const auto &r : rows) if ((int)r.size() > width) width = (int)r.size();

        // find 'x' and 'f'
        bool foundStart=false, foundGoal=false;
        for (int y=0;y<height;++y)
        {
            for (int x=0;x<(int)rows[y].size();++x)
            {
                char c = rows[y][x];
                if (c == 'x' && !foundStart)
                {
                    startPos = { (float)x * tileSize, (float)y * tileSize };
                    foundStart = true;
                }
                if (c == 'f' && !foundGoal)
                {
                    goalPos = { (float)x * tileSize, (float)y * tileSize };
                    foundGoal = true;
                }
            }
        }
    }

    return true;
}

char Level::GetTileAt(int tx, int ty) const
{
    if (ty < 0 || ty >= (int)rows.size()) return (char)Empty;
    const std::string &row = rows[ty];
    if (tx < 0 || tx >= (int)row.size()) return (char)Empty;
    return row[tx];
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
                if (spikeLoaded)
                {
                    // draw first 32x32 frame from spritesheet
                    DrawTexturePro(spikeTex, Rectangle{0,0,32.0f,32.0f}, dest, Vector2{0,0}, 0, WHITE);
                }
                else DrawRectangleRec(dest, RED);
                break;
            case TrapSpike:
                if (trapLoaded) DrawTexturePro(trapSpikeTex, Rectangle{0,0,32.0f,32.0f}, dest, Vector2{0,0}, 0, WHITE);
                else DrawRectangleRec(dest, ORANGE);
                break;
            case GunTrapTile:
                if (gunLoaded) DrawTexturePro(gunTrapTex, Rectangle{0,0,32.0f,32.0f}, dest, Vector2{0,0}, 0, WHITE);
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

