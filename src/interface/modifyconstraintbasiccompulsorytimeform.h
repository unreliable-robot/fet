/***************************************************************************
                          modifyconstraintbasiccompulsorytimeform.h  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTBASICCOMPULSORYTIMEFORM_H
#define MODIFYCONSTRAINTBASICCOMPULSORYTIMEFORM_H

#include "ui_modifyconstraintbasiccompulsorytimeform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintBasicCompulsoryTimeForm : public QDialog, Ui::ModifyConstraintBasicCompulsoryTimeForm_template  {
	Q_OBJECT
public:
	ConstraintBasicCompulsoryTime* _ctr;

	ModifyConstraintBasicCompulsoryTimeForm(QWidget* parent, ConstraintBasicCompulsoryTime* ctr);
	~ModifyConstraintBasicCompulsoryTimeForm();

public slots:
	void ok();
};

#endif
