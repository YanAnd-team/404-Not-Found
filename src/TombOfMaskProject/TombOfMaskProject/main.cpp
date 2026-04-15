#include "Game00.h"

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
