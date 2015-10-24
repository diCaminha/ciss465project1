//This code was based in a code found on the website: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

#include <SDL_ttf.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include "classes.h"
#include "SDL.h"
#include "SDL_net.h"

using std::cout;
using std::cerr;
using std::endl;



//################### GAME CONFIGUTATIONS ##############################333
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

 // set up key state
const Uint8 *keys = SDL_GetKeyboardState(NULL); 

//The window we'll be rendering to
SDL_Window* window = NULL;
	
//The surface contained by the window
SDL_Surface* screenSurface = NULL;

// set game speed devider (which means the lower the number the faster
// everything will move
const int GAME_SPEED = 3;

int determineMazeRightEdge(Maze maze[MAZE_WIDTH][MAZE_HEIGHT])
{    
    int mazeRightEdge = maze[0][0].rect.x;
    for (int i = 1; i < MAZE_WIDTH; ++i)
    {
        if (maze[i][0].rect.x + BLOCK_WIDTH > mazeRightEdge)
            mazeRightEdge = maze[i][0].rect.x + BLOCK_WIDTH;
    }
    return mazeRightEdge;
}

// check for ship collision with maze
bool shipMazeCollision(Spaceship spaceship,
                       Maze maze[MAZE_WIDTH][MAZE_HEIGHT])
{
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
                
                if ((bottomMaze >= topSpaceship)
                    && (leftMaze <= rightSpaceship)
                    && (topMaze <=  bottomSpaceship)
                    && (rightMaze >= leftSpaceship))
                {
                    //return true;
                }
            }    
        }
    }
    return false;
}

