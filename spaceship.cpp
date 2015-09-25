
// spaceship class
// contains an surface which is initialized in the contructor by calling
// getSurfaceImageBy
// contains a rect, with x, y, w, h (I didn't bother with looking into
// calling the super constructor)

// contains xVel, yVel to make movement smoother

#include "spaceship.h"

class Spaceship    
{
public:
    Spaceship()
        : surface(getSurfaceImageBy("spaceship.bmp")),
          xVel(0), yVel(0)
    {
        rect.x = 305;
        rect.y = 225;
        rect.w = 30;
        rect.h = 30;        
    }
    
    SDL_Surface* surface;
    SDL_Rect rect;
    int xVel;
    int yVel;
};