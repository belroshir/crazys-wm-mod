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
#include "cClinic.h"
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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Clinic - Surgery
bool cJobManager::WorkPhysicalSurgery(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	// if she was not in surgery last turn, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_PHYSICALSURGERY) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }

	bool hasDoctor = (g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, true) > 0 || g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, false) > 0);
	if (!hasDoctor)
	{
		ss << " does nothing. You don't have any Doctors working. (require 1) ";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}
	ss << " is in the Clinic to get general surgery.\n\n";

	int msgtype = Day0Night1;
	g_Girls.UnequipCombat(girl);	// not for patient

	if (Day0Night1 == SHIFT_DAY)	// the Doctor works on her durring the day
	{
		girl->m_WorkingDay++;
	}
	else	// and if there are nurses on duty, they take care of her at night
	{
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 0)
		{
			girl->m_WorkingDay++;
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, 10);
			g_Girls.UpdateStat(girl, STAT_MANA, 10);
		}
	}

	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, Day0Night1);

	if (girl->m_WorkingDay >= 5)
	{
		ss << "The surgery is a success.\n";
		msgtype = EVENT_GOODNEWS;
		if (!g_Girls.HasTrait(girl, "Sexy Air"))
		{
			girl->add_trait("Sexy Air", false);
			ss << "She gains Sexy Air trait.\n";
		}
		else if (!g_Girls.HasTrait(girl, "Cute"))
		{
			girl->add_trait("Cute", false);
			ss << "She gains Cute trait.\n";
		}
		if (numnurse > 1)
		{
			ss << "The Nurses kept her healthy and happy during her recovery.\n";
			g_Girls.UpdateStat(girl, STAT_SPIRIT, 5);
			g_Girls.UpdateStat(girl, STAT_MANA, 10);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 15);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 15);
		}
		else if (numnurse > 0)
		{
			ss << "The Nurse helped her during her recovery.\n";
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
			g_Girls.UpdateStat(girl, STAT_HEALTH, -10);
			g_Girls.UpdateStat(girl, STAT_MANA, -10);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 12);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 12);
		}
		else
		{
			ss << "She is sad and has lost some health during the operation.\n";
			g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -15);
			g_Girls.UpdateStat(girl, STAT_HEALTH, -20);
			g_Girls.UpdateStat(girl, STAT_MANA, -20);
			g_Girls.UpdateStat(girl, STAT_BEAUTY, 8);
			g_Girls.UpdateStat(girl, STAT_CHARISMA, 8);
		}

		if (g_Girls.HasTrait(girl, "Fragile")){ g_Girls.UpdateStat(girl, STAT_HEALTH, -5); }
		else if (g_Girls.HasTrait(girl, "Tough")){ g_Girls.UpdateStat(girl, STAT_HEALTH, 5); }
		if (g_Girls.HasTrait(girl, "Pessimist")){ g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5); }
		else if (g_Girls.HasTrait(girl, "Optimist")){ g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5); }

		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
	}
	else
	{
		int wdays = (5 - girl->m_WorkingDay);
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 0)
		{
			if (wdays >= 3)		{ wdays = 3; }
			else if (wdays > 1)	{ wdays = 2; }
			else				{ wdays = 1; }
		}
		ss << "The operation is in progress (" << wdays << " day remaining).\n";
		if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 1)		{ ss << "The Nurses are taking care of her at night."; }
		else if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, 1) > 0){ ss << "The Nurse is taking care of her at night."; }
		else							{ ss << "Having a Nurse on duty will speed up her recovery."; }
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);



	// Improve girl
	int libido = 1;
	if (g_Girls.HasTrait(girl, "Lesbian"))		libido += numnurse;
	if (g_Girls.HasTrait(girl, "Masochist"))	libido += 1;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	libido += 2;
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);
	if (g_Dice % 10 == 0)
		g_Girls.UpdateSkill(girl, SKILL_MEDICINE, 1);	// `J` she watched what the doctors and nurses were doing

	return false;
}


double cJobManager::JP_PhysicalSurgery(sGirl* girl, bool estimate)
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string - how much do they need this?
	{
		if (!g_Girls.HasTrait(girl, "Sexy Air"))	jobperformance += 100;
		if (!g_Girls.HasTrait(girl, "Cute"))		jobperformance += 100;
		jobperformance += (100 - girl->m_Stats[STAT_CHARISMA]);
		jobperformance += (100 - girl->m_Stats[STAT_BEAUTY]);
	}
	return jobperformance;
}
