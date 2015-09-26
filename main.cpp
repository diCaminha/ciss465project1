//This code was based in a code found on the website: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

#include "SDL2/SDL.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* window = NULL;
	
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

SDL_Surface* getSurfaceImageBy( std::string path );
bool loadMedia();

// set game speed devider (which means the lower the number the faster
// everything will move
const int GAME_SPEED = 5;

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
        rect.w = BLOCK_WIDTH;
        rect.h = BLOCK_WIDTH;        
    }
    
    SDL_Surface* surface;
    SDL_Rect rect;
    int xVel;
    int yVel;
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

const int MAZE_SPEED = 1;

//The image we will load and show on the screen
SDL_Surface* spaceShipSurface = NULL;


bool init()
{
    
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        window = SDL_CreateWindow( "It's a beautiful night in the city",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n",
                    SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );
        }
    }
    return success;
}

void close()
{
    //Deallocate surface
    SDL_FreeSurface( spaceShipSurface );
    spaceShipSurface = NULL;

    //Destroy window
    SDL_DestroyWindow( window );
    window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

/*
bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load splash image
    spaceShipSurface = getSurfaceImageBy("spaceship.bmp" );
    
    return success;
}
*/

//Function that load surfaces by the path of the image in the parameter
SDL_Surface* getSurfaceImageBy( std::string path )
{
    
   
    //getting a surface from a specific path, passed by parameter
    SDL_Surface* surface = SDL_LoadBMP( path.c_str());
    
    return surface;
}

int main( int argc, char* args[] )
{               
    srand((unsigned int) time(NULL));

    const Uint8 *keys = SDL_GetKeyboardState(NULL); 
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    /*
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
    }
    */

     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    // create the spaceship object
    Spaceship spaceship;

    // set how many peices the maze will have
    const int MAZE_WIDTH = 23;
    const int MAZE_HEIGHT = 16;
    int mazeRightEdge;
    // create the maze array
    Maze maze[MAZE_WIDTH][MAZE_HEIGHT];
    // position the maze array
    for (int i = 0; i < MAZE_WIDTH; ++i)
    {
        for (int j = 0; j < MAZE_HEIGHT; ++j)
        {            
            maze[i][j].rect.x = i * BLOCK_WIDTH;
            maze[i][j].rect.y = j * BLOCK_WIDTH;
        }
    }

    // cut out some of the maze temporarily just so we can see it    
    for (int i = 0; i < MAZE_WIDTH; ++i)
    {
        for (int j = 6; j < 10; ++j)
        {
            maze[i][j].exist = false;
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 3; j < 6; ++j)
        {
            maze[i][j].exist = false;
        }
    }

    
    //While application is running
    while( !quit )
    {
        // 1 INPUT ------------------------------------------------------------
        
        //Uint32 totalTime, timeSinceLastLoop, oldTotalTime;
        int totalTime, timeSinceLastLoop, oldTotalTime;
        totalTime = SDL_GetTicks();
        timeSinceLastLoop = totalTime - oldTotalTime;            
        oldTotalTime = totalTime;
        //std::cout << timeSinceLastLoop << std::endl;
        
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
          
          }  
                   
                
                         
                if (keys[SDL_SCANCODE_LEFT]){
                    spaceship.xVel = -1;
                    spaceship.rect.x += spaceship.xVel
                        * timeSinceLastLoop / GAME_SPEED;
                }
                if (keys[SDL_SCANCODE_RIGHT]){

                  spaceship.xVel = 1;
                    spaceship.rect.x += spaceship.xVel
                        * timeSinceLastLoop / GAME_SPEED;
                }
                if (keys[SDL_SCANCODE_DOWN]){
                    spaceship.yVel = 1;
                     spaceship.rect.y += spaceship.yVel
                         * timeSinceLastLoop / GAME_SPEED;
    
                }
                if (keys[SDL_SCANCODE_UP]){ 
               
                    spaceship.yVel = -1;
                    spaceship.rect.y += spaceship.yVel
                        * timeSinceLastLoop / GAME_SPEED;

                }
                 // update spaceship
        
        
            
         
    

        // 2 UPDATE -----------------------------------------------------------
       
        // move maze
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            for (int j = 0; j < MAZE_HEIGHT; ++j)
            {
                maze[i][j].rect.x -= std::abs(spaceship.xVel)
                        * timeSinceLastLoop / GAME_SPEED;
            }
        }
        
        // determine maze right edge
        mazeRightEdge = maze[0][0].rect.x;
        for (int i = 1; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x + BLOCK_WIDTH > mazeRightEdge)
                mazeRightEdge = maze[i][0].rect.x + BLOCK_WIDTH;
        }
        
        // generate maze
        //std::cout << rand() % 3 << std::endl;

        // if maze touches wall        
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x < 0)
            {
                for (int j = 0; j < MAZE_HEIGHT; ++j)
                {
                    maze[i][j].rect.x = mazeRightEdge;
                    //maze[0][0].rect.w * (MAZE_WIDTH - 2);
                }
            }
        }                

        // 3 DRAW -------------------------------------------------------------
        
        SDL_FillRect(screenSurface, NULL, 0x000000);

        // draw maze
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            for (int j = 0; j < MAZE_HEIGHT; ++j)
            {
                if (maze[i][j].exist)
                {
                    SDL_BlitSurface( maze[i][j].surface, NULL, screenSurface,
                                     &maze[i][j].rect );
                }
            }
        }

        
        // draw spaceship
        SDL_BlitSurface( spaceship.surface, NULL, screenSurface,
                         &spaceship.rect );
        
        //Update the surface
        SDL_UpdateWindowSurface( window );
    }
     
    //Free resources and close SDL
    close();

    return 0;
}
