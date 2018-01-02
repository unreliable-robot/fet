//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ROOM_H
#define ROOM_H

#include <QCoreApplication>

#include <QString>
#include <QList>

class Room;
class Rules;

typedef QList<Room*> RoomsList;

/**
This class represents a room

@author Liviu Lalescu
*/
class Room{ /*classroom :-)*/
	Q_DECLARE_TR_FUNCTIONS(Room)

public:
	QString name;
	int capacity;
	
	/**
	If empty string, it is ignored
	*/
	QString building;
	
	QString comments;
	
	/**
	Internal. If -1, it is ignored
	*/
	int buildingIndex;

	Room();
	~Room();
	
	void computeInternalStructure(const Rules &r);

	QString getXmlDescription() const;
	QString getDescription() const;
	QString getDetailedDescription() const;
	QString getDetailedDescriptionWithConstraints(const Rules& r) const;
};

bool roomsAscending(const Room* r1, const Room* r2);

#endif
