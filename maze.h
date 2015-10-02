#include "SDL2/SDL.h"
#include "config.h"

class Maze
{
	
	public:
	    Maze()        
	        : surface(myconfig::getSurfaceImageBy("maze.bmp")), exist(true)
	    {
	        rect.w = BLOCK_WIDTH;
	        rect.h = BLOCK_WIDTH;
	    }
	    
	    SDL_Surface* surface;
	    SDL_Rect rect;
	    bool exist;
};