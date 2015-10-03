//This code was based in a code found on the website: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

#include "SDL.h"
#include <SDL_ttf.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* window = NULL;
	
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

// ttf font
TTF_Font * font = NULL;
SDL_Surface * score_surface;
SDL_Rect scoreRect;

SDL_Surface* getSurfaceImageBy( std::string path );
bool loadMedia();

// set game speed devider (which means the lower the number the faster
// everything will move
const int GAME_SPEED = 3;

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
};

const int MAZE_SPEED = 1;

//The image we will load and show on the screen
SDL_Surface* spaceShipSurface = NULL;


bool init()
{
    
    //Initialization flag
    bool success = true;


    /*
    if (SDL_Init(SDL_INIT_EVERYTHING ) == -1)
    {
        success = false;
        return success;
    }
    */

    TTF_Init();
    font = TTF_OpenFont( "FreeSansBold.ttf", 28 );
    
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

    //TTF_Font * font = NULL;
    //TTF_Font * font = TTF_OpenFont( "FreeMonoBold.ttf", 28 );

    SDL_Texture* mTexture;

    /*
    if (TTF_Init() == -1)
    {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                TTF_GetError() );
        //suces = false;
    }
    */    
    
     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    // create the spaceship object
    Spaceship spaceship;

    GameOverBoard gameover;

    // set how many peices the maze will have
    const int MAZE_WIDTH = 23;
    const int MAZE_HEIGHT = 16;
    int mazeRightEdge;
    int mazeTop = 6, mazeBottom = 10;
    // create the maze array
    Maze maze[MAZE_WIDTH][MAZE_HEIGHT];

    int score = 0;
    int distance = 0;
    int SCORE_RATE = 16;
    Score scoreObj;
    
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
        for (int j = mazeTop; j < mazeBottom; ++j)
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
            spaceship.rect.x += spaceship.xVel * timeSinceLastLoop / GAME_SPEED;
        }
                
        if (keys[SDL_SCANCODE_RIGHT]){
            spaceship.xVel = 1;
            spaceship.rect.x += spaceship.xVel * timeSinceLastLoop / GAME_SPEED;
        }
                
        if (keys[SDL_SCANCODE_DOWN]){
            spaceship.yVel = 1;
            spaceship.rect.y += spaceship.yVel * timeSinceLastLoop / GAME_SPEED;
        }
                
        if (keys[SDL_SCANCODE_UP]){ 
            spaceship.yVel = -1;
            spaceship.rect.y += spaceship.yVel * timeSinceLastLoop / GAME_SPEED;
        }
                 
        

        
            
         
    

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

        // if we are moving sum up the distance and use to determine the score
        if (spaceship.xVel != 0)
        {
            ++distance;
        }
        score = distance / SCORE_RATE;
        
        // determine maze right edge
        mazeRightEdge = maze[0][0].rect.x;
        for (int i = 1; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x + BLOCK_WIDTH > mazeRightEdge)
                mazeRightEdge = maze[i][0].rect.x + BLOCK_WIDTH;
        }

        // if maze touches wall        
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x < 0)
            {
                // decide if the maze will change
                int randomChange = rand() % 5;
                // there are 4 dirrections for the maze to change so there is
                // a 4 in 10 chance that it will change every block reset
                switch (randomChange)
                {
                    case 0:
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop > 5)
                                ++mazeTop;
                        }
                        break;
                    case 1:
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8
                                && mazeTop > 1)
                                --mazeTop;
                        }
                        break;                        
                    case 2:
                        while (rand() % 2 == 0)
                        {       
                            if (mazeBottom - mazeTop > 5)
                                --mazeBottom;
                        }
                        break;
                    case 3:                        
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8
                                && mazeBottom < MAZE_HEIGHT - 1)
                                ++mazeBottom;
                        }
                        break;                        
                }

                // update the maze based on changes in mazeTop/mazeBottom
                for (int j = 0; j < MAZE_HEIGHT; ++j)
                {
                    maze[i][j].rect.x = mazeRightEdge;
                    if (j > mazeTop && j < mazeBottom)
                    {
                        maze[i][j].exist = false;
                    }
                    else
                    {
                        maze[i][j].exist = true;
                    }
                    //maze[0][0].rect.w * (MAZE_WIDTH - 2);
                }
            }
        }

        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            for (int j = 0; j < MAZE_HEIGHT; ++j)
            {
                //if spacechip touch maze[i][j]
                if(maze[i][j].exist){
                    
                    //The sides of the rectangles
                    int leftMaze, leftSpaceship;
                    int rightMaze, rightSpaceship;
                    int topMaze, topSpaceship;
                    int bottomMaze, bottomSpaceship;

                    //Calculate the sides of the maze
                    leftMaze = maze[i][j].rect.x;
                    rightMaze = maze[i][j].rect.x + maze[i][j].rect.w;
                    topMaze = maze[i][j].rect.y;
                    bottomMaze = maze[i][j].rect.y + maze[i][j].rect.h;

                    //Calculate the sides of the spaceship
                    leftSpaceship = spaceship.rect.x;
                    rightSpaceship = spaceship.rect.x + spaceship.rect.w;
                    topSpaceship = spaceship.rect.y;
                    bottomSpaceship = spaceship.rect.y + spaceship.rect.h;


                    if((bottomMaze >= topSpaceship) && (leftMaze <= rightSpaceship) && 
                        (topMaze <=  bottomSpaceship) &&  (rightMaze >= leftSpaceship)){
                           
                           
                            //Free resources and close SDL
                            
                            
                            //Deallocate surface 
                            SDL_FreeSurface( spaceship.surface );
                            spaceship.surface = NULL;

                            // draw game over board
                            SDL_BlitSurface( gameover.surface, NULL, screenSurface,
                                                &gameover.rect );

                            //Update the surface
                            SDL_UpdateWindowSurface( window );

                            SDL_Delay(3000);
                            close();
                    }
                    

                }    
            }

        }

        //++score;
     

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

        // draw score        
        SDL_Color score_color = {0, 255, 0};
        scoreObj.surface = TTF_RenderText_Solid(
            font,
            std::to_string(score).c_str(),
            score_color);
        
        SDL_BlitSurface(scoreObj.surface, NULL, screenSurface,
                        &scoreObj.rect);

        /*
        if (font == NULL)
        {
            std::cout << "error" << std::endl;
            std::cout << TTF_GetError() << std::endl;
        }
        */
        
        //Update the surface
        SDL_UpdateWindowSurface( window );
    }
     
    //Free resources and close SDL
    close();

    return 0;
}
