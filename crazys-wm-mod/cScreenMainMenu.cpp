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
#include "cScreenMainMenu.h"
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"
#include "FileList.h"

extern void NewGame();
extern void LoadGameScreen();
extern void GetString();

extern void PreparingLoad();
extern string g_ReturnText;


extern cWindowManager g_WinManager;

extern bool g_InitWin;
extern int g_CurrentScreen;

extern bool g_C_Key;	// continue
extern bool g_L_Key;	// load game
extern bool g_N_Key;	// new game
extern bool g_Q_Key;	// quit
extern bool g_S_Key;	// settings

bool cScreenMainMenu::ids_set = false;

void cScreenMainMenu::set_ids()
{
	ids_set = true;
	continue_id = get_id("Continue");
	load_id = get_id("Load Game");
	new_id = get_id("New Game");
	settings_id = get_id("Settings");
	quit_id = get_id("Quit Game");
}

cScreenMainMenu::cScreenMainMenu()
{
	cConfig cfg;
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "main_menu.xml";
	m_filename = dp.c_str();
}

void cScreenMainMenu::init()
{
	g_CurrentScreen = SCREEN_MAINMENU;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		
		DirPath location = DirPath(cfg.folders.saves().c_str());
		const char *pattern = "autosave.gam";
		FileList fl(location, pattern);
		DisableButton(continue_id, fl.size() < 1);		// `J` disable continue button if autosave.gam is not found

		pattern = "*.gam";
		FileList fla(location, pattern);
		DisableButton(load_id, fla.size() < 1);			// `J` disable load game button if there are no save games found


		DisableButton(settings_id, true);			// `J` disable settings button until settings page is added

	}

}

void cScreenMainMenu::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

void cScreenMainMenu::check_events()
{
	// no events means we can go home
	if (g_InterfaceEvents.GetNumEvents() == 0) return;

	if (g_InterfaceEvents.CheckButton(new_id))
	{
		// the new new game code
		g_WinManager.push("New Game");
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(continue_id))		// `J` not ready yet
	{
		g_ReturnText = "autosave.gam";
		g_WinManager.Push(PreparingLoad, &g_Preparing);
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(load_id))
	{
		g_WinManager.Push(LoadGameScreen, &g_LoadGame);
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(quit_id))
	{
		SDL_Event evn;
		evn.type = SDL_QUIT;
		SDL_PushEvent(&evn);
	}
}

bool cScreenMainMenu::check_keys()
{
	if (g_C_Key && !m_Buttons[continue_id]->m_Disabled)
	{
		g_C_Key = false;
		g_ReturnText = "autosave.gam";
		g_WinManager.Push(PreparingLoad, &g_Preparing);
		g_InitWin = true;
		return true;
	}
	if (g_L_Key)
	{
		g_L_Key = false;
		g_WinManager.Push(LoadGameScreen, &g_LoadGame);
		g_InitWin = true;
		return true;
	}
	if (g_N_Key)
	{
		g_N_Key = false;
		// the new new game code
		g_WinManager.push("New Game");
		g_InitWin = true;
		return true;
	}
	if (g_Q_Key)
	{
		g_Q_Key = false;
		SDL_Event evn;
		evn.type = SDL_QUIT;
		SDL_PushEvent(&evn);
		return true;
	}
	if (g_S_Key)
	{
		g_S_Key = false;
		return true;
	}

	return false;
}