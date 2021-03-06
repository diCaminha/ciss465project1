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

//################### GAME CONFIGUTATIONS ##############################
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum Protocal { SPACESHIP_POS, MAZE, DONT_SEND_MAZE, SPACESHIP_COLLIDED};

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
                    return true;
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

void sendSpaceshipPos(int x, int y, TCPsocket sock)
{
    Protocal protocal = SPACESHIP_POS;
    int sent;
    sent = SDLNet_TCP_Send(sock, &protocal, sizeof(protocal));
    if (sent != sizeof(sock, protocal))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    sent = SDLNet_TCP_Send(sock, &x, sizeof(x));
    if (sent != sizeof(sock, x))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    sent = SDLNet_TCP_Send(sock, &y, sizeof(y));
    if (sent != sizeof(sock, y))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
}

void sendSpaceshipCollided(int x, int y, TCPsocket sock)
{
    Protocal protocal = SPACESHIP_COLLIDED;
    int sent;
    sent = SDLNet_TCP_Send(sock, &protocal, sizeof(protocal));
    if (sent != sizeof(sock, protocal))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    sent = SDLNet_TCP_Send(sock, &x, sizeof(x));
    if (sent != sizeof(sock, x))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    sent = SDLNet_TCP_Send(sock, &y, sizeof(y));
    if (sent != sizeof(sock, y))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
}

