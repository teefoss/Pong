
#ifndef sys_h
#define sys_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>    // when I did a linux install of SDL2_ttf, my package manager installed it under /usr/include/SDL2/
#include <SDL2/SDL_mixer.h>  // when I did a linux install of SDL2_mixer, my package manager installed it under /usr/include/SDL2/
#include "tffunctions.h" // I don't think you use anything from tffunctions.h in this file or sys.c, so I would only include it in main.c

// Window Options
#define WINDOWNAME		"Pong"
#define SCREENWIDTH 	640
#define SCREENHEIGHT 	480
//#define FULLSCREEN

// Font Options
#define FONT

#ifdef FONT
#define FONTSIZE		48
#define FONTNAME		"Px437_IBM_VGA9.ttf"
#endif

extern SDL_Window		*window;
extern SDL_Renderer		*renderer;
extern TTF_Font			*font;

extern Mix_Chunk		*tophit;
extern Mix_Chunk		*padhit;
extern Mix_Chunk		*goal;


bool PrintSDLError(const char *message);
void StartSDL(void);
void StopSDL(void);

#endif /* sys_h */
