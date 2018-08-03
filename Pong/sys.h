
#ifndef sys_h
#define sys_h

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#include "tffunctions.h"

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
