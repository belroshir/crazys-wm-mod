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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cCentre.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;
extern cPlayer* The_Player;

// `J` Job Centre - General
bool cJobManager::WorkComunityService(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCENTRE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked doing comunity service.\n\n";

	g_Building = BUILDING_CENTRE;

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int image = IMGTYPE_PROFILE;
	bool blow = false, sex = false;
	int dispo = 0;
	int roll = g_Dice.d100();
	int wages = 100, work = 0, help = 0;
	double jobperformance = JP_ComunityService(girl, false);

	//Adding cust here for use in scripts...
	sCustomer cust;
	GetMiscCustomer(brothel, cust);

	// `J` merged slave/free messages and moved actual dispo change to after
	/* */if (jobperformance >= 245)
	{
		dispo = 12;
		ss << " She must be perfect at this.\n\n";
	}
	else if (jobperformance >= 185)
	{
		dispo = 10;
		ss << " She's unbelievable at this and is always getting praised by people for her work.\n\n";
	}
	else if (jobperformance >= 145)
	{
		dispo = 8;
		ss << " She's good at this job and gets praised by people often.\n\n";
	}
	else if (jobperformance >= 100)
	{
		dispo = 6;
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
	}
	else if (jobperformance >= 70)
	{
		dispo = 4;
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
	}
	else
	{
		dispo = 2;
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
	}



	//try and add randomness here
	if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Dice.percent(30) && !g_Girls.HasTrait(girl, "Virgin")
		&& !g_Girls.HasTrait(girl, "Lesbian") && g_Girls.GetStat(girl, STAT_LIBIDO) > 75
		&& !brothel->m_RestrictNormal)
	{
		sex = true;
		ss << "Her Nymphomania got the better of her today and she decide the best way to services her community was on her back!\n";
	}

	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 55 && g_Dice.percent(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
	{
		blow = true;
		ss << "An elderly fellow managed to convince " << girlName << " that the best way to serve her community was on her knees. She ended up giving him a blow job!\n\n";
	}


	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, actiontype, work);


	if (sex)
	{
		if (roll <= 50 && !brothel->m_RestrictNormal)
		{
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
			image = IMGTYPE_SEX;
			if (g_Girls.CheckVirginity(girl))
			{
				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
				ss << "She is no longer a virgin.\n";
			}
			if (!girl->calc_pregnancy(&cust, false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
			}
		}
		else if (!brothel->m_RestrictAnal)
		{
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2); image = IMGTYPE_ANAL;
		}
		brothel->m_Happiness += 100;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3);
		dispo += 6;
	}
	else if (blow)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		dispo += 4;
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
		image = IMGTYPE_ORAL;
	}

	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
	}
	else
	{
		ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
		girl->m_Pay = wages;
		g_Gold.staff_wages(100);  // wages come from you
		dispo = int(dispo*1.5);
	}

	The_Player->disposition(dispo);
	girl->m_Events.AddMessage(ss.str(), image, Day0Night1);

	help += (int)(jobperformance / 10);		//  1 helped per 10 point of performance

	ss.str("");
	ss << girlName << " helped " << help << " people today.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);



	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2 == 1)	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else				g_Girls.UpdateStat(girl, STAT_CHARISMA, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_ComunityService(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetStat(girl, STAT_CHARISMA) / 2 +
		g_Girls.GetSkill(girl, SKILL_SERVICE));



	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;	//people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming")) 		jobperformance += 15;	//people like charming people
	if (g_Girls.HasTrait(girl, "Optimist"))			jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50;	//needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20;	//spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20;	//gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30;	//don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;

	return jobperformance;
}
