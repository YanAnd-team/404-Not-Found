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
    if (FileExists("resources/sprites/Stars and coins/Star.png"))
    {
        starTex = LoadTexture("resources/sprites/Stars and coins/Star.png");
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
            // Use the layer's actual cell size (4px) for the wall collision grid
            wallCellSize  = layer.getCellSize();
            wallGridWidth  = layer.getGridSize().x;
            wallGridHeight = layer.getGridSize().y;
            wallRows.assign(wallGridHeight, std::string(wallGridWidth, '-'));

            for (int y = 0; y < wallGridHeight; ++y)
                for (int x = 0; x < wallGridWidth; ++x)
                    if (layer.getIntGridVal(x, y).value == 1)
                        wallRows[y][x] = '1';
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
                    rows[gy][gx] = 'f';
                }
                else if (name == "Star")       { rows[gy][gx] = 's'; }
                else if (name == "Coin")       { rows[gy][gx] = 'c'; }
                else if (name == "Coin1")      { rows[gy][gx] = 'k'; }
                // "Step" is a floor decoration tile, not a trap - no entity needed
                else if (name == "Sharp1")     { rows[gy][gx] = 'S'; }
                else if (name == "Sharp2")     { rows[gy][gx] = '2'; }
                else if (name == "Sharp4")     { rows[gy][gx] = '4'; }
                else if (name == "Sharp6")     { rows[gy][gx] = 'Y'; }
                else if (name == "Monster1")   { rows[gy][gx] = '6'; }
                else if (name == "Monster2")   { rows[gy][gx] = '7'; }
                else if (name == "Arrow_trap") { rows[gy][gx] = 'G'; }
                else if (name == "Arrow_trap1"){ rows[gy][gx] = 'g'; }
                else if (name == "Arrow_trap2"){ rows[gy][gx] = 'u'; }
                else if (name == "Arrow_trap3"){ rows[gy][gx] = 'd'; }
                else if (name == "Ice_box")    { rows[gy][gx] = 'i'; }
            }
        }
    }

    std::string pngPath = "Level/level/png/Level_" + std::to_string(levelNumber - 1) + "_Tiles" + std::to_string(levelNumber) + ".png";
    if (FileExists(pngPath.c_str()))
    {
        m_levelTex = LoadTexture(pngPath.c_str());
        m_levelTexLoaded = true;
    }

    return true;
}

char Level::GetTileAt(int tileX, int tileY) const
{
    if (tileY < 0 || tileY >= (int)rows.size()) return '-';
    const std::string& row = rows[tileY];
    if (tileX < 0 || tileX >= (int)row.size()) return '-';
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
            if      (t == '1') DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, GRAY);
            else if (t == '2') DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, DARKGRAY);
        }
}

bool Level::IsWall(float worldX, float worldY) const
{
    if (wallCellSize <= 0 || wallRows.empty()) return false;
    int cx = (int)(worldX / wallCellSize);
    int cy = (int)(worldY / wallCellSize);
    if (cy < 0 || cy >= (int)wallRows.size())    return false;
    if (cx < 0 || cx >= (int)wallRows[cy].size()) return false;
    return wallRows[cy][cx] == '1';
}

Vector2 Level::GetStartPosition() const { return startPos; }
Vector2 Level::GetGoalPosition()  const { return goalPos;  }
Rectangle Level::GetWorldBounds() const { return Rectangle{ 0.0f, 0.0f, (float)width * tileSize, (float)height * tileSize }; }

void Level::SetTileAt(int x, int y, char v)
{
    if (y >= 0 && y < (int)rows.size() && x >= 0 && x < (int)rows[y].size())
        rows[y][x] = v;
}
