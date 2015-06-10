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
#include <iostream>
#include <locale>
#include <sstream>
#include "cBrothel.h"
#include "cClinic.h"
#include "cClinicScreen.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include "cGangs.h"

extern int						g_CurrBrothel;
extern int						g_CurrentScreen;
extern int						g_Building;
extern bool						g_InitWin;
extern bool						g_Cheats;
extern bool						eventrunning;
extern bool						g_AllTogle;
extern cGold					g_Gold;
extern string					g_ReturnText;
extern cGangManager				g_Gangs;
extern cBrothelManager			g_Brothels;
extern cClinicManager			g_Clinic;
extern cWindowManager			g_WinManager;
extern cInterfaceEventManager	g_InterfaceEvents;

bool cClinicScreen::ids_set = false;

void cClinicScreen::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	walk_id = get_id("WalkButton");
	curbrothel_id = get_id("CurrentBrothel");
	clinic_id = get_id("Clinic");
	girls_id = get_id("Girls");
	staff_id = get_id("Staff");
	dungeon_id = get_id("Dungeon");
	turns_id = get_id("Turn");
	weeks_id = get_id("Weeks");
	setup_id = get_id("SetUp");
	info_id = get_id("Info");
	nextbrothel_id = get_id("Next");
	prevbrothel_id = get_id("Prev");
	clinicdetails_id = get_id("ClinicDetails");
}

void cClinicScreen::init()
{
	g_CurrentScreen = SCREEN_CLINIC;
	g_Building = BUILDING_CLINIC;
	/*
	*	buttons enable/disable
	*/
}

void cClinicScreen::process()
{
	/*
	*	we need to make sure the ID variables are set
	*/
	if (!ids_set) set_ids();
	init();

	if (g_InitWin)
	{
		EditTextItem(g_Clinic.GetBrothelString(0), clinicdetails_id);
		g_InitWin = false;
	}
	/*
	*	no events means we can go home
	*/
	if (g_InterfaceEvents.GetNumEvents() == 0) return;

	/*
	*	otherwise, compare event IDs
	*
	*	if it's the back button, pop the window off the stack
	*	and we're done
	*/
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckButton(walk_id))
	{
		g_InitWin = true;
		g_WinManager.push("Clinic Try");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(girls_id))
	{
		g_InitWin = true;
		g_WinManager.push("Clinic");
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
		g_Building = BUILDING_CLINIC;
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
}