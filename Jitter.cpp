#include "toad.h"

void toad::jitter::do_jitter()
{
    if (toad::random_int(0, 100) < toad::jitter::chance) {
        POINT pt;
        GetCursorPos(&pt);
        
        pt.x += toad::random_int(-toad::jitter::intensity_X, toad::jitter::intensity_X);
        pt.y += toad::random_int(-toad::jitter::intensity_Y, toad::jitter::intensity_Y);

        SetCursorPos(pt.x, pt.y);
    }
}
