#ifndef CLASSES
#define CLASSES


SDL_Surface* getSurfaceImageBy( std::string path );
bool loadMedia();

// set the size for all the blocks in the game
const int BLOCK_WIDTH = 30;

// spaceship class
// contains an surface which is initialized in the contructor by calling
// getSurfaceImageBy
// contains a rect, with x, y, w, h (I didn't bother with looking into
// calling the super constructor)
// contains xVel, yVel to make movement smoother
class Spaceship
{
public:
    Spaceship()
        : surface(getSurfaceImageBy("spaceship.bmp")),
          xVel(0), yVel(0)
    {
        rect.x = 305;
        rect.y = 225;
        rect.w = 10;
        rect.h = 10;        
    }
    
    SDL_Surface* surface;
    SDL_Rect rect;
    int xVel;
    int yVel;
};

class GameOverBoard    
{
public:
    GameOverBoard()
        : surface(getSurfaceImageBy("gameOver.bmp"))
    {
        rect.x = 105;
        rect.y = 130;
        rect.w = 100;
        rect.h = 100;        
    }
    
    SDL_Surface* surface;
    SDL_Rect rect;  
};

class Maze
{
public:
    Maze()        
        : surface(getSurfaceImageBy("maze.bmp")), exist(true)
    {
        rect.w = BLOCK_WIDTH;
        rect.h = BLOCK_WIDTH;
    }
    
    SDL_Surface* surface;
    SDL_Rect rect;
    bool exist;
};
const int MAZE_WIDTH = 23;
const int MAZE_HEIGHT = 16;
const int MAZE_SPEED = 1;

class Score
{
public:
    Score()    
    {
        rect.x = 25;
        rect.y = 10;
        rect.w = 50;
        rect.h = 50;
    }
    SDL_Surface* surface;
    SDL_Rect rect;
    TTF_Font * font = NULL;
};

#endif
