//
//  tffunctions.c
//  Pong
//
//  Created by Thomas Foster on 7/27/18.
//  Copyright © 2018 Thomas Foster. All rights reserved.
//

#include "tffunctions.h"

point_t MakePoint(int x, int y)
{
	point_t pt;
	
	pt.x = x;
	pt.y = y;
	return pt;
}

sizetype MakeSize(int w, int h)
{
	sizetype s;
	
	s.width = w;
	s.height = h;
	return s;
}

rect_t MakeRect(int x, int y, int w, int h)
{
	rect_t r;
	
	r.origin = MakePoint(x, y);
	r.size = MakeSize(w, h);
	return r;
}




//
// tfRectToSDL
// Converts a rect_t to SDL_Rect
//
void RectToSDL(SDL_Rect *dest, rect_t *src)
{
	dest->x = src->origin.x;
	dest->y = src->origin.y;
	dest->w = src->size.width;
	dest->h = src->size.height;
}



void MakeBoxFromRect(box_t *destbox, rect_t const *srcrect)
{
	destbox->left = srcrect->origin.x;
	destbox->right = srcrect->origin.x + srcrect->size.width;
	destbox->top = srcrect->origin.y;
	destbox->bottom = srcrect->origin.y + srcrect->size.height;
}



bool tfRectCollision(rect_t *aRect, rect_t *bRect)
{
	box_t abox, bbox;
	bool xaligned, yaligned;
	
    // I think this collision checking only works if the ball has a speed of 1, we'd need something a bit smarter if
    // the ball goes faster
	MakeBoxFromRect(&abox, aRect);
	MakeBoxFromRect(&bbox, bRect);
	xaligned = !(abox.top >= bbox.bottom || abox.bottom <= bbox.top);
	yaligned = !(abox.left >= bbox.right || abox.right <= bbox.left);
	
	if (xaligned && yaligned)
		return true;
	return false;
}