bool init()
{    
    //Initialization flag
    bool success = true;

    // prepare text
    TTF_Init();
    
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
//######################################################################





//############ NETWORKING CONFIGUTATIONS #####################

// A programs communicating via the network must agree on which port they
// will use. The computer uses the port number to know which program network
// messages are being sent to.

const int PORT = 3750;

// The client and server will send each other messages. 1 means "I pressed
// left," 0 means "I pressed right."

const int LEFT = 1;
const int RIGHT = 0;

// Prototypes for server and client functions.

void beTheServer();
void beTheClient(const char servername[]);


//###############################################



int main( int argc, char* argv[] )
{
    
    // Is this program the host, or the client?
    bool operating_as_host;

    // Check the command line arguments.
    if (argc == 1)
    {
        operating_as_host = true;
        cout << "Operating as host: clients connect to me." << endl;
    }
    else if (argc == 2)
    {
        operating_as_host = false;
        cout << "Operating as client, connecting to " << argv[1] << endl;
    }
    else
    {
        cerr << "Too many command line arguments" << endl;
        return 1;
    }    

    // seed random
    srand((unsigned int) time(NULL));

   

    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    
    if (operating_as_host)
        beTheServer();
    else
        beTheClient(argv[1]);
    
    // Shut everything down.
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}
   
void beTheClient(const char servername[])
{
 
    // Connection information
    IPaddress ip;

    //As a test: Trying to send the spaceship x position to the Client
    //but it's not working yet.
    int serverSpaceship_x;
    int serverSpaceship_y;

    // Resolve the argument into an IPaddress
    if (SDLNet_ResolveHost(&ip, servername, PORT) == -1)
    {
        cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
        return;
    }

    // Open a socket to communicate with the server
    TCPsocket sock = SDLNet_TCP_Open(&ip);
    if(!sock)
    {
        cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
        return;
    }

    // Create a "socket set" with the new socket, so that you can check
    // to see if the server has sent anything.

    SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
    if(!set)
        cerr << "SDLNet_AllocSocketSet: " << SDLNet_GetError() << endl;
    else
    {
        if (SDLNet_TCP_AddSocket(set, sock) == -1)
            cerr << "SDLNet_AddSocket: " << SDLNet_GetError() << endl;
    }
    

    int mazeTop = 6;
    int mazeBottom = 10;
    
    bool quit = false;
    
    // Main client loop
    while (!quit)
    {
        SDL_Event event;
        bool send_something = false;
        
        int clientSpaceship_X;
        int clientSpaceship_Y;
        int serverSpaceship_x;
        int serverSpaceship_y;
        int protocal;
        int recieved;
        
        if (send_something)
        {
            int sent = SDLNet_TCP_Send(sock, &clientSpaceship_X, sizeof(clientSpaceship_X));
            if (sent != sizeof(clientSpaceship_X))
                cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
        }

        while (SDLNet_CheckSockets(set, 0))
        {
            int got = SDLNet_TCP_Recv(sock, &recieved, sizeof(serverSpaceship_x));
            if (got <= 0)
            {
                cerr << "Connection problem, quitting..." << endl;
                return;
            }

            if (recieved == -1)
            {
                protocal = recieved;
            }
            else if (recieved == -3)
            {
                protocal = recieved;
            }
            else if (recieved == -5)
            {                
                protocal = recieved;
            }
            else if (recieved == -7)
            {
                protocal = recieved;
            }
            else
            {
                if (protocal == -1)
                {
                    serverSpaceship_x = recieved;
                }
                else if (protocal == -3)
                {
                    serverSpaceship_y = recieved;
                }
                else if (protocal == -5)
                {
                    mazeTop = recieved;
                }
                else if (protocal == -7)
                {
                    mazeBottom = recieved;
                }
            }
            
            cout << "protocal:" << protocal
                 << ", serverSpacehip_x:" << serverSpaceship_x
                 << ", serverSpacehip_y:" << serverSpaceship_y
                 << ", recieved:" << recieved << '\n'
                 << "mazeTop:" << mazeTop
                 << ", mazeBottom:" << mazeBottom
                 << endl;
        }

        // Because our loop doesn't do much, wait a bit before going again,
        // to keep from overwhelming the CPU.
        SDL_Delay(50);
    }

    SDLNet_TCP_Close(sock);
}


void beTheServer()
{
   

    // Connection information
    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, NULL, PORT) == -1)
    {
        cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
        return;
    }

    // Open the socket to listen for connections from the client
    TCPsocket listener = SDLNet_TCP_Open(&ip);
    if(!listener)
    {
        cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
        return;
    }

    // Prepare a SocketSet so we can check for messages from the client
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
    
     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    // create the spaceship object
    Spaceship spaceship;

    // create the gameover object
    GameOverBoard gameover;

    // set up score
    int score = 0;
    int distance = 0;
    int SCORE_RATE = 16;
    Score scoreObj;
    scoreObj.font = TTF_OpenFont( "FreeSansBold.ttf", 28 );

    // maze
    int mazeRightEdge;
    int mazeTop = 6, mazeBottom = 10;
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
        for (int j = mazeTop; j < mazeBottom; ++j)
        {
            maze[i][j].exist = false;
        }
    }

    // variables that will help us keep track of loop time
    int totalTime, timeSinceLastLoop, oldTotalTime;
    
    TCPsocket client = 0;

    int protocal = -1;

    // Main server loop    
    //While application is running
    while( !quit )
    {
       
        bool send_something = false;
        //As a test: Trying to send the spaceship x position to the Client
        //but it's not working yet.
        int serverSpaceship_x = spaceship.rect.x;
        int serverSpaceship_y = spaceship.rect.y;

        // 1 INPUT ------------------------------------------------------------
        
        // keep track of loop time
        totalTime = SDL_GetTicks();
        timeSinceLastLoop = totalTime - oldTotalTime;            
        oldTotalTime = totalTime;
        
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }          
        }

        // key input with scancode
        if (keys[SDL_SCANCODE_LEFT]){
            spaceship.xVel = -1;
            spaceship.rect.x += spaceship.xVel * timeSinceLastLoop / GAME_SPEED;
            serverSpaceship_x = spaceship.rect.x;
            //protocal = -1;
            //send_something = true;
        }
                
        if (keys[SDL_SCANCODE_RIGHT]){
            spaceship.xVel = 1;
            spaceship.rect.x += spaceship.xVel * timeSinceLastLoop / GAME_SPEED;
            serverSpaceship_x = spaceship.rect.x;
            //protocal = -1;
            //send_something = true;
        }
                
        if (keys[SDL_SCANCODE_DOWN]){
            spaceship.yVel = 1;
            spaceship.rect.y += spaceship.yVel * timeSinceLastLoop / GAME_SPEED;
            serverSpaceship_y = spaceship.rect.y;
            //protocal = -3;
            //send_something = true;
        }
                
        if (keys[SDL_SCANCODE_UP]){ 
            spaceship.yVel = -1;
            spaceship.rect.y += spaceship.yVel * timeSinceLastLoop / GAME_SPEED;
            serverSpaceship_y = spaceship.rect.y;
            //protocal = -3;
            //send_something = true;
        }
        
        // 2 UPDATE -----------------------------------------------------------
       
        // if we are moving sum up the distance and use to determine the score
        if (spaceship.xVel != 0)
        {
            ++distance;
        }
        score = distance / SCORE_RATE;
        
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
        mazeRightEdge = determineMazeRightEdge(maze);

        // declare variables for sending maze info to client
        int mazeChange; // mazeChangeProtocal;
        
        // if maze touches wall
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x < 0)
            {
                // decide if the maze will change
                int randomChange = rand() % 5;
                // keep track of how far the mazes changes
                mazeChange = 0;
                // mazeChangeProtocal = -5 means mazeTop changed
                // mazeChangeProtocal = -7 means mazeBottom changed
                //mazeChangeProtocal = 0;
                // there are 4 dirrections for the maze to change so there is
                // a 4 in 10 chance that it will change every block reset
                switch (randomChange)
                {
                    case 0:
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop > 5)
                            {
                                ++mazeTop;
                                ++mazeChange;
                                protocal = -5;
                            }
                        }
                        break;
                    case 1:
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8
                                && mazeTop > 1)
                                --mazeTop;
                                --mazeChange;
                                protocal = -5;
                        }
                        break;
                    case 2:
                        while (rand() % 2 == 0)
                        {       
                            if (mazeBottom - mazeTop > 5)
                                --mazeBottom;
                                --mazeChange;
                                protocal = -7;
                        }
                        break;
                    case 3:                        
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8
                                && mazeBottom < MAZE_HEIGHT - 1)
                                ++mazeBottom;
                                ++mazeChange;
                                protocal = -7;
                        }
                        break;
                }

                std::cout << "mazeChange:" << mazeChange
                          << ", protocal:" << protocal
                          << ", mazeTop:" << mazeTop
                          << ", mazeBottom:" << mazeBottom
                          << std::endl;

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
                }                
            }
        }

        // check for ship collision with maze
        if (shipMazeCollision(spaceship, maze))
        {            
            // draw game over board
            SDL_BlitSurface( gameover.surface, NULL,
                             screenSurface, &gameover.rect );
            
            //Update the surface
            SDL_UpdateWindowSurface( window );
            
            // wait and then close
            SDL_Delay(3000);
            
            // set quit to true so loop will exit on next iteration
            quit = true;
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

        // draw score        
        SDL_Color score_color = {0, 255, 0};
        scoreObj.surface = TTF_RenderText_Solid(
            scoreObj.font,
            std::to_string(score).c_str(),
            score_color);
        
        SDL_BlitSurface(scoreObj.surface, NULL, screenSurface,
                        &scoreObj.rect);
        
        //Update the surface
        SDL_UpdateWindowSurface( window );
    

        // If the client socket is still NULL, no-one has connected yet.
        // Check to see if someone wants to connect.

        if (client == 0)
        {
            client = SDLNet_TCP_Accept(listener);

            // If it isn't zero any more, the client socket is now connected.
            // Add it to the SocketSet so that we can check it for data later.

            if (client != 0)
                if (SDLNet_TCP_AddSocket(set, client) == -1)
                    cerr << "SDLNet_AddSocket: " << SDLNet_GetError() << endl;
        }

        // If we're connected to a client, we may have data to send, and we
        // should check to see if they've sent any data to us.

        if (client != 0)
        {
            if (protocal == -1)
            {
                int sent = SDLNet_TCP_Send(client, &protocal,
                                           sizeof(protocal));
                if (sent != sizeof(protocal))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -2;
            }
            else if (protocal == -2)
            {
                int sent = SDLNet_TCP_Send(client, &spaceship.rect.x,
                                           sizeof(spaceship.rect.x));
                if (sent != sizeof(spaceship.rect.x))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -3;
            }
            else if (protocal == -3)
            {
                int sent = SDLNet_TCP_Send(client, &protocal,
                                           sizeof(protocal));
                if (sent != sizeof(protocal))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -4;
            }
            else if (protocal == -4)
            {
                int sent = SDLNet_TCP_Send(client, &spaceship.rect.y,
                                           sizeof(spaceship.rect.y));
                if (sent != sizeof(spaceship.rect.y))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -1;
            }
            else if (protocal == -5)
            {
                int sent = SDLNet_TCP_Send(client, &protocal,
                                           sizeof(protocal));
                if (sent != sizeof(protocal))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -6;
            }
            else if (protocal == -6)
            {
                int sent = SDLNet_TCP_Send(client, &mazeTop,
                                           sizeof(mazeTop));
                if (sent != sizeof(mazeTop))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -1;
            }
            else if (protocal == -7)
            {
                int sent = SDLNet_TCP_Send(client, &protocal,
                                           sizeof(protocal));
                if (sent != sizeof(protocal))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -8;
            }
            else if (protocal == -8)
            {
                int sent = SDLNet_TCP_Send(client, &mazeBottom,
                                           sizeof(mazeBottom));
                if (sent != sizeof(mazeBottom))
                    cerr << "SDLNet_TCP_Send: " << SDLNet_GetError()
                         << endl;
                protocal = -1;
            }

            int clientSpaceship_X;
            while (SDLNet_CheckSockets(set, 0))
            {
                int got = SDLNet_TCP_Recv(client, &clientSpaceship_X,
                                          sizeof(clientSpaceship_X));
                if (got <= 0)
                {
                    cerr << "Connection problem, quitting..." << endl;
                    return;
                }
            }
        }
    }

    //Deallocate surfaces    
    SDL_FreeSurface( spaceship.surface );
    spaceship.surface = NULL;
    SDL_FreeSurface( gameover.surface );
    gameover.surface = NULL;
    SDL_FreeSurface( screenSurface );
    screenSurface = NULL;
    for (int i = 0; i < MAZE_WIDTH; ++i)
    {
        for (int j = 0; j < MAZE_HEIGHT; ++j)
        {            
            SDL_FreeSurface(maze[i][j].surface);
            maze[i][j].surface = NULL;
        }
    }
    SDL_FreeSurface(scoreObj.surface);
    scoreObj.surface = NULL;    
     
    //Free resources and close SDL
    close();    

   

        
        // Because our loop doesn't do much, wait a bit before going again,
        // to keep from overwhelming the CPU.
        SDL_Delay(50);
    

    SDLNet_TCP_Close(listener);
    SDLNet_TCP_Close(client);

}

