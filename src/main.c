#define _ALLOW_KEYWORD_MACROS

#include "game.h"

int main(void)
{
    Game game = { 0 };
    Game_Init(&game);
    Game_Loop(&game);
    Game_Shutdown(&game);

    return 0;
}
