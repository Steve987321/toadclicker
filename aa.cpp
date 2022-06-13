#include "aa.h"
#include "toad.h"

void aa::thread()
{
    RECT windowRect;
    int width = 0, height = 0;
    POINT middle;

    int scanRegionx = 150;
    int scanRegiony = 150;

    COLORREF pixelCol;
    HDC dc;

    int tolerance = 50;
    int r = 0, g = 138, b = 75;

    while (true)
    {
        if (toad::minecraft_window != NULL && width == 0 && height == 0)
        {
            if (GetWindowRect(toad::minecraft_window, &windowRect))
            {    
                width = windowRect.right - windowRect.left;
                height = windowRect.bottom - windowRect.top;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        else
        {
             std::this_thread::sleep_for(std::chrono::milliseconds(1));
             for (int i = middle.x - scanRegionx; i < middle.x + scanRegionx; i++)
             {
                 for (int j = middle.y - scanRegiony; j < middle.y + scanRegiony; j++)
                 {
                     dc = GetDC(NULL);
                     pixelCol = GetPixel(dc, i, j);
                     if (pixelCol == RGB(r,g,b))
                     {
                         
                     }
                     ReleaseDC(NULL, dc);
                 }
             }
        }
        
       
    }
}

