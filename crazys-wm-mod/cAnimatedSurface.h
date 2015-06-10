/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "CSurface.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class CSurface;

// Class to hold the data for a single animation within an image
class cAnimatedSurface
{
public:
	cAnimatedSurface();
	~cAnimatedSurface();

	void PlayOnce(bool playOnce) {m_PlayOnce = playOnce;}

	void Stop() {m_LastTime=0;}

	bool DrawFrame(int x, int y, int width, int height, unsigned int currentTime);	// Updates animation according to speed, and then draws it on the screen
	void SetData(int xPos, int yPos, int numFrames, int speed, int width, int height, CSurface* surface);
	void UpdateSprite(SDL_Rect& rect, int width, int height);

private:
	bool m_FrameDone;
	bool m_PlayOnce;

	int m_CurrentFrame;				// Current frame in a playing animation
	int m_CurrentRow;				// current row playing the animation from
	int m_CurrentColumn;
	int m_Speed;					// Speed to play the animation (in time between draws)
	int m_NumFrames;				// Number of frames in the animation
	unsigned int m_LastTime;		// The last time animation was updated
	int m_Rows;
	int m_Colums;

	CSurface* m_Surface;			// pointer to the image where all the sprites are kept
	SDL_Surface* m_SpriteSurface;	// pointer to the image where the current sprite is kept
	SDL_Rect m_Frames;				// Holds the data for ALL frames, since all frames have same width/height
};

// Manages a file with multiple animations
class CAnimatedSprite
{
public:
	CAnimatedSprite(){m_Animations=0; m_Image=0;m_CurrAnimation=0;}
	~CAnimatedSprite();

	void Free();

	bool LoadAnimations(string imgFilename, string animationData);
	bool Draw(int x, int y, int width, int height, unsigned int currentTime);
	void SetAnimation(int animation) {m_CurrAnimation = animation;}

private:
	int m_CurrAnimation;
	cAnimatedSurface* m_Animations;
	CSurface* m_Image;
};
