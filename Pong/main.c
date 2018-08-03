//
//  main.c
//  Pong
//
//  Created by Thomas Foster on 7/27/18.
//

#include <stdio.h>
#include <time.h>
#include "sys.h" // this also includes "tffunctions.h", but see my comment there
#include "tffunctions.h"

#define P0X				16	// paddle[0] x position
#define P1X				SCREENWIDTH-PADDLEWIDTH-16
#define PADDLEWIDTH		16
#define PADDLEHEIGHT	96
#define BALLRADIUS		16

#define PADDLESPEED		2
#define BALLSPEED		1
#define FRAMESKIP		10
#define SLEEPTIME		2

typedef struct
{
	rect_t frame;
	int dx, dy;
} mobrect_t;
// I think this is what they call in the game biz an entity_t

const int		scoresy = 8; // score labels along the top
typedef struct
{
	SDL_Texture *texture;
	int x;	// y is constant, x set each time by UpdateScores
	int w,h;
} label_t;

// I prefer to put these kinds of things in like a game_t struct and pass that to functions like Initialize(),
// SetupBall(), ProcessEvents(), ProcessGame(), etc. The reason is mostly organization but also name collision. If,
// for instance, in any function, you create a local 'ticks' variable, you will no longer be able modify the global
// 'ticks' variable. The other reason, is that I like to know what variables my functions are going to be modifying,
// which is why I like passing in arguments.
int 			ticks;
mobrect_t		paddle[2];
mobrect_t		ball;

unsigned short	scores[2];
label_t			labels[2];

bool			updatescores, resetgame;



//
//  SetupBall
/// Setup up initial ball position and direction
///	Called at start and after reset.
//
void SetupBall(void)
{
    // it's just my preference but I like variables on their own lines
    // the reason is really only related to pointers. These are the same
    // int a, *b; | int  a;
    //            | int* b;
    // idk I just really don't like that one on the left lol, so I just always put var decls on their own lines
	int ballx, bally;
	bool signx, signy;

	ballx = SCREENWIDTH/2-BALLRADIUS;		// center x
	bally = 1+rand()%(SCREENHEIGHT-BALLRADIUS-1);	// random y

    // I'm not sure if there is actually too much difference between these 2 rand() evals and if rand() was actually
    // a well distributed function, I think you could replace these calls with rand() % 2. But if you happen to care,
    // look up some stuff about rand().. it sux. the c++11 standard library re-vamped the standard library random
    // modules and it's much better.
	signx = (rand()%888 > 444);			// random sign for starting dir
	signy = (rand()%90210 > 45105);

	ball.frame = MakeRect(ballx, bally, BALLRADIUS, BALLRADIUS); // it's "approximately" round!

    // look at you usin' the ternary operator
	ball.dx = signx ? BALLSPEED : -BALLSPEED;
	ball.dy = signy ? BALLSPEED : -BALLSPEED;
}


//
//  Initialize
/// Set paddle positions and ball, etc.
//	Called once before game loop.
//
void Initialize(void)
{
	int pady;
	int i;

	srand((int)time(NULL));
    // i don't know if this is true on mac with clang, but on linux with gcc globals are initialized to 0
	ticks = 0;

	// set paddle positions
	pady = SCREENHEIGHT/2 - PADDLEHEIGHT/2;
	paddle[0].frame = MakeRect(P0X, pady, PADDLEWIDTH, PADDLEHEIGHT);
	paddle[1].frame = MakeRect(P1X, pady, PADDLEWIDTH, PADDLEHEIGHT);

    // There is an alternative here, in #include <string.h>, there is a function memset(), where you can
    // zero the structure completely without having to set individual fields
    // memset(&paddle, 0, sizeof(paddle));
    // memset(&labels, 0, sizeof(labels));
    // memset(&scores, 0, sizeof(scores));
    // that being said, you would have to do it before you set the paddle[0 and 1].frames just above here.
	for (i=0;i<2;i++)
	{
        // I'm not a fan of this, although I see it occasionally, I prefer
        // paddle[i].dx = 0;
        // paddle[i].dy = 0;
		paddle[i].dx = paddle[i].dy = 0;
		labels[i].texture = NULL;
		labels[i].x = 0;
		labels[i].w = labels[i].h = 0;
		scores[i] = 0;
	}

	updatescores = true;

	SetupBall();
}



//
//  UpdateScores
/// Generate labels and positions for the score
//
void UpdateScores(void)
{
    // if I had to remake textures every time I wanted to render different text, I'd probably make this too
	if (!updatescores)
		return;
	
	char msg0[8] = ""; char msg1[8] = "";
	SDL_Surface *temp1, *temp2;
	
	sprintf(msg0, "%d",	scores[0]);
	sprintf(msg1, "%d", scores[1]);
	
	SDL_Color gray = { 128,128,128,255 };
	
	// make the labels
	temp1 = TTF_RenderText_Solid(font, msg0, gray);
	temp2 = TTF_RenderText_Solid(font, msg1, gray);
	if (!temp1 || !temp2)
        // just a question, do you ever get any `Oops` appearing in the console ?
        // Also, I hope SDL_CreateTextureFromSurface() and SDL_FreeSurface() can handle
        // being passed NULL pointers
		PrintSDLError("Oops");
	
    // you may want to consider creating function here because this code is duplicated with the only
    // difference being the .x field. I'd do this if the plan was to have more text in the future (maybe for menus)
	labels[0].texture = SDL_CreateTextureFromSurface(renderer, temp1);
	labels[0].x = SCREENWIDTH/4-temp1->w/2;
	labels[0].w = temp1->w;
	labels[0].h = temp1->h;

	labels[1].texture = SDL_CreateTextureFromSurface(renderer, temp2);
	labels[1].x = SCREENWIDTH*0.75-temp2->w/2;
	labels[1].w = temp2->w;
	labels[1].h = temp2->h;

	SDL_FreeSurface(temp1);
	SDL_FreeSurface(temp2);

	updatescores = false;
}



