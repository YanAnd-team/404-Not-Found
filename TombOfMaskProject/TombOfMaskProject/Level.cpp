#include "Level.h"
#include <string>

Level::Level()
{
    width = height = 0;
    tileSize = 16;
    startPos = { 0, 0 };
    goalPos  = { 0, 0 };
    starLoaded = false;
    m_levelTexLoaded = false;
    projectLoaded = false;
}

void Level::Init()
{
    if (FileExists("resources/sprites/Stars and coins/star0.png"))
    {
        starTex = LoadTexture("resources/sprites/Stars and coins/star0.png");
        starLoaded = true;
    }

    if (FileExists("Level/level.ldtk"))
    {
        try
        {
            ldtkProject.loadFromFile("Level/level.ldtk");
            projectLoaded = true;
        }
        catch (...) { projectLoaded = false; }
    }
}

void Level::DeInit()
{
    if (starLoaded)       UnloadTexture(starTex);
    if (m_levelTexLoaded) UnloadTexture(m_levelTex);
}

bool Level::Load(int levelNumber)
{
    if (m_levelTexLoaded)
    {
        UnloadTexture(m_levelTex);
        m_levelTexLoaded = false;
    }

    rows.clear();
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
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                {
                    int val = layer.getIntGridVal(x, y).value;
                    if      (val == 1) rows[y][x] = (char)Wall1;
                    else if (val == 2) rows[y][x] = (char)Wall2;
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

    // PNG files should be exported from LDtk and named level_1.png, level_2.png, etc.
    std::string pngPath = "Level/level_" + std::to_string(levelNumber) + ".png";
    if (FileExists(pngPath.c_str()))
    {
        m_levelTex = LoadTexture(pngPath.c_str());
        m_levelTexLoaded = true;
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
    if (m_levelTexLoaded)
    {
        DrawTexture(m_levelTex, 0, 0, WHITE);
        return;
    }

    // Fallback: draw walls as colored rectangles when PNG is missing
    for (int y = 0; y < (int)rows.size(); ++y)
        for (int x = 0; x < (int)rows[y].size(); ++x)
        {
            char t = rows[y][x];
            if      (t == (char)Wall1) DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, GRAY);
            else if (t == (char)Wall2) DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, DARKGRAY);
        }
}

Vector2 Level::GetStartPosition() const { return startPos; }
Vector2 Level::GetGoalPosition()  const { return goalPos;  }
Rectangle Level::GetWorldBounds() const { return Rectangle{ 0.0f, 0.0f, (float)width * tileSize, (float)height * tileSize }; }
