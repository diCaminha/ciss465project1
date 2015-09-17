//This code was based in a code found on the website: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

#include "SDL2/SDL.h"
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* myWindow = NULL;
	
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* spaceShipSurface = NULL;

SDL_Surface* getSurfaceImageBy( std::string path );

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
        myWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( myWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n",
                    SDL_GetError() );
            success = false;
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( myWindow );
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
    SDL_DestroyWindow( myWindow );
    myWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load splash image
    spaceShipSurface = getSurfaceImageBy("spaceship.bmp" );
    
    return success;
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
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Apply the image of the spaceship on the screen
            SDL_BlitSurface( spaceShipSurface, NULL, screenSurface, NULL );

            //Update the surface
            SDL_UpdateWindowSurface( myWindow );

            //Wait two seconds
            //SDL_Delay( 10000 );
        }
    }

     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
