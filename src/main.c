#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define WIDTH       500

void populateArray(int *lifeArray, int arLength)
{
    for (int i = 0; i < arLength; i++)
    {
        for (int j = 0; j < arLength; j++)
        {
            lifeArray[i*arLength+j] = rand() % 2;	//Initialize each cell to be randomly alive or dead
        }
    }
    
    //Setting the gutters to match the right parts of the core grid
    
    lifeArray[0] = lifeArray[(arLength - 1)*arLength - 2];
    lifeArray[arLength - 1] = lifeArray[arLength * (arLength - 2) + 1];
    lifeArray[(arLength)*(arLength - 1)] = lifeArray[arLength * 2 - 2];
    lifeArray[arLength*arLength - 1] = lifeArray[arLength + 1];
    
    for (int j = 1; j < arLength-1; j++)
    {
        lifeArray[j] = lifeArray[arLength*(arLength - 2) + j];					//Put bottom row along top
        lifeArray[arLength * (arLength -1) + j] = lifeArray[arLength + j];		//Put top row along bottom
    }
    
    
    for (int i = 1; i < arLength-1; i++)
    {
        lifeArray[i*arLength] = lifeArray[i*arLength + (arLength-2)];			//Put right row along left
        lifeArray[i*arLength + (arLength - 1)] = lifeArray[i*arLength + 1];		//Put left row along right
    }
    
}

void writeFirstGeneration(int * lifeArray, int * myTempArray, int len)
{
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len; j++)
        {
            myTempArray[i*len+j] = lifeArray[i*len + j];
        }
    }
}

void copyBoardToSurfacePixels(int *lifeArray, int *pPixels, int len)
{
    for (int i = 1; i < len-1; i++)
    {
        for (int j = 1; j < len-1; j++)
        {
            // Color in the gutter in a different color so it does not appear part of the board
            if (i == 0 || j == 0 || i == len-1 || j == len-1)
            {
                *pPixels=0x00FFD700;
                pPixels += 1;
            }
            else if (lifeArray[i*len + j] == 0)
            {
                *pPixels=0x00ffcccc;
                pPixels += 1;
            }
            else
            {
                *pPixels=0x00000000;
                pPixels += 1;
            }
        }
    }
}

void updateAndWriteGeneration(int * lifeArray, int * myTempArray, int len)
{
    for (int i = 1; i < len - 1; i++)
    {
        for (int j = 1; j < len - 1; j++)
        {
            int sum = lifeArray[i*len + j - 1] +		//left neighbor
            lifeArray[i*len + j + 1] +		//right neighbor
            lifeArray[(i - 1)*len + j] +		//upper neighbor
            lifeArray[(i + 1)*len + j] +		//lower neighbor
            lifeArray[(i - 1)*len + j - 1] +	//upper left
            lifeArray[(i - 1)*len + j + 1] +	//upper right
            lifeArray[(i + 1)*len + j - 1] +		//lower left neighbor
            lifeArray[(i + 1)*len + j + 1];		//lower right neighbor
            
            if (sum == 3)
            {
                myTempArray[i*len + j] = 1;	//It liiiiiives
            }
            else if (sum == 2 && lifeArray[i*len + j] == 1)
            {
                myTempArray[i*len + j] = 1;	//It stays alive
            }
            else
            {
                myTempArray[i*len + j] = 0;	//Cell is not alive
            }
        }
    }
    
    //Setting the gutters to match the right parts of the core grid
    
    myTempArray[0] = myTempArray[(len - 1)*len - 2];
    myTempArray[len - 1] = myTempArray[len * (len - 2) + 1];
    myTempArray[(len)*(len - 1)] = myTempArray[len * 2 - 2];
    myTempArray[len*len - 1] = myTempArray[len + 1];
    
    for (int j = 1; j < len - 1; j++)
    {
        myTempArray[j] = myTempArray[len*(len - 2) + j];				//Put bottom row along top
        myTempArray[len * (len - 1) + j] = myTempArray[len + j];		//Put top row along bottom
    }
    
    for (int i = 1; i < len - 1; i++)
    {
        myTempArray[i*len] = myTempArray[i*len + (len - 2)];			//Put right row along left
        myTempArray[i*len + (len - 1)] = myTempArray[i*len + 1];		//Put left row along right
    }
    
    //Transfer contents of tempArray into myArray
    
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len; j++)
        {
            lifeArray[i*len + j] = myTempArray[i*len + j];
        }
    }
}

int main ( int argc, char *argv[] ) {
    int myArrayDeclaredLength = WIDTH;                //Actual size of desired grid (side length)
    const int myArrayLength = myArrayDeclaredLength + 2;	//Size with gutters added
    int myArray [WIDTH + 2][WIDTH + 2];     //Primary array
    int tempArray[WIDTH + 2][WIDTH + 2];	//Temporary array from which to update primary
    int seed = 7;
    int numGens = 10000;                            //Number of gens to run simulation
    
    srand(seed);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow(
        "Game of Live",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        WIDTH,
        SDL_WINDOW_SHOWN
    );

    SDL_Surface *surface = SDL_GetWindowSurface(win);
    int *pixels = (int *)surface->pixels;

    populateArray(&myArray[0][0], myArrayLength);
    writeFirstGeneration(&myArray[0][0], &tempArray[0][0], myArrayLength);
    copyBoardToSurfacePixels(&myArray[0][0], pixels, myArrayLength);
    SDL_UpdateWindowSurface(win);

    SDL_Event event;
    int running = 1;
    
    // Update board and display each new generation
    
    for (int t = 0; t < numGens-1; t++)
    {
        while (running)
        {
            while (SDL_PollEvent (&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    running = 0;
                }
            }
            
            //Update array and store "next generation" in tempArray
            updateAndWriteGeneration(&myArray[0][0], &tempArray[0][0], myArrayLength);
            copyBoardToSurfacePixels(&myArray[0][0], pixels, myArrayLength);
            SDL_Delay(10);
            SDL_UpdateWindowSurface(win);
        }
    }
    
    SDL_Quit();
    

    return EXIT_SUCCESS;
}