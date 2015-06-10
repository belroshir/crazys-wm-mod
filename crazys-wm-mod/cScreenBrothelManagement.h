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
#include "cInterfaceWindow.h"

class cScreenBrothelManagement : public cInterfaceWindowXML
{
	void set_ids();
	
	static bool id_set;

	int id_header;
	int id_details;
	int id_image;
	int id_girls;
	int id_staff;
	int id_setup;
	int id_dungeon;
	int id_town;
	int id_turn;
	int id_week;
	int id_save;
	int id_quit;
	int id_prev;
	int id_next;

public:

	cScreenBrothelManagement();
	~cScreenBrothelManagement();

	void init();
	void process();
	void check_events();
	bool check_keys();

	void Free() { cInterfaceWindowXML::Free(); }
};

