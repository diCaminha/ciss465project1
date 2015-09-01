//This code was based in a code found on the website: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

#include "SDL2/SDL.h"
#include <stdio.h>

//Screen dimension constants

int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* myWindow = NULL;
	
	//The surface contained by the window
	SDL_Surface* screen = NULL;

	//Initialize SDL
	
	//Create window
	myWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 500, SDL_WINDOW_SHOWN );
	
	//Get window surface
	screen = SDL_GetWindowSurface( myWindow );
	//Fill the surface white
	SDL_FillRect( screen, NULL, SDL_MapRGB( screen->format, 1.0f, 1.0f, 1.0f ) );
	//Update the surface
	SDL_UpdateWindowSurface( myWindow );
	//Wait 4 seconds
	SDL_Delay( 4000 );
	//Destroy window
	SDL_DestroyWindow( myWindow );

	//Quit SDL
	SDL_Quit();

	return 0;
}