#include <ti/getcsc.h>
#include <graphx.h>
#include <time.h>
#include <ti/screen.h>
#include <string.h>

#include "defines.h"
#include "game.h"

int main(void)
{
    gfx_Begin();

    // Set the color palette
    gfx_palette[0] = gfx_RGBTo1555(214, 203, 189); // 0
    gfx_palette[1] = gfx_RGBTo1555(231, 227, 214); // 2
    gfx_palette[2] = gfx_RGBTo1555(231, 223, 198); // 4
    gfx_palette[3] = gfx_RGBTo1555(239, 174, 123); // 8
    gfx_palette[4] = gfx_RGBTo1555(247, 146, 99);  // 16
    gfx_palette[5] = gfx_RGBTo1555(247, 125, 97);  // 32
    gfx_palette[6] = gfx_RGBTo1555(247, 97, 66);   // 64

    gfx_palette[7] = gfx_RGBTo1555(239, 203, 121); // 128
    gfx_palette[8] = gfx_RGBTo1555(239, 199, 105); // 256
    gfx_palette[9] = 30506;                        // 512
    gfx_palette[10] = 30471;                       // 1024
    gfx_palette[11] = 30469;                       // 2048
    gfx_palette[12] = 7398;                        // > 2048

    gfx_palette[13] = gfx_RGBTo1555(247, 243, 239); // White Fore
    gfx_palette[14] = gfx_RGBTo1555(115, 109, 99);  // Black Fore
    gfx_palette[15] = gfx_RGBTo1555(182, 170, 158); // Board Back

    srand(time(NULL));
    Game game;
    init_game(&game);
    render_background();
    render(&game);

    int running = 1;
    while (running)
    {
        int keycode = os_GetCSC();
        if (keycode <= 0x04 && keycode >= 0x01)
        {
            move(&game, keycode);
        }
        else if (keycode == sk_Clear)
        {
            running = 0;
        }
    }

    // Exit
    save_game(&game);
    gfx_End();
    return 0;
}
