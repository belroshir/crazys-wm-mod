/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
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
#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cMovieScreen.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include <iostream>
#include <locale>
#include <sstream>
#include "cGangs.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cMovieStudioManager		g_Studios;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern bool g_Cheats;
extern	bool	eventrunning;
extern string g_ReturnText;
extern cGangManager g_Gangs;
extern bool g_AllTogle;
extern int g_CurrentScreen;
extern int g_Building;

bool cMovieScreen::ids_set = false;

void cMovieScreen::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	walk_id = get_id("WalkButton");
	curbrothel_id = get_id("CurrentBrothel");
	movie_id = get_id("Movie");
	girls_id = get_id("Girls");
	staff_id = get_id("Staff");
	dungeon_id = get_id("Dungeon");
	turns_id = get_id("Turn");
	weeks_id = get_id("Weeks");
	setup_id = get_id("SetUp");
	nextbrothel_id = get_id("Next");
	prevbrothel_id = get_id("Prev");
	moviedetails_id = get_id("MovieDetails");
}

void cMovieScreen::init()
{
	g_CurrentScreen = SCREEN_STUDIO;
	g_Building = BUILDING_STUDIO;

	/*
	*	buttons enable/disable
	*/
#if 0
	DisableButton(walk_id, g_TryOuts);
#endif

}

void cMovieScreen::process()
{
	/*
	*	we need to make sure the ID variables are set
	*/
	if (!ids_set) {
		set_ids();
	}

	init();

	if (g_InitWin)
	{
		EditTextItem(g_Studios.GetBrothelString(0), moviedetails_id);
		g_InitWin = false;
	}
	/*
	*	no events means we can go home
	*/
	if (g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

	/*
	*	otherwise, compare event IDs
	*
	*	if it's the back button, pop the window off the stack
	*	and we're done
	*/
	if (g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckButton(walk_id))
	{
		g_InitWin = true;
		g_WinManager.push("Casting Try");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(girls_id))
	{
		g_InitWin = true;
		g_WinManager.push("Studio");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(staff_id))
	{
		g_InitWin = true;
		g_WinManager.push("Gangs");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(turns_id))
	{
		g_InitWin = true;
		g_WinManager.Push(Turnsummary, &g_Turnsummary);
		return;
	}
	else if (g_InterfaceEvents.CheckButton(setup_id))
	{
		g_Building = BUILDING_STUDIO;
		g_InitWin = true;
		g_WinManager.push("Building Setup");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(dungeon_id))
	{
		g_InitWin = true;
		g_WinManager.push("Dungeon");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(weeks_id))
	{
		g_InitWin = true;
		AutoSaveGame();
		NextWeek();
		g_WinManager.Push(Turnsummary, &g_Turnsummary);
		return;
	}
	else if (g_InterfaceEvents.CheckButton(nextbrothel_id))
	{
		g_InitWin = true;
		g_WinManager.push("Arena Screen");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(prevbrothel_id))
	{
		g_InitWin = true;
		g_WinManager.push("Centre Screen");
		return;
	}
}
