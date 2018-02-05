/***************************************************************************
                          timetableviewroomstimehorizontalform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWROOMSTIMEHORIZONTALFORM_H
#define TIMETABLEVIEWROOMSTIMEHORIZONTALFORM_H

class QColor; //by Marco Vassura

#include "ui_timetableviewroomstimehorizontalform_template.h"

#include "timetabletimehorizontalitemdelegate.h"

class TimetableViewRoomsTimeHorizontalForm : public QDialog, public Ui::TimetableViewRoomsTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	TimetableTimeHorizontalItemDelegate* newItemDelegate;

public:
	TimetableViewRoomsTimeHorizontalForm(QWidget* parent);
	~TimetableViewRoomsTimeHorizontalForm();
	
	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateRoomsTimetableTable();

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();
	
	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

protected:
	QColor stringToColor(QString s); //by Marco Vassura
};

#endif
