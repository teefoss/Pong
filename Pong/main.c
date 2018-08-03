//
//  main.c
//  Pong
//
//  Created by Thomas Foster on 7/27/18.
//

#include <stdio.h>
#include <time.h>
#include "sys.h"
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

const int		scoresy = 8; // score labels along the top
typedef struct
{
	SDL_Texture *texture;
	int x;	// y is constant, x set each time by UpdateScores
	int w,h;
} label_t;

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
	int ballx, bally;
	bool signx, signy;

	ballx = SCREENWIDTH/2-BALLRADIUS;		// center x
	bally = 1+rand()%(SCREENHEIGHT-BALLRADIUS-1);	// random y
	signx = (rand()%888 > 444);			// random sign for starting dir
	signy = (rand()%90210 > 45105);
	
	ball.frame = MakeRect(ballx, bally, BALLRADIUS, BALLRADIUS); // it's "round"!
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
	ticks = 0;
	
	// set paddle positions
	pady = SCREENHEIGHT/2 - PADDLEHEIGHT/2;
	paddle[0].frame = MakeRect(P0X, pady, PADDLEWIDTH, PADDLEHEIGHT);
	paddle[1].frame = MakeRect(P1X, pady, PADDLEWIDTH, PADDLEHEIGHT);
	for (i=0;i<2;i++)
	{
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
		PrintSDLError("Oops");
	
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
	
	const unsigned char *state = SDL_GetKeyboardState(NULL);
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
