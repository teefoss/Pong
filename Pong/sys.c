//
//  sys.c
//
//  Created by Thomas Foster on 4/15/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//
//  starting/stopping SDL, loading and assets
//

#include "sys.h"

SDL_Window		*window = NULL;
SDL_Surface		*screensurface = NULL;
SDL_Renderer	*renderer = NULL;
TTF_Font		*font = NULL;

#define VOLUME 8
Mix_Chunk *tophit = NULL;
Mix_Chunk *padhit = NULL;
Mix_Chunk *goal = NULL;



void LoadWavFile(Mix_Chunk *chunk, const char *file)
{
	chunk = Mix_LoadWAV(file);
	if (!chunk) {
		printf("LoadWavFile: couldn't load %s!",file);
		PrintSDLError("");
	}
	Mix_VolumeChunk(chunk, VOLUME);
} // Justin: shouldn't this work? pointer f!%k-up?


bool PrintSDLError(const char *message)
{
	printf("%s SDL Error: %s\n", message, SDL_GetError());
	return false;
}


void StartSDL(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("StartSDL: Error! Count not init. SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}
	printf("SDL_Init: success\n");
	
	// WINDOW & RENDERER
	
	window = SDL_CreateWindow(WINDOWNAME,
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  SCREENWIDTH,
							  SCREENHEIGHT,
							  SDL_WINDOW_SHOWN);
	if (!window) {
		PrintSDLError("StartSDL: Error! Could not create window");
		SDL_Quit();
		exit(1);
	}
	
#ifdef FULLSCREEN
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
#endif
	
	renderer = SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		PrintSDLError("InitRenderer: Could not create renderer");
		SDL_Quit();
		exit(1);
	}
	screensurface = SDL_GetWindowSurface(window);

	
	// FONTS
	
#ifdef FONT
	if (TTF_Init() == -1) {
			PrintSDLError("StartSDL: Error! Could not init TTF");
			SDL_Quit();
			exit(1);
		}
		
		font = TTF_OpenFont(FONTNAME, FONTSIZE);
		if (!font) {
			PrintSDLError("StartSDL: Error! Could not load font");
		}
#endif
	
	
	// AUDIO
	
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512) == -1) {
		PrintSDLError("Could not open audio.");
		SDL_Quit();
		exit(1);
	}
//	LoadWavFile(tophit, "tophit.wav");
//	LoadWavFile(padhit, "padhit.wav");
//	LoadWavFile(goal, "goal.wav");
	tophit = Mix_LoadWAV("tophit.wav");
	padhit = Mix_LoadWAV("padhit.wav");
	goal = Mix_LoadWAV("goal.wav");
	if (!tophit || !padhit || !goal)
	{
		PrintSDLError("Could not load sound!");
		SDL_Quit();
		exit(1);
	}
	Mix_VolumeChunk(tophit, VOLUME);
	Mix_VolumeChunk(padhit, VOLUME);
	Mix_VolumeChunk(goal, VOLUME);
}


//
// StopSDL
//
void StopSDL()
{
	SDL_DestroyRenderer(renderer);
	renderer = NULL; // Justin: saw this in a tutorial. Is it really necessary if about to quit?

	SDL_DestroyWindow(window);
	window = NULL;

#ifdef FONT
	TTF_CloseFont(font);
	TTF_Quit();
	font = NULL;
#endif
	
	Mix_CloseAudio();
	Mix_FreeChunk(tophit);
	Mix_FreeChunk(padhit);
	Mix_FreeChunk(goal);

	SDL_Quit();
}


