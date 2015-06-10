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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Brothel - Hall
bool cJobManager::WorkHallDealer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKHALL;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a dealer in the gambling hall.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	sGirl* enteronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_ENTERTAINMENT, Day0Night1);
	string entername = (enteronduty ? "Entertainer " + enteronduty->m_Realname + "" : "the Entertainer");

	sGirl* xxxenteronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_XXXENTERTAINMENT, Day0Night1);
	string xxxentername = (xxxenteronduty ? "Entertainer " + xxxenteronduty->m_Realname + "" : "the Sexual Entertainer");

	double wages = 25, tips = 0;
	int work = 0;

	int roll = g_Dice.d100();
	int jobperformance = (int)JP_HallDealer(girl, false);


	//a little pre-game randomness
	if (g_Dice.percent(10))
	{
		if (g_Girls.HasTrait(girl, "Strange Eyes"))
		{
			ss << " " << girlName << "'s strange eyes were somehow hypnotic, giving her some advantage.";
			jobperformance += 15;
		}
		if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Girls.GetStat(girl, STAT_LIBIDO) > 75)
		{
			ss << " " << girlName << " had very high libido, making it hard for her to concentrate.";
			jobperformance -= 10;
		}
		if (g_Girls.GetSkill(girl, SKILL_FOOTJOB) > 50)
		{
			ss << " " << girlName << " skillfully used her feet under the table to break customers' concentration.";
			jobperformance += 5;
		}
	}
	if (girl->is_addict(true) && g_Dice.percent(20))
	{
		ss << "\nNoticing her addiction, a customer offered her drugs. She accepted, and had an awful day at the card table.\n";
		if (g_Girls.HasTrait(girl, "Shroud Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Shroud Mushroom"));
		}
		if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Fairy Dust"));
		}
		if (g_Girls.HasTrait(girl, "Viras Blood Addict"))
		{
			g_Girls.AddInv(girl, g_InvManager.GetItem("Vira Blood"));
		}
		jobperformance -= 50;
	}


	if (jobperformance >= 245)
	{
		ss << " She's the perfect dealer. The customers love her and keep coming back to play against her, even after they lose the lose the shirts off their backs.\n\n";
		wages += 155;
		if (roll <= 33)
		{
			//SIN: Fixed - think this is all of them!
			if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
			{
				ss << "Between her exceptional card skills and her massive tits, " << girlName << " raked the money in this shift.\n";
			}
			else if (g_Girls.HasTrait(girl, "Lolita"))
			{
				ss << "Behind her small frame and innocent face lurks a true card-shark.\n";
			}
			else
			{
				ss << girlName << " is as near to perfect as any being could get.  She made a pile of money today.\n";
			}
		}
		else if (roll <= 66)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "Her sexy body draws gamblers to her table like flies to a pitcher plant.\n";
			}
			else
			{
				ss << girlName << " managed to win every game she played in today.\n";
			}
		}
		else
		{
			ss << "A master of card-counting, the other players had no chance when " << girlName << " joined them this shift.\n";
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always finding new ways to beat the customer.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			ss << girlName << "'s a skilled card dealer, and turned a substantial profit today.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << "'s sex appeal is paying off in a different way, as the profits from her table tumble in.\n";
			}
			else
			{
				ss << "She won all of her games bar one or two today!\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Quick Learner"))
			{
				ss << "After a good deal of practical education, " << girlName << " is a formidable card dealer.\n";
			}
			else
			{
				ss << girlName << " could find a place in any gambling institution with her skills with cards.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Cool Scars") || g_Girls.HasTrait(girl, "Horrific Scars"))
			{
				ss << "Distracted by her visible scars, customers couldn't keep up with her skills.\n";
			}
			else ss << "A fat merchant nearly had a heart attack after losing this month's profits to " << girlName << ".\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Lolita"))
			{
				ss << "Lured into a false sense of security, the gamblers were shocked to lose to such a child-like woman!\n";
			}
			else if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) //SIN: Fixed
			{
				ss << "Distracted by " << girlName << "'s breasts, players didn't even seem to notice their money vanishing.\n";
			}
			else
			{
				ss << "You flash a congratulatory smile at " << girlName << " on her way out the door at the end of her shift.\n";
			}
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and knows a few tricks to win.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << girlName << "'s a fairly good card dealer, and turned a profit today.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Lolita"))
			{
				ss << "Nobody expected such a sweet little girl to win anything!\n";
			}
			else if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) //SIN: Fixed
			{
				ss << "While she's a good card dealer, " << girlName << "'s big tits helped weigh the odds in her favor.\n";
			}
			else
			{
				ss << "Her professional smile and pleasing form reinforced her acceptable skill level.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Quick Learner"))
			{
				ss << "Using tricks learned before from her past, " << girlName << " had a productive shift.\n";
			}
			else
			{
				ss << " Lady Luck seems to be smiling on " << girlName << " today - she won more games then she lost.\n";
			}
		}
		else if (roll <= 80)
		{
			ss << "Most of the patrons that sat down at " << girlName << "'s table today rose just a bit lighter.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "The gamblers always seem surprised that such a lovely piece of ass can beat them at their chosen game.\n";
			}
			else
			{
				ss << girlName << " shows real promise as a dealer.\n";
			}
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
			{
				ss << "Despite her uncertain nature, " << girlName << " is holding her own at the card-table.\n";
			}
			else
			{
				ss << "She's no cardsharp, but " << girlName << " can hold her own against the patrons.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Quick Learner"))
			{
				ss << "She could be a good dealer, but " << girlName << " has a lot to learn still.\n";
			}
			else
			{
				ss << girlName << "broke even today, thank the Lady.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << " isn't a terrible card dealer, but she's much more eye-candy then gambling queen.\n";
			}
			else
			{
				ss << "Pasteboard isn't her friend as the cards seemed to taunt her.\n";
			}
		}
		else if (roll <= 80)
		{
			ss << "Almost all the patrons managed to preserve most of their initial stake.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) //SIN: Fixed
			{
				ss << "She turned a slight profit, with the help of her not inconsiderable breasts' distraction factor.\n";
			}
			else
			{
				ss << "She's clocked in and clocked out, but nothing spectacular has happened in between.\n";
			}
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Quick Learner"))
			{
				ss << "She's got a clue, but still has a long way to go to reach competency.\n";
			}
			else
			{
				ss << girlName << " struggles valiantly against the forces of chance, and wins! A. Single. Game.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
			{
				ss << girlName << "'s weak personality made it easy for clients to bully her out of money.\n";
			}
			else
			{
				ss << "Despite her feeble protests, gamblers walked all over " << girlName << ".\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) > 70)
			{
				ss << girlName << " is smart enough to understand the game. But seems not to have the luck to win.\n";
			}
			else
			{
				ss << "As you watch " << girlName << " fold like a house of cards on a royal flush, you idly wonder if she could be replaced with a shaved ape.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) //SIN: Fixed
			{
				ss << "While players were distracted by " << girlName << "'s breasts for a few turns, she still lost more then she won.\n";
			}
			else
			{
				ss << "The cards are not in her favor today - the highest hand you saw her with was two pair.\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << " could make a corpse stand up and beg for a blow-job, but she can't play cards worth a damn.\n";
			}
			else
			{
				ss << "As " << girlName << "'s shift ends, you struggle mightily against the urge to sigh in relief.\n";
			}
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "It's almost a pity how attractive" << girlName << " is.  If she wasn't so desirable, fewer vultures would alight on her table.\n";
			}
			else
			{
				ss << girlName << " dropped the deck on the floor, spraying cards everywhere.\n";
			}
		}
		else if (roll <= 40)
		{
			ss << girlName << " managed, against all probability, to lose every single game.\n";
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
				|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Big Boobs")
				|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")) //SIN: Fixed
			{
				ss << girlName << "'s large breasts pleased the clients as they won over and over again.\n";
			}
			else
			{
				ss << girlName << " shrugged with a degree of embarrassment as a chortling patron walked away with a fat moneybag.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
			{
				ss << girlName << "'s weak personality made it easy for clients to bully her out of money.\n";
			}
			else
			{
				ss << girlName << " is really, really, bad at this job.\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Quick Learner"))
			{
				ss << "After a terrible shift, you can only hope that she learned something from it.\n";
			}
			else
			{
				ss << "You can almost see the profits slipping away as " << girlName << " loses yet another hand of poker.\n";
			}
		}
	}
	
	//I'm not aware of tipping card dealers being a common practice, so no base tips


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; tips += 25;
	}

	//SIN: Fixed - add all traits and moved dice roll to start so that if this returns false, the bulky bit won't be evaluated (will be short-circuited)
	if (g_Dice.percent(15) && (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs")
		|| g_Girls.HasTrait(girl, "Titanic Tits") || g_Girls.HasTrait(girl, "Massive Melons")
		|| g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs")))
	{
		if (jobperformance < 150)
		{
			ss << "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n";
		}
		else
		{
			ss << "A patron was staring obviously at her large breasts. So she used the chance to cheat him out of all his gold.\n";  wages += 35;
		}
	}

	if (g_Girls.HasTrait(girl, "Lolita") && g_Dice.percent(15))
	{
		if (jobperformance < 125)
		{
			ss << "Furious at being outplayed by such a young girl, a couple of gamblers stormed out, and didn't give " << girlName << " any tips.\n";
		}
		else
		{
			ss << "One of the gamblers was amused at being outplayed by such a young girl, and gave her an extra-large tip!\n"; tips += 15;
		}
	}

	//SIN - typo on "Elegant" which would cause it to always fail - fixed.
	//ANDs (&&) are processed before ORs (||) due to higher precedence. So this will be processed as:
	//					do it if (she's elegant [always]), or (she's a princess [always]) or (she's a queen [only 15% of the time])
	//Don't think this was intended, so re-writing to force the roll to apply to all traits
	if (g_Dice.percent(15) && (g_Girls.HasTrait(girl, "Elegant") || g_Girls.HasTrait(girl, "Princess") || g_Girls.HasTrait(girl, "Queen")))
	{
		if (jobperformance < 150)
		{
			ss << "Surly at her apparently stuck-up attitude, several gamblers refused to tip " << girlName << ".\n";
		}
		else
		{
			ss << "Impressed by her elegant demeanor and graceful compartment, several gamblers gave " << girlName << " larger tips then usual.\n";  tips += 20;
		}
	}

	if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
	{
		if (jobperformance < 150)
		{
			ss << "She decided a patron was cheating so she killed him causing a paninc of people running out with your money.\n";  wages -= 50;
		}
		else
		{
			ss << "She thought a patron was cheating but decided it was a lucky streak that she would end with her card skills.\n";
		}
	}

	if (g_Girls.HasTrait(girl, "Psychic") && g_Dice.percent(20))
	{
		ss << "She used her Psychic skills to know exactly what cards were coming up and won a big hand.\n"; wages += 30;
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_ENTERTAINMENT, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " wasn't good enough at her job to use " << entername << "'s distraction to make more money.\n";
		}
		else
		{
			ss << girlName << " used " << entername << "'s distraction to make you some extra money.\n"; wages += 25;
		}
	}

	//SIN: a bit more randomness
	if (g_Dice.percent(20) && wages < 20 && g_Girls.GetStat(girl, STAT_CHARISMA) > 60)
	{
		ss << girlName << " did so badly, a customer felt sorry for her and left her a few coins from his winnings.\n";
		wages += ((g_Dice % 18) + 3);
	}
	if (g_Dice.percent(5) && g_Girls.GetSkill(girl, SKILL_NORMALSEX) > 50 && g_Girls.GetStat(girl, STAT_FAME) > 30)
	{
		ss << "A customer taunted " << girlName << ", saying the best use for a dumb whore like her is bent over the gambling table.";
		bool spirited = (g_Girls.GetStat(girl, STAT_SPIRIT) + g_Girls.GetStat(girl, STAT_SPIRIT) > 80);
		if (spirited)
		{
			ss << "\n\"But this way\"" << girlName << " smiled, \"I can take your money, without having to try and find your penis.\"";
		}
		else
		{
			ss << "She didn't acknowledge it in any way, but inwardly determined to beat him.";
		}
		if (jobperformance >= 145)
		{
			ss << "\nShe cleaned him out, deliberately humiliating him and taunting him into gambling more than he could afford. ";
			ss << "He ended up losing every penny and all his clothes to this 'dumb whore'. He was finally kicked out, naked into the streets.\n\n";
			ss << girlName << " enjoyed this. A lot.";
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, 3);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5);
			wages += 100;
		}
		else if (jobperformance >= 99)
		{
			ss << "\nShe managed to hold her own, and in the end was just happy not to lose to a guy like this.";
		}
		else
		{
			ss << "\nSadly her card skills let her down and he beat her in almost every hand. He finally stood up pointing at the table:";
			ss << "\n\"If you wanna make your money back, whore, you know what to do.\"";
			if (spirited)
			{
				ss << "\"Bend over it then,\" she scowled. \"I'll show you where you can shove those gold coins.\"\nHe left laughing.";
			}
			else
			{
				ss << "\"I'm not doing that today, sir,\" she mumbled. \"But there are other girls.\"\nHe left for the brothel.";
			}
			ss << "\n\nShe really hated losing at this stupid card game.";
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, -3);
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -5);
			wages -= 50;
		}
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_XXXENTERTAINMENT, false) >= 1)
	{
		if (jobperformance < 125)
		{
			if (!g_Girls.HasTrait(girl, "Straight"))
			{
				if (g_Girls.GetStat(girl, STAT_LIBIDO) > 90)
				{
					ss << girlName << " found herself looking at " << xxxentername << "'s performance often, losing more times than usual.\n";
					wages *= 0.9;
				}
				else
				{
					ss << girlName << " wasn't good enough at her job to use " << xxxentername << "'s distraction to make more money.\n";
				}
			}
			else
			{
				ss << girlName << " wasn't good enough at her job to use " << xxxentername << "'s distraction to make more money.\n";
			}
		}
		else
		{
			ss << girlName << " took advantage of " << xxxentername << "'s show to win more hands and make some extra money.\n";
			wages *= 1.2;
		}
	}

	if (wages < 0) wages = 0;
	if (tips < 0) tips = 0;


	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_CARD, Day0Night1);

	// work out the pay between the house and the girl
	wages += (g_Dice % ((int)(((g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA)) / 2)*0.5f))) + 10;
	girl->m_Pay = (int)wages;
	girl->m_Tips = (int)tips;



	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }
	if (!g_Girls.HasTrait(girl, "Straight"))	{ libido += min(3, g_Brothels.GetNumGirlsOnJob(0, JOB_XXXENTERTAINMENT, false)); }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	int gain = g_Dice % 3;
	/* */if (gain == 0)	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else if (gain == 1)	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice%skill);
	else /*          */	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_HallDealer(sGirl* girl, bool estimate)
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 + 	// intel makes her smart enough to know when to cheat
		g_Girls.GetStat(girl, STAT_AGILITY) / 2 +			// agility makes her fast enough to cheat
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +	// performance helps her get away with it
		g_Girls.GetSkill(girl, SKILL_SERVICE) / 2);

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))    jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))    jobperformance += 5; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		  jobperformance += 10; //people like charming people
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 15;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 10; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 15;

	if (g_Girls.HasTrait(girl, "One Arm"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "One Foot"))		jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "One Hand"))		jobperformance -= 30; 
	if (g_Girls.HasTrait(girl, "One Leg"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "No Arms"))		jobperformance -= 150;
	if (g_Girls.HasTrait(girl, "No Feet"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "No Hands"))		jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "No Legs"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Blind"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Deaf"))			jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}