#include "Level.h"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <sstream>

// Tileset slot convention (0-based local ID = GID - firstGID).
// Tell your teammate to arrange the tileset image in this order:
//   slot 0  ->  Wall1        ('1')
//   slot 1  ->  Wall2        ('2')
//   slot 2  ->  FixedSpike   ('S')
//   slot 3  ->  TriggerSpike ('T')
//   slot 4  ->  GunTrap      ('G')
//   slot 5  ->  Ghost        ('6')
//   slot 6  ->  GhostPlus    ('7')
//   slot 7  ->  Star         ('s')
//   slot 8  ->  Exit         ('f')
//   slot 9  ->  PlayerStart  ('x')
//   GID 0 (empty cell) is always treated as empty ('-').
static char LocalIDToChar(uint32_t localID)
{
    switch (localID)
    {
    case 0:  return '1';
    case 1:  return '2';
    case 2:  return 'S';
    case 3:  return 'T';
    case 4:  return 'G';
    case 5:  return '6';
    case 6:  return '7';
    case 7:  return 's';
    case 8:  return 'f';
    case 9:  return 'x';
    default: return '-';
    }
}

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
    if (spikeLoaded)  UnloadTexture(spikeTex);
    if (trapLoaded)   UnloadTexture(trapSpikeTex);
    if (gunLoaded)    UnloadTexture(gunTrapTex);
    if (wallLoaded[0]) UnloadTexture(wallTex[0]);
    if (wallLoaded[1]) UnloadTexture(wallTex[1]);
    if (endLoaded)    UnloadTexture(endTex);
    if (starLoaded)   UnloadTexture(starTex);
}

bool Level::Load(int levelNumber)
{
    rows.clear();
    std::ostringstream path;
    path << "Level/" << levelNumber << ".tmx";
    if (FileExists(path.str().c_str()))
        return LoadFromFile(path.str().c_str());
    return false;
}

bool Level::LoadFromFile(const char* filename)
{
    rows.clear();
    startPos = { 0, 0 };
    goalPos  = { 0, 0 };

    tmx::Map map;
    if (!map.load(filename)) return false;

    const auto& tileCount   = map.getTileCount();
    const auto& tmxTileSize = map.getTileSize();
    width    = (int)tileCount.x;
    height   = (int)tileCount.y;
    tileSize = (int)tmxTileSize.x;

    // Find the first tile layer
    const tmx::TileLayer* tileLayer = nullptr;
    for (const auto& layer : map.getLayers())
    {
        if (layer->getType() == tmx::Layer::Type::Tile)
        {
            tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            break;
        }
    }
    if (!tileLayer) return false;

    const auto& tiles = tileLayer->getTiles();
    uint32_t firstGid = map.getTilesets().empty() ? 1u : map.getTilesets()[0].getFirstGID();

    rows.assign(height, std::string(width, '-'));
    bool foundStart = false, foundGoal = false;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint32_t gid = tiles[y * width + x].ID;
            if (gid == 0) continue;

            char ch = LocalIDToChar(gid - firstGid);
            rows[y][x] = ch;

            if (ch == 'x' && !foundStart)
            {
                startPos = { (float)(x * tileSize), (float)(y * tileSize) };
                foundStart = true;
            }
            if (ch == 'f' && !foundGoal)
            {
                goalPos = { (float)(x * tileSize), (float)(y * tileSize) };
                foundGoal = true;
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
                else
                    DrawRectangleRec(dest, RED);
                break;
            case TrapSpike:
                if (trapLoaded)
                    DrawTexturePro(trapSpikeTex, Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, ORANGE);
                break;
            case GunTrapTile:
                if (gunLoaded)
                    DrawTexturePro(gunTrapTex, Rectangle{ 0,0,32.0f,32.0f }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, PURPLE);
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
Vector2 Level::GetGoalPosition()  const { return goalPos;  }
Rectangle Level::GetWorldBounds() const { return Rectangle{ 0.0f, 0.0f, (float)width * tileSize, (float)height * tileSize }; }