//
//  Display Scores
/// Render the labels that display the scores
//
void DisplayScores(void)
{
	SDL_Rect r0 = { labels[0].x, scoresy, labels[0].w, labels[0].h };
	SDL_Rect r1 = { labels[1].x, scoresy, labels[1].w, labels[1].h };
	SDL_RenderCopy(renderer, labels[0].texture, NULL, &r0);
	SDL_RenderCopy(renderer, labels[1].texture, NULL, &r1);
}



//
//  ProcessEvents
///	Return false to exit gameloop
//
bool ProcessEvents(void)
{
	SDL_Event	ev;
	
	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_QUIT)
			return false;
		if (ev.type == SDL_KEYDOWN) {
			switch (ev.key.keysym.sym) {
				case SDLK_ESCAPE:
					return false;
			}
		}
	}
	
    // I actually didn't know about SDL_GetKeyboardState(), I've always tracked my key presses through events, sweet
	const unsigned char *state = SDL_GetKeyboardState(NULL);

    // this is just my preference but I like something like this:
    // paddle[1].dy = 0
    // if(state[SDL_SCANCODE_UP])
    //     paddle[1].dy = -PADDLESPEED;
    // if(state[SDL_SCANCODE_DOWN])
    //     paddle[1].dy = PADDLESPEED;
    // this way, if they hold down both keys or no, it doesn't move, but if they hold down one key, it goes in that dir
	if (state[SDL_SCANCODE_UP])
		paddle[1].dy = -PADDLESPEED;
	else if (state[SDL_SCANCODE_DOWN])
		paddle[1].dy = PADDLESPEED;
	else
		paddle[1].dy = 0;

	if (state[SDL_SCANCODE_W])
		paddle[0].dy = -PADDLESPEED;
	else if (state[SDL_SCANCODE_S])
		paddle[0].dy = PADDLESPEED;
	else
		paddle[0].dy = 0;

	
	return true;
}




void ProcessGame(void)
{
	box_t bbox, pbox[2];
	int *pady;
	int i;

	ticks++;
	
    // with this ticks frameskip thing commented out, does the 'ticks' variable do anything ?
//	if (ticks % FRAMESKIP != 0)
//		return;
	
	if (resetgame)
	{
		SDL_Delay(2000); // TODO Change this to something better
		SetupBall();
		resetgame = false;
	}

	
	// MOVE THE BALL
	
	ball.frame.origin.x += ball.dx;
	ball.frame.origin.y += ball.dy;
	MakeBoxFromRect(&bbox, &ball.frame);
	if (bbox.right < 0) {
		Mix_PlayChannel(-1, goal, 0);
		scores[1]++;
		updatescores = true;
		resetgame = true;
	}
	if (bbox.left > SCREENWIDTH) {
		Mix_PlayChannel(-1, goal, 0);
		scores[0]++;
		updatescores = true;
		resetgame = true;
	}
	if (bbox.top <= 0 || bbox.bottom >= SCREENHEIGHT) {
		Mix_PlayChannel(-1, tophit, 0);
		ball.dy = -ball.dy;
	}

	
	// CHECK COLLISION
	
	for (i=0;i<2;i++) {
		if (tfRectCollision(&paddle[i].frame, &ball.frame)) {
			Mix_PlayChannel(-1, padhit, 0);
			ball.dx = -ball.dx;
		}
	}
	
	
	// MOVE THE PADDLES

	for (i=0;i<2;i++)
	{
		pady = &paddle[i].frame.origin.y;
		*pady += paddle[i].dy;
		
		MakeBoxFromRect(&pbox[i], &paddle[i].frame);
		if (pbox[i].top <= 0) {
			*pady = paddle[i].dy = 0;
		}
		if (pbox[i].bottom >= SCREENHEIGHT) {
			*pady = SCREENHEIGHT-PADDLEHEIGHT;
			paddle[i].dy = 0;
		}
	}
	UpdateScores();
}





void Display(void)
{
	int i;
    // I think here you can just use one SDL_Rect for all your calls to RectToSDL() and SDL_RenderFillRect()
	SDL_Rect paddlerects[2];
	SDL_Rect ballrect;
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	SDL_RenderClear(renderer);
	
	DisplayScores();
	
	// DRAW PADDLES
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (i=0;i<2;i++) {
		RectToSDL(&paddlerects[i], &paddle[i].frame);
		SDL_RenderFillRect(renderer, &paddlerects[i]);
	}
	

	// DRAW BALL
	RectToSDL(&ballrect, &ball.frame);
	SDL_RenderFillRect(renderer, &ballrect);
	
	SDL_RenderPresent(renderer);
}



int main(int argc, const char * argv[])
{	
	StartSDL();
	Initialize();
	
	while (1)
	{
		if (!ProcessEvents())
			break; // user quit

		ProcessGame();
		Display();
		SDL_Delay(SLEEPTIME);
	}
	
	StopSDL();
	return 0;
}
