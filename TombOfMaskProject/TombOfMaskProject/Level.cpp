#include "Level.h"
#include <string>

Level::Level()
{
    width = height = 0;
    tileSize = 16;
    startPos = { 0, 0 };
    goalPos  = { 0, 0 };
    spikeLoaded = trapLoaded = gunLoaded = endLoaded = starLoaded = false;
    wallLoaded[0] = wallLoaded[1] = false;
    projectLoaded = false;
}

void Level::Init()
{
    if (FileExists("resources/sprites/Traps/Spike/Spike-down.png"))
    {
        spikeTex = LoadTexture("resources/sprites/Traps/Spike/Spike-down.png");
        spikeLoaded = true;
    }
    if (FileExists("resources/sprites/Traps/Spike-trap/Spike-trap-square.png"))
    {
        trapSpikeTex = LoadTexture("resources/sprites/Traps/Spike-trap/Spike-trap-square.png");
        trapLoaded = true;
    }
    if (FileExists("resources/sprites/Traps/Dart Trap/Dart Trap right stop.png"))
    {
        gunTrapTex = LoadTexture("resources/sprites/Traps/Dart Trap/Dart Trap right stop.png");
        gunLoaded = true;
    }
    if (FileExists("resources/sprites/Exit/exit0.png"))
    {
        endTex = LoadTexture("resources/sprites/Exit/exit0.png");
        endLoaded = true;
    }
    if (FileExists("resources/sprites/Stars and coins/star0.png"))
    {
        starTex = LoadTexture("resources/sprites/Stars and coins/star0.png");
        starLoaded = true;
    }
    if (FileExists("resources/sprites/Wall/Wall-full.png"))
    {
        wallTex[0] = LoadTexture("resources/sprites/Wall/Wall-full.png");
        wallLoaded[0] = true;
    }
    if (FileExists("resources/sprites/Wall/wall2.1.png"))
    {
        wallTex[1] = LoadTexture("resources/sprites/Wall/wall2.1.png");
        wallLoaded[1] = true;
    }

    if (FileExists("Level/Levels.ldtk"))
    {
        try
        {
            ldtkProject.loadFromFile("Level/Levels.ldtk");
            projectLoaded = true;
        }
        catch (...) { projectLoaded = false; }
    }
}

void Level::Update()
{
}

void Level::DeInit()
{
    if (spikeLoaded)   UnloadTexture(spikeTex);
    if (trapLoaded)    UnloadTexture(trapSpikeTex);
    if (gunLoaded)     UnloadTexture(gunTrapTex);
    if (wallLoaded[0]) UnloadTexture(wallTex[0]);
    if (wallLoaded[1]) UnloadTexture(wallTex[1]);
    if (endLoaded)     UnloadTexture(endTex);
    if (starLoaded)    UnloadTexture(starTex);

    for (auto& [uid, tex] : m_tilesetTextures)
        UnloadTexture(tex);
    m_tilesetTextures.clear();
}