void sendMaze(Protocal protocal, int network_maze_index, int mazeTop,
              int mazeBottom, TCPsocket sock)
{
    int sent;
    sent = SDLNet_TCP_Send(sock, &protocal, sizeof(protocal));
    if (sent != sizeof(sock, protocal))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    
    sent = SDLNet_TCP_Send(sock, &network_maze_index,
                           sizeof(network_maze_index));    
    if (sent != sizeof(sock, network_maze_index))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    
    sent = SDLNet_TCP_Send(sock, &mazeTop, sizeof(mazeTop));    
    if (sent != sizeof(sock, mazeTop))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
    
    sent = SDLNet_TCP_Send(sock, &mazeBottom, sizeof(mazeBottom));
    if (sent != sizeof(sock, mazeBottom))
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
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

    // variables for network connection
    int serverSpaceship_x = 0;
    int serverSpaceship_y;
    Protocal protocalRecv;

     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    // create the spaceship object
    Spaceship spaceship;
    // offset the client spaceship from the server spaceship
    spaceship.rect.x -= 50;

    // create the spaceship object that will draw were the server spaceship
    // is
    Spaceship spaceshipServer;
    spaceshipServer.xVel = 0;

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
    int mazeLeftIndex;
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

    // cut out some of the maze
    for (int i = 0; i < MAZE_WIDTH; ++i)
    {
        for (int j = mazeTop; j < mazeBottom; ++j)
        {
            maze[i][j].exist = false;
        }
    }

    // variables that will help us keep track of loop time
    int totalTime, timeSinceLastLoop, oldTotalTime;
    bool collisionHappens = false;
    
    // Main client loop
    while (!quit)
    {
        //SDL_Event event;

        // 1 INPUT ----------------------------------------------------------
        
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

        // 2 UPDATE ---------------------------------------------------------

        // position the replica of the server spaceship according to the
        // x y that was recieved through network
        spaceshipServer.rect.x = serverSpaceship_x;
        spaceshipServer.rect.y = serverSpaceship_y;
        
        //if (protocalServerToClient == -1 && spaceshipServer.rect.x != 305)
        if (spaceshipServer.rect.x != 0 && spaceshipServer.rect.x != 305)
        {
            spaceshipServer.xVel = 1;
        }
        
        // determine maze right edge
        mazeRightEdge = determineMazeRightEdge(maze);
                   
        // move maze
        if (spaceshipServer.xVel == 1)
        {
            for (int i = 0; i < MAZE_WIDTH; ++i)
            {
                for (int j = 0; j < MAZE_HEIGHT; ++j)
                {
                    maze[i][j].rect.x -= 2;
                }
            }
        }
        
        // check for ship collision with maze
        if (shipMazeCollision(spaceship, maze))
        {            
            
            collisionHappens = true;
            /*
            // draw game over board
            SDL_BlitSurface( gameover.surface, NULL,
                             screenSurface, &gameover.rect );
            

            //Update the surface
            SDL_UpdateWindowSurface( window );
            
            // wait and then close
            SDL_Delay(3000);
            
            // set quit to true so loop will exit on next iteration
            quit = true;
            */
        }
        
        // 3 DRAW -----------------------------------------------------------

        // fill screen surface
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
        
        // draw server spaceship        
        SDL_BlitSurface( spaceshipServer.surface, NULL, screenSurface,
                         &spaceshipServer.rect );
        
        // draw client spaceship        
        if(!collisionHappens){
            SDL_BlitSurface( spaceship.surface, NULL, screenSurface,
                         &spaceship.rect );
        }
        //Update the surface
        SDL_UpdateWindowSurface( window );

        // 4 NETWORK --------------------------------------------------------
        

        // send spaceship position to the server
        if(!collisionHappens){
            sendSpaceshipPos(spaceship.rect.x, spaceship.rect.y, sock);
        }else{

            sendSpaceshipCollided(0,0,sock);
        }
            
        // recieve from server
        while (SDLNet_CheckSockets(set, 0))
        {
            //SDLNet_CheckSockets(set, 0)
            int got = SDLNet_TCP_Recv(sock, &protocalRecv,
                                      sizeof(serverSpaceship_x));
            if (got <= 0)
            {
                cerr << "Connection problem, quitting..." << endl;
                return;
            }

            if (protocalRecv == SPACESHIP_POS)
            {
                SDLNet_TCP_Recv(sock, &serverSpaceship_x,
                                sizeof(serverSpaceship_x));
                SDLNet_TCP_Recv(sock, &serverSpaceship_y,
                                sizeof(serverSpaceship_y));
            }else if(protocalRecv == SPACESHIP_COLLIDED){

                     SDLNet_TCP_Recv(sock, &serverSpaceship_x,
                                    sizeof(serverSpaceship_x));
                    SDLNet_TCP_Recv(sock, &serverSpaceship_y,
                                    sizeof(serverSpaceship_y));
                    //serverAlive = false;
                    if(collisionHappens){
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
                    
                }
                
            if (protocalRecv == MAZE)
            {
                int network_maze_index;
                SDLNet_TCP_Recv(sock, &network_maze_index,
                                sizeof(network_maze_index));
                SDLNet_TCP_Recv(sock, &mazeTop,
                                sizeof(mazeTop));
                SDLNet_TCP_Recv(sock, &mazeBottom,
                                sizeof(mazeBottom));
                
                for (int j = 0; j < MAZE_HEIGHT; ++j)
                {
                    maze[network_maze_index][j].rect.x = mazeRightEdge;
                    if (j > mazeTop && j < mazeBottom)
                    {
                        maze[network_maze_index][j].exist = false;
                    }
                    else
                    {
                        maze[network_maze_index][j].exist = true;
                    }
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

    SDLNet_TCP_Close(sock);
}

void beTheServer()
{
    // Connection information
    IPaddress ip;

    //variable used to check if the server spaceship collide
    bool collisionHappens = false;

    // Resolve the argument into an IPaddress
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
        
    TCPsocket client = 0;
    
    // variables for network connection
    int clientSpaceship_x;
    int clientSpaceship_y;
    Protocal protocal;
    int network_maze_index;

    // Prepare a SocketSet so we can check for messages from the client
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
    
     //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    // create the spaceship object
    Spaceship spaceship;
    // create the spaceship object that is a replica of the client spaceship
    Spaceship spaceshipClient;

    // create the gameover object
    GameOverBoard gameover;

    // set up score
    int score = 0;
    int distance = 0;
    int SCORE_RATE = 16;
    Score scoreObj;
    scoreObj.font = TTF_OpenFont( "FreeSansBold.ttf", 28 );

    bool clientAlive = true;

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

    // cut out some of the maze
    for (int i = 0; i < MAZE_WIDTH; ++i)
    {
        for (int j = mazeTop; j < mazeBottom; ++j)
        {
            maze[i][j].exist = false;
        }
    }

    // variables that will help us keep track of loop time
    int totalTime, timeSinceLastLoop, oldTotalTime;

    // Main server loop    
    //While application is running
    while( !quit )
    {
        //As a test: Trying to send the spaceship x position to the Client
        //but it's not working yet.
        int serverSpaceship_x = spaceship.rect.x;
        int serverSpaceship_y = spaceship.rect.y;

        // 1 INPUT ----------------------------------------------------------
        
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
        
        // 2 UPDATE ---------------------------------------------------------
       
        // if we are moving sum up the distance and use to determine the score
        if (spaceship.xVel != 0)
        {
            ++distance;
        }
        score = distance / SCORE_RATE;
        
        // position the replica of the client spaceship according to the
        // x y that was recieved through network
        spaceshipClient.rect.x = clientSpaceship_x;
        spaceshipClient.rect.y = clientSpaceship_y;
        
        // move maze
        if (spaceship.xVel != 0)
        {
            for (int i = 0; i < MAZE_WIDTH; ++i)
            {
                for (int j = 0; j < MAZE_HEIGHT; ++j)
                {
                    maze[i][j].rect.x -= 2;
                }
            }
        }
        
        // determine maze right edge
        mazeRightEdge = determineMazeRightEdge(maze);
        
        // if maze touches wall
        for (int i = 0; i < MAZE_WIDTH; ++i)
        {
            if (maze[i][0].rect.x < 0)
            {
                // decide if the maze will change
                int randomChange = rand() % 5;
                // keep track of how far the mazes changes
                //mazeChange = 0;
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
                            }
                        }
                        break;
                    case 1:
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8 && mazeTop > 1)
                            {
                                --mazeTop;
                            }
                        }
                        break;
                    case 2:
                        while (rand() % 2 == 0)
                        {       
                            if (mazeBottom - mazeTop > 5)
                            {
                                --mazeBottom;
                            }
                        }
                        break;
                    case 3:                        
                        while (rand() % 2 == 0)
                        {
                            if (mazeBottom - mazeTop < 8
                                && mazeBottom < MAZE_HEIGHT - 1)
                            {
                                ++mazeBottom;
                            }
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
                }
                // let the network know it is time to send an update to the
                // maze
                protocal = MAZE;
                network_maze_index = i;
            }
        }

        // check for ship collision with maze
        if (shipMazeCollision(spaceship, maze))
        {            
            collisionHappens = true;
            /*// draw game over board
            SDL_BlitSurface( gameover.surface, NULL,
                             screenSurface, &gameover.rect );
            
            //Update the surface
            SDL_UpdateWindowSurface( window );
            
            // wait and then close
            SDL_Delay(3000);
            
            // set quit to true so loop will exit on next iteration
            quit = true;*/
        }

        // 3 DRAW -----------------------------------------------------------

        // fill screen surface
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
        
        // draw client spaceship
        SDL_BlitSurface( spaceshipClient.surface, NULL, screenSurface,
                         &spaceshipClient.rect );
        
        if(!collisionHappens){
            // draw server spaceship
            SDL_BlitSurface( spaceship.surface, NULL, screenSurface,
                            &spaceship.rect );
        }
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

        // 4 NETWORK --------------------------------------------------------

        // If the client socket is still NULL, no-one has connected yet.
        // Check to see if someone wants to connect.

        if (client == 0)
        {
            client = SDLNet_TCP_Accept(listener);

            // If it isn't zero any more, the client socket is now connected.
            // Add it to the SocketSet so that we can check it for data later.

            if (client != 0)
                if (SDLNet_TCP_AddSocket(set, client) == -1)
                    cerr << "SDLNet_AddSocket: " << SDLNet_GetError()
                         << endl;
        }

        // If we're connected to a client, we may have data to send, and we
        // should check to see if they've sent any data to us.
        
        if (client != 0)
        {
            if (protocal == MAZE)
            {
                sendMaze(protocal, network_maze_index, mazeTop, mazeBottom,
                         client);
            }
            
            // send spaceship position to the client
            if(!collisionHappens){
                sendSpaceshipPos(spaceship.rect.x, spaceship.rect.y, client);
            }else{

                sendSpaceshipCollided(0,0,client);
            }

            int clientSpaceship_X;
            while (SDLNet_CheckSockets(set, 0))
            {
                int got = SDLNet_TCP_Recv(client, &protocal,
                                          sizeof(protocal));
                if (got <= 0)
                {
                    cerr << "Connection problem, quitting..." << endl;
                    return;
                }

                if (protocal == SPACESHIP_POS)
                {
                    SDLNet_TCP_Recv(client, &clientSpaceship_x,
                                    sizeof(clientSpaceship_x));
                    SDLNet_TCP_Recv(client, &clientSpaceship_y,
                                    sizeof(clientSpaceship_y));
                }else if(protocal == SPACESHIP_COLLIDED){

                     SDLNet_TCP_Recv(client, &clientSpaceship_x,
                                    sizeof(clientSpaceship_x));
                    SDLNet_TCP_Recv(client, &clientSpaceship_y,
                                    sizeof(clientSpaceship_y));
                    clientAlive = false;
                    if(collisionHappens){
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
                    
                }
            }
            protocal = DONT_SEND_MAZE;
            
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
