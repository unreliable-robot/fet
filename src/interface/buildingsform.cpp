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

#include "timetable_defs.h"
#include "fet.h"
#include "buildingsform.h"
#include "addbuildingform.h"
#include "modifybuildingform.h"
#include "interface/editcommentsform.h"

#include <QMessageBox>
#include "centerwidgetonscreen.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

BuildingsForm::BuildingsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentBuildingTextEdit->setReadOnly(true);

	modifyBuildingPushButton->setDefault(true);

	buildingsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addBuildingPushButton, SIGNAL(clicked()), this, SLOT(addBuilding()));
	connect(removeBuildingPushButton, SIGNAL(clicked()), this, SLOT(removeBuilding()));
	connect(buildingsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(buildingChanged(int)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(modifyBuildingPushButton, SIGNAL(clicked()), this, SLOT(modifyBuilding()));

	connect(moveBuildingUpPushButton, SIGNAL(clicked()), this, SLOT(moveBuildingUp()));
	connect(moveBuildingDownPushButton, SIGNAL(clicked()), this, SLOT(moveBuildingDown()));

	connect(sortBuildingsPushButton, SIGNAL(clicked()), this, SLOT(sortBuildings()));
	connect(buildingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyBuilding()));

	connect(commentsPushButton, SIGNAL(clicked()), this, SLOT(comments()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings;
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}

BuildingsForm::~BuildingsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings;
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool BuildingsForm::filterOk(Building* bu)
{
	Q_UNUSED(bu);

	bool ok=true;

	return ok;
}

void BuildingsForm::filterChanged()
{
	QString s;
	buildingsListWidget->clear();
	visibleBuildingsList.clear();
	for(int i=0; i<gt.rules.buildingsList.size(); i++){
		Building* bu=gt.rules.buildingsList[i];
		if(this->filterOk(bu)){
			s=bu->name;
			visibleBuildingsList.append(bu);
			buildingsListWidget->addItem(s);
		}
	}
	
	if(buildingsListWidget->count()>0)
		buildingsListWidget->setCurrentRow(0);
	else
		buildingChanged(-1);
}

void BuildingsForm::addBuilding()
{
	AddBuildingForm addBuildingForm(this);
	addBuildingForm.exec();
	
	filterChanged();
	
	buildingsListWidget->setCurrentRow(buildingsListWidget->count()-1);
}

void BuildingsForm::removeBuilding()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ind);
	assert(bu!=NULL);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this building?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;
		
	bool tmp=gt.rules.removeBuilding(bu->name);
	assert(tmp);
	
	visibleBuildingsList.removeAt(ind);
	buildingsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=buildingsListWidget->takeItem(ind);
	delete item;
	
	if(ind>=buildingsListWidget->count())
		ind=buildingsListWidget->count()-1;
	if(ind>=0)
		buildingsListWidget->setCurrentRow(ind);
	else
		currentBuildingTextEdit->setPlainText(QString(""));
}

void BuildingsForm::buildingChanged(int index)
{
	if(index<0){
		currentBuildingTextEdit->setPlainText("");
		return;
	}

	QString s;
	Building* building=visibleBuildingsList.at(index);

	assert(building!=NULL);
	s=building->getDetailedDescriptionWithConstraints(gt.rules);
	currentBuildingTextEdit->setPlainText(s);
}

void BuildingsForm::moveBuildingUp()
{
	if(buildingsListWidget->count()<=1)
		return;
	int i=buildingsListWidget->currentRow();
	if(i<0 || i>=buildingsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=buildingsListWidget->item(i)->text();
	QString s2=buildingsListWidget->item(i-1)->text();
	
	buildingsListWidget->item(i)->setText(s2);
	buildingsListWidget->item(i-1)->setText(s1);
	
	gt.rules.buildingsList.swap(i, i-1);
	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);

	visibleBuildingsList.swap(i, i-1);
	buildingsListWidget->setCurrentRow(i-1);
}

void BuildingsForm::moveBuildingDown()
{
	if(buildingsListWidget->count()<=1)
		return;
	int i=buildingsListWidget->currentRow();
	if(i<0 || i>=buildingsListWidget->count())
		return;
	if(i==buildingsListWidget->count()-1)
		return;
		
	QString s1=buildingsListWidget->item(i)->text();
	QString s2=buildingsListWidget->item(i+1)->text();
	
	buildingsListWidget->item(i)->setText(s2);
	buildingsListWidget->item(i+1)->setText(s1);
	
	gt.rules.buildingsList.swap(i, i+1);
	gt.rules.internalStructureComputed=false;
	gt.rules.setModified(true);

	visibleBuildingsList.swap(i, i+1);
	buildingsListWidget->setCurrentRow(i+1);
}

void BuildingsForm::sortBuildings()
{
	gt.rules.sortBuildingsAlphabetically();

	filterChanged();
}

void BuildingsForm::modifyBuilding()
{
	int valv=buildingsListWidget->verticalScrollBar()->value();
	int valh=buildingsListWidget->horizontalScrollBar()->value();

	int ci=buildingsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ci);
	ModifyBuildingForm form(this, bu->name);
	form.exec();

	filterChanged();
	
	buildingsListWidget->verticalScrollBar()->setValue(valv);
	buildingsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=buildingsListWidget->count())
		ci=buildingsListWidget->count()-1;

	if(ci>=0)
		buildingsListWidget->setCurrentRow(ci);
}

void BuildingsForm::comments()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=gt.rules.buildingsList[ind];
	assert(bu!=NULL);

	EditCommentsForm dialog("BuildingsCommentsDialog", this, tr("Building comments"));
	dialog.setComments(bu->comments);
	
	int t=dialog.exec();
	
	if(t==QDialog::Accepted){
		bu->comments=dialog.getComments();
	
		gt.rules.internalStructureComputed=false;
		gt.rules.setModified(true);

		buildingChanged(ind);
	}
}