bool Level::Load(int levelNumber)
{
    rows.clear();
    m_tileCmds.clear();
    startPos = { 0, 0 };
    goalPos  = { 0, 0 };

    if (!projectLoaded) return false;

    const auto& world  = ldtkProject.getWorld();
    const auto& levels = world.allLevels();

    if (levelNumber < 1 || levelNumber > (int)levels.size()) return false;

    const ldtk::Level& ldtkLevel = levels[levelNumber - 1];

    tileSize = 16;
    width  = (ldtkLevel.size.x + tileSize - 1) / tileSize;
    height = (ldtkLevel.size.y + tileSize - 1) / tileSize;

    rows.assign(height, std::string(width, '-'));

    for (const auto& layer : ldtkLevel.allLayers())
    {
        if (layer.getType() == ldtk::LayerType::IntGrid)
        {
            // Wall collision data — populated once an IntGrid layer is added in LDtk
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                {
                    int val = layer.getIntGridVal(x, y).value;
                    if      (val == 1) rows[y][x] = (char)Wall1;
                    else if (val == 2) rows[y][x] = (char)Wall2;
                }
        }
        else if (layer.getType() == ldtk::LayerType::Tiles)
        {
            if (!layer.hasTileset()) continue;
            const ldtk::Tileset& tileset = layer.getTileset();

            if (m_tilesetTextures.find(tileset.uid) == m_tilesetTextures.end())
            {
                std::string texPath = "Level/" + tileset.path;
                if (FileExists(texPath.c_str()))
                    m_tilesetTextures[tileset.uid] = LoadTexture(texPath.c_str());
            }

            for (const auto& tile : layer.allTiles())
            {
                auto pos = tile.getPosition();
                auto tr  = tile.getTextureRect();
                TileDrawCmd cmd;
                cmd.tilesetUid = tileset.uid;
                cmd.src = { (float)tr.x, (float)tr.y, (float)tr.width, (float)tr.height };
                cmd.dst = { (float)pos.x, (float)pos.y, (float)tr.width, (float)tr.height };
                cmd.flipX = tile.flipX;
                cmd.flipY = tile.flipY;
                m_tileCmds.push_back(cmd);
            }
        }
        else if (layer.getType() == ldtk::LayerType::Entities)
        {
            for (const auto& entity : layer.allEntities())
            {
                int gx = entity.getGridPosition().x;
                int gy = entity.getGridPosition().y;
                if (gx < 0 || gx >= width || gy < 0 || gy >= height) continue;

                const std::string& name = entity.getName();

                if (name == "Start")
                {
                    startPos     = { (float)entity.getPosition().x, (float)entity.getPosition().y };
                    rows[gy][gx] = 'x';
                }
                else if (name == "Final")
                {
                    goalPos      = { (float)entity.getPosition().x, (float)entity.getPosition().y };
                    rows[gy][gx] = (char)End;
                }
                else if (name == "Star")                              { rows[gy][gx] = (char)Star;        }
                else if (name == "Step")                              { rows[gy][gx] = (char)TrapSpike;   }
                else if (name == "Sharp1" || name == "Sharp2")        { rows[gy][gx] = (char)Spike;       }
                else if (name == "Monster1")                          { rows[gy][gx] = '6';               }
                else if (name == "Monster2")                          { rows[gy][gx] = '7';               }
                else if (name == "Arrow_trap")                        { rows[gy][gx] = (char)GunTrapTile; }
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
    for (const auto& cmd : m_tileCmds)
    {
        auto it = m_tilesetTextures.find(cmd.tilesetUid);
        if (it == m_tilesetTextures.end()) continue;
        Rectangle src = cmd.src;
        if (cmd.flipX) src.width  = -src.width;
        if (cmd.flipY) src.height = -src.height;
        DrawTexturePro(it->second, src, cmd.dst, Vector2{ 0, 0 }, 0.0f, WHITE);
    }

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
                    DrawTexturePro(wallTex[0], Rectangle{ 0,0,(float)wallTex[0].width,(float)wallTex[0].height }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, GRAY);
                break;
            case Wall2:
                if (wallLoaded[1])
                    DrawTexturePro(wallTex[1], Rectangle{ 0,0,(float)wallTex[1].width,(float)wallTex[1].height }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, DARKGRAY);
                break;
            case Spike:
                if (spikeLoaded)
                    DrawTexturePro(spikeTex, Rectangle{ 0,0,(float)spikeTex.width,(float)spikeTex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, RED);
                break;
            case TrapSpike:
                if (trapLoaded)
                    DrawTexturePro(trapSpikeTex, Rectangle{ 0,0,(float)trapSpikeTex.width,(float)trapSpikeTex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
                else
                    DrawRectangleRec(dest, ORANGE);
                break;
            case GunTrapTile:
                if (gunLoaded)
                    DrawTexturePro(gunTrapTex, Rectangle{ 0,0,(float)gunTrapTex.width,(float)gunTrapTex.height }, dest, Vector2{ 0,0 }, 0, WHITE);
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
