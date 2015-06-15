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
#include "cScreenPrison.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "libintl.h"

#ifdef LINUX
#include "linux.h"
#endif

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	int		g_CurrentScreen;

void cScreenPrison::init()
{
	stringstream ss;
	g_CurrentScreen = SCREEN_JAIL;
	if(!g_InitWin) { return; }
	Focused();
	g_InitWin = false;

/*
 *	buttons enable/disable
 */
	DisableButton(more_id, true);
	DisableButton(release_id, true);
	selection=-1;
	update_details();

	int i=0;
	ClearListBox(prison_list_id);
	sGirl* pgirls = g_Brothels.GetPrison();
	while(pgirls)
	{
		stringstream ss;
		int cost = PrisonReleaseCost(pgirls);
		ss << pgirls->m_Realname << gettext("  (release cost: ") << cost << gettext(" gold)");
		AddToListBox(prison_list_id, i, ss.str());
		i++;
		pgirls = pgirls->m_Next;
	}
}

bool cScreenPrison::ids_set = false;

void cScreenPrison::set_ids()
{
	stringstream ss;

	ids_set			= true;
	header_id		= get_id("ScreenHeader");
	back_id			= get_id("BackButton");
	more_id			= get_id("ShowMoreButton");
	release_id		= get_id("ReleaseButton");
	prison_list_id	= get_id("PrisonList");
	girl_desc_id	= get_id("GirlDescription");
	DetailLevel		= 0;
}

bool cScreenPrison::check_keys()
{
	if (g_UpArrow || (g_AltKeys && g_A_Key))
	{
		selection = ArrowUpListBox(prison_list_id);
		g_UpArrow = false;
		g_A_Key = false;
		//g_InitWin = true;
		SetSelectedItemInList(prison_list_id, selection);
		return true;
	}
	if (g_DownArrow || (g_AltKeys && g_D_Key))
	{
		selection = ArrowDownListBox(prison_list_id);
		g_DownArrow = false;
		g_D_Key = false;
		//g_InitWin = true;
		SetSelectedItemInList(prison_list_id, selection);
		return true;
	}
	return false;
}

void cScreenPrison::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) { set_ids(); }
/*
 *	handle arrow keys
 */
 	if(check_keys()) { return; }

	init();
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) { return; }

/*
 *	otherwise, compare event IDs 
 *
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) 
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
/*
 *	The More button to switch between girl details
 */
	if(g_InterfaceEvents.CheckButton(more_id)) 
	{
		more_button();
		//g_InitWin = true;
		return;
	}
/*
 *	The Release button to release selected girl from prison
 */
	if(g_InterfaceEvents.CheckButton(release_id))
	{
		release_button();
		g_InitWin = true;
		return;
	}
/*
 *	catch a selection change event for the listbox
 */
	if(g_InterfaceEvents.CheckListbox(prison_list_id))
	{
		selection = GetSelectedItemFromList(prison_list_id);
		selection_change();
		//g_InitWin = true;
		return;
	}
}

void cScreenPrison::selection_change()
{
	DisableButton(more_id, (selection == -1));
	DisableButton(release_id, (selection == -1));

	update_details();
}

void cScreenPrison::update_details()
{
	EditTextItem(gettext("No Prisoner Selected"), girl_desc_id);

	if(selection == -1) return;

	sGirl* pgirls = get_selected_girl();
	if(!pgirls) return;

	if (DetailLevel == 1)		EditTextItem(g_Girls.GetMoreDetailsString(pgirls, true), girl_desc_id);
	else if (DetailLevel == 2)	EditTextItem(g_Girls.GetThirdDetailsString(pgirls), girl_desc_id);
	else						EditTextItem(g_Girls.GetDetailsString(pgirls,true), girl_desc_id);
}

sGirl* cScreenPrison::get_selected_girl()
{
	if (selection == -1) return 0;

	sGirl* pgirls = g_Brothels.GetPrison();
	int i = 0;
	while (pgirls)
	{
		if (i == selection) break;
		i++;
		pgirls = pgirls->m_Next;
	}
	return pgirls;
}

void cScreenPrison::more_button()
{
	     if (DetailLevel == 0)	DetailLevel = 1;
	else if (DetailLevel == 1)	DetailLevel = 2;
	else						DetailLevel = 0;

	update_details();
}

void cScreenPrison::release_button()
{
	if(selection == -1) return;

	sGirl* pgirls = get_selected_girl();
	if(!pgirls) return;

	int cost = PrisonReleaseCost(pgirls);

	if(!g_Gold.afford((double)cost))
	{
		g_MessageQue.AddToQue(gettext("You don't have enough gold"), 0);
		return;
	}

	g_Gold.item_cost((double)cost);
	g_Gold.girl_support((double)cost);
	g_Brothels.RemoveGirlFromPrison(pgirls);
	if (g_Brothels.GetFreeRooms(g_CurrBrothel) < 1)
	{
		string text = pgirls->m_Realname;
		text += gettext(" has been sent to your dungeon, since current brothel is full.");
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.GetDungeon()->AddGirl(pgirls, DUNGEON_NEWGIRL);
	}
	else
	{
		string text = pgirls->m_Realname;
		text += gettext(" has been sent to your current brothel.");
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.AddGirl(g_CurrBrothel, pgirls);
	}
}

int cScreenPrison::PrisonReleaseCost(sGirl* girl)
{
	g_Girls.CalculateAskPrice(girl, false);
	int cost = g_Girls.GetStat(girl, STAT_ASKPRICE) * 15;
	cost += g_Girls.GetSkillWorth(girl);
	if (g_Girls.CheckVirginity(girl)) cost += int(cost / 2);	//	`J` fixed virgin adds half cost more
	cost *= 2;
	return cost;
}
