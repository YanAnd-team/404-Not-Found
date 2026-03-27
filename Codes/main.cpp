#include "Game.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Game game;

    game.Init();

    while (!game.ShouldClose())
    {
        game.Update();
        game.Draw();
    }

    game.DeInit();

    return 0;
}
