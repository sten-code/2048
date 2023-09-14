#include "graphics.h"

void RoundFillRectangle(int x, int y, int width, int height, int radius) {
    gfx_FillCircle(x + radius, y + radius, radius);
    gfx_FillCircle(x + width - radius - 1, y + radius, radius);

    gfx_FillCircle(x + radius, y + height - radius - 1, radius);
    gfx_FillCircle(x + width - radius - 1, y + height - radius - 1, radius);

    gfx_FillRectangle(x + radius, y, width - radius - radius, height);
    gfx_FillRectangle(x, y + radius, width, height - radius - radius);

    // gfx_SetColor(14);
    // gfx_Rectangle(x, y, width, height);
}