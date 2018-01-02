/***************************************************************************
                          modifyconstraintsubjectpreferredroomform.cpp  -  description
                             -------------------
    begin                : 8 Apr 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include <QMessageBox>

#include "modifyconstraintsubjectpreferredroomform.h"
#include "spaceconstraint.h"

ModifyConstraintSubjectPreferredRoomForm::ModifyConstraintSubjectPreferredRoomForm(QWidget* parent, ConstraintSubjectPreferredRoom* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	updateSubjectsComboBox();
	updateRoomsComboBox();
}

ModifyConstraintSubjectPreferredRoomForm::~ModifyConstraintSubjectPreferredRoomForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintSubjectPreferredRoomForm::updateSubjectsComboBox()
{
	subjectsComboBox->clear();
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* s=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(s->name);
	}
	subjectsComboBox->setCurrentText(this->_ctr->subjectName);
}

void ModifyConstraintSubjectPreferredRoomForm::updateRoomsComboBox()
{
	int i=0, j=-1;
	roomsComboBox->clear();
	for(int k=0; k<gt.rules.roomsList.size(); k++){
		Room* rm=gt.rules.roomsList[k];
		roomsComboBox->addItem(rm->name);
		if(rm->name==this->_ctr->roomName)
			j=i;
		i++;
	}
	assert(j>=0);
	roomsComboBox->setCurrentIndex(j);
}

void ModifyConstraintSubjectPreferredRoomForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int i=subjectsComboBox->currentIndex();
	if(i<0 || subjectsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid subject"));
		return;
	}
	QString subject=subjectsComboBox->currentText();

	i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	QString room=roomsComboBox->currentText();

	this->_ctr->weightPercentage=weight;
	this->_ctr->roomName=room;
	this->_ctr->subjectName=subject;

	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);
	
	this->close();
}
