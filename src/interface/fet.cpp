/*
File fet.cpp - this is where the program FET starts
*/

/***************************************************************************
                          fet.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Lalescu Liviu
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

#include "fet.h"

#include "matrix.h"

#include "messageboxes.h"

#include "randomknuth.h"

#ifndef FET_COMMAND_LINE
#include <QMessageBox>

#include <QWidget>
#endif

#include <QStandardPaths>

#include <QLocale>
#include <QTime>
#include <QDate>
#include <QDateTime>

#include <QSet>

static QSet<QString> languagesSet;

#include <ctime>
#include <cstdlib>

#include "timetableexport.h"
#include "generate.h"

#include "timetable_defs.h"
#include "timetable.h"

#ifndef FET_COMMAND_LINE
#include "fetmainform.h"

#include "helpaboutform.h"
#include "helpfaqform.h"
#include "helptipsform.h"
#include "helpinstructionsform.h"

#include "timetableshowconflictsform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetableviewstudentstimehorizontalform.h"
#include "timetableviewteachersdayshorizontalform.h"
#include "timetableviewteacherstimehorizontalform.h"
#include "timetableviewroomsdayshorizontalform.h"
#include "timetableviewroomstimehorizontalform.h"

#include "fetguisettings.h"
#endif

#include <QCoreApplication>

#ifndef FET_COMMAND_LINE
#include <QApplication>

#include <QSettings>
#include <QRect>
#endif

#include <QMutex>
#include <QString>
#include <QTranslator>

#include <QDir>

#include <QTextStream>
#include <QFile>

#include <csignal>

#include <iostream>
using namespace std;

extern int initialOrderOfActivitiesIndices[MAX_ACTIVITIES];

#ifndef FET_COMMAND_LINE
extern QMutex myMutex;
#else
QMutex myMutex;
#endif

QTranslator translator;

/**
The one and only instantiation of the main class.
*/
Timetable gt;

/**
The name of the file from where the rules are read.
*/
QString INPUT_FILENAME_XML;

/**
The working directory
*/
QString WORKING_DIRECTORY;

/**
The import directory
*/
QString IMPORT_DIRECTORY;

#ifndef FET_COMMAND_LINE
QApplication* pqapplication=NULL;
#endif

#ifdef FET_COMMAND_LINE
#include "export.h"
#endif

//for command line version, if the user stops using a signal
#ifdef FET_COMMAND_LINE

Generate* terminateGeneratePointer = NULL;

static void terminate(int param)
{
	Q_UNUSED(param);

	assert(terminateGeneratePointer!=NULL);
	
	terminateGeneratePointer->abort();
}

static void usage(QTextStream* out, const QString& error)
{
	QString s="";
	
	s+=QString("Incorrect command-line parameters (%1).").arg(error);
	
	s+="\n\n";
	
	s+=QString(
		"Command line usage: \"fet-cl --inputfile=x [--outputdir=d] [--timelimitseconds=y] [--htmllevel=z] [--language=t] "
		"[--writetimetableconflicts=wt1] "
		"[--writetimetablesstatistics=wt2] "
		"[--writetimetablesxml=wt3] "
		"[--writetimetablesdayshorizontal=wt4] "
		"[--writetimetablesdaysvertical=wt5] "
		"[--writetimetablestimehorizontal=wt6] "
		"[--writetimetablestimevertical=wt7] "
		"[--writetimetablessubgroups=wt8] "
		"[--writetimetablesgroups=wt9] "
		"[--writetimetablesyears=wt10] "
		"[--writetimetablesteachers=wt11] "
		"[--writetimetablesteachersfreeperiods=wt12] "
		"[--writetimetablesrooms=wt13] "
		"[--writetimetablessubjects=wt14] "
		"[--writetimetablesactivities=wt15] "
		"[--printactivitytags=a] [--printnotavailable=u] [--printbreak=b] [--dividetimeaxisbydays=v] [--duplicateverticalheaders=e] "
		"[--printsimultaneousactivities=w] [--randomseedx=rx --randomseedy=ry] [--warnifusingnotperfectconstraints=s] "
		"[--warnifusingstudentsminhoursdailywithallowemptydays=p] [--warnifusinggroupactivitiesininitialorder=g] [--warnsubgroupswiththesameactivities=ssa]\n"
		"[--printdetailedtimetables=pdt] [--printdetailedteachersfreeperiodstimetables=pdtfp] "
		"[--exportcsv=ecsv] [--overwritecsv=ocsv] [--firstlineisheadingcsv=flhcsv] [--quotescsv=qcsv] [--fieldseparatorcsv=fscsv] "
		"[--verbose=r]\",\n"
		"where:\nx is the input file, for instance \"data.fet\"\n"
		"d is the path to results directory, without trailing slash or backslash (default is current working path). "
		"Make sure you have write permissions there.\n"
		"y is integer (seconds) (default 2000000000, which is practically infinite).\n"
		"z is integer from 0 to 7 and represents the detail level for the generated HTML timetables "
		"(default 2, larger values have more details/facilities and larger file sizes).\n"
		"t is one of ar, ca, cs, da, de, el, en_GB, en_US, es, eu, fa, fr, gl, he, hu, id, it, ja, lt, mk, ms, nl, pl, pt_BR, ro, ru, si, sk, sq, sr, tr, "
		"uk, uz, vi, zh_CN, zh_TW (default en_US).\n"
		"wt1 to wt15 are either true or false and represent whether you want the corresponding timetables to be written on the disk (default true).\n"
		"a is either true or false and represets if you want activity tags to be present in the final HTML timetables (default true).\n"
		"u is either true or false and represents if you want -x- (for true) or --- (for false) in the generated timetables for the "
		"not available slots (default true).\n"
		"b is either true or false and represents if you want -X- (for true) or --- (for false) in the generated timetables for the "
		"break slots (default true).\n"
		"v is either true or false, represents if you want the HTML timetables with time-axis divided by days (default false).\n"
		"e is either true or false, represents if you want the HTML timetables to duplicate vertical headers to the right of the tables, for easier reading (default false).\n"
		"w is either true or false, represents if you want the HTML timetables to show related activities which have constraints with same starting time (default false).\n"
		"(for instance, if A1 (T1, G1) and A2 (T2, G2) have constraint activities same starting time, then in T1's timetable will appear also A2, at the same slot "
		"as A1).\n"
		"rx is the random seed X component, minimum 1 to maximum 2147483646, ry is the random seed Y component, minimum 1 to maximum 2147483398"
		" (you can get the same timetable if the input file is identical, if the FET version is the same and if the random seed X and Y components are the same).\n"
		"s is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains not perfect constraints "
		"(activity tag max hours daily or students max gaps per day) (default true).\n"
		"p is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains nonstandard constraints "
		"students min hours daily with allow empty days (default true).\n"
		"g is either true or false, represents whether you want a message box to be shown, with a warning, if the input file contains nonstandard timetable "
		"generation options to group activities in the initial order (default true).\n"
		"ssa is either true or false, represents whether you want a message box to be show, with a warning, if your input file contains subgroups which have "
		"the same activities (default true).\n"
		"pdt is either true or false, represents whether you want to show the detailed (true) or less detailed (false) years and groups timetables (default true).\n"
		"pdtfp is either true or false, represents whether you want to show the detailed (true) or less detailed (false) teachers free periods timetables (default true).\n"
		"ecsv is either true or false, represents whether you want to export the CSV file and timetables (default false).\n"
		"ocsv is either true or false, represents whether you want to overwrite old CSV files, if they exist (default false).\n"
		"flhcsv is either true or false, represents whether you want the heading of the CSV files to be header (true) or data (false). The default value is true.\n"
		"qcsv is one value from the set [doublequotes|singlequotes|none] (write a single value from these three exactly as it is written here). The default value is "
		"doublequotes.\n"
		"fscsv is one value from the set [comma|semicolon|verticalbar] (write a single value from these three exactly as it is written here). The default value is "
		"comma.\n"
		"r is either true or false, represents whether you want additional generation messages and other messages to be shown on the command line (default false).\n"
		"Alternatively, you can run \"fet-cl --version [--outputdir=d]\" to get the current FET version. "
		"where:\nd is the path to results directory, without trailing slash or backslash (default is current working path). "
		"Make sure you have write permissions there.\n"
		"(If you specify the --version argument, FET just prints version number on the command line prompt and in the output directory and exits.)\n"
		"\n"
		"You can ask the FET command line process to stop the timetable generation, by sending it the SIGTERM signal. "
		"FET will then write the current timetable and the highest stage timetable and exit. "
		"(If you send the FET command line the SIGKILL signal it will stop immediately, without writing the timetable.)"
	);
	
	cout<<qPrintable(s)<<endl;
	if(out!=NULL)
		(*out)<<qPrintable(s)<<endl;
}

static void renderErrorList(const ErrorList& errors) {
	foreach (ErrorCode erc, errors)
		if (erc)
			IrreconcilableCriticalMessage::critical((QWidget*)NULL, erc.getSeverityTitle(), erc.message);
}

#endif

#ifndef FET_COMMAND_LINE
static void readSimulationParameters()
{
	const QString predefDir=QDir::homePath()+FILE_SEP+"fet-results";

	QSettings newSettings;

	if(newSettings.contains("output-directory")){
		OUTPUT_DIR=newSettings.value("output-directory").toString();
		QDir dir;
		if(!dir.exists(OUTPUT_DIR)){
			bool t=dir.mkpath(OUTPUT_DIR);
			if(!t){
				QMessageBox::warning(NULL, FetTranslate::tr("FET warning"), FetTranslate::tr("Output directory %1 does not exist and cannot be"
				 " created - output directory will be made the default value %2")
				 .arg(QDir::toNativeSeparators(OUTPUT_DIR)).arg(QDir::toNativeSeparators(predefDir)));
				OUTPUT_DIR=predefDir;
			}
		}
	}
	else{
		OUTPUT_DIR=predefDir;
	}

#ifndef USE_SYSTEM_LOCALE
	FET_LANGUAGE=newSettings.value("language", "en_US").toString();
#else
	if(newSettings.contains("language")){
		FET_LANGUAGE=newSettings.value("language").toString();
	}
	else{
		FET_LANGUAGE=QLocale::system().name();

		bool ok=false;
		foreach(QString s, languagesSet){
			if(FET_LANGUAGE.left(s.length())==s){
				FET_LANGUAGE=s;
				ok=true;
				break;
			}
		}
		if(!ok)
			FET_LANGUAGE="en_US";
	}
#endif
	
	WORKING_DIRECTORY=newSettings.value("working-directory", "examples").toString();
	IMPORT_DIRECTORY=newSettings.value("import-directory", OUTPUT_DIR).toString();
	
	QDir d(WORKING_DIRECTORY);
	if(!d.exists())
		WORKING_DIRECTORY="examples";
	QDir d2(WORKING_DIRECTORY);
	if(!d2.exists())
		WORKING_DIRECTORY=QDir::homePath();
	else
		WORKING_DIRECTORY=d2.absolutePath();

	QDir i(IMPORT_DIRECTORY);
	if(!i.exists())
		IMPORT_DIRECTORY=OUTPUT_DIR;
	
	checkForUpdates=newSettings.value("check-for-updates", "false").toBool();

	QString ver=newSettings.value("version", "-1").toString();
	
	TIMETABLE_HTML_LEVEL=newSettings.value("html-level", "2").toInt();
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=newSettings.value("print-activity-tags", "true").toBool();
	PRINT_DETAILED_HTML_TIMETABLES=newSettings.value("print-detailed-timetables", "true").toBool();
	PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=newSettings.value("print-detailed-teachers-free-periods-timetables", "true").toBool();
	PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=newSettings.value("print-activities-with-same-starting-time", "false").toBool();
	PRINT_NOT_AVAILABLE_TIME_SLOTS=newSettings.value("print-not-available", "true").toBool();
	PRINT_BREAK_TIME_SLOTS=newSettings.value("print-break", "true").toBool();
	DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=newSettings.value("divide-html-timetables-with-time-axis-by-days", "false").toBool();
	TIMETABLE_HTML_REPEAT_NAMES=newSettings.value("timetables-repeat-vertical-names", "false").toBool();
	
	USE_GUI_COLORS=newSettings.value("use-gui-colors", "false").toBool();

	SHOW_SUBGROUPS_IN_COMBO_BOXES=newSettings.value("show-subgroups-in-combo-boxes", "true").toBool();
	SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING=newSettings.value("show-subgroups-in-activity-planning", "true").toBool();
	
	WRITE_TIMETABLE_CONFLICTS=newSettings.value("write-timetable-conflicts", "true").toBool();

	WRITE_TIMETABLES_STATISTICS=newSettings.value("write-timetables-statistics", "true").toBool();
	WRITE_TIMETABLES_XML=newSettings.value("write-timetables-xml", "true").toBool();
	WRITE_TIMETABLES_DAYS_HORIZONTAL=newSettings.value("write-timetables-days-horizontal", "true").toBool();
	WRITE_TIMETABLES_DAYS_VERTICAL=newSettings.value("write-timetables-days-vertical", "true").toBool();
	WRITE_TIMETABLES_TIME_HORIZONTAL=newSettings.value("write-timetables-time-horizontal", "true").toBool();
	WRITE_TIMETABLES_TIME_VERTICAL=newSettings.value("write-timetables-time-vertical", "true").toBool();

	WRITE_TIMETABLES_SUBGROUPS=newSettings.value("write-timetables-subgroups", "true").toBool();
	WRITE_TIMETABLES_GROUPS=newSettings.value("write-timetables-groups", "true").toBool();
	WRITE_TIMETABLES_YEARS=newSettings.value("write-timetables-years", "true").toBool();
	WRITE_TIMETABLES_TEACHERS=newSettings.value("write-timetables-teachers", "true").toBool();
	WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=newSettings.value("write-timetables-teachers-free-periods", "true").toBool();
	WRITE_TIMETABLES_ROOMS=newSettings.value("write-timetables-rooms", "true").toBool();
	WRITE_TIMETABLES_SUBJECTS=newSettings.value("write-timetables-subjects", "true").toBool();
	WRITE_TIMETABLES_ACTIVITY_TAGS=newSettings.value("write-timetables-activity-tags", "true").toBool();
	WRITE_TIMETABLES_ACTIVITIES=newSettings.value("write-timetables-activities", "true").toBool();

	ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY=newSettings.value("enable-activity-tag-max-hours-daily", "false").toBool();
	ENABLE_STUDENTS_MAX_GAPS_PER_DAY=newSettings.value("enable-students-max-gaps-per-day", "false").toBool();
	SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=newSettings.value("warn-if-using-not-perfect-constraints", "true").toBool();
	SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=newSettings.value("warn-subgroups-with-the-same-activities", "true").toBool();
	ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=newSettings.value("enable-students-min-hours-daily-with-allow-empty-days", "false").toBool();
	SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=newSettings.value("warn-if-using-students-min-hours-daily-with-allow-empty-days", "true").toBool();
	
	ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER=newSettings.value("enable-group-activities-in-initial-order", "false").toBool();
	SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=newSettings.value("warn-if-using-group-activities-in-initial-order", "true").toBool();
	
	//main form

	SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES=newSettings.value("FetMainForm/show-tooltips-for-constraints-with-tables", "false").toBool();
	
	if(VERBOSE){
		cout<<"Settings read"<<endl;
	}
}

static void writeSimulationParameters()
{
	QSettings settings;

	settings.setValue("output-directory", OUTPUT_DIR);
	settings.setValue("language", FET_LANGUAGE);
	settings.setValue("working-directory", WORKING_DIRECTORY);
	settings.setValue("import-directory", IMPORT_DIRECTORY);
	settings.setValue("version", FET_VERSION);
	settings.setValue("check-for-updates", checkForUpdates);
	settings.setValue("html-level", TIMETABLE_HTML_LEVEL);
	settings.setValue("print-activity-tags", TIMETABLE_HTML_PRINT_ACTIVITY_TAGS);
	settings.setValue("print-detailed-timetables", PRINT_DETAILED_HTML_TIMETABLES);
	settings.setValue("print-detailed-teachers-free-periods-timetables", PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS);
	settings.setValue("print-activities-with-same-starting-time", PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME);
	settings.setValue("divide-html-timetables-with-time-axis-by-days", DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS);
	settings.setValue("timetables-repeat-vertical-names", TIMETABLE_HTML_REPEAT_NAMES);
	settings.setValue("print-not-available", PRINT_NOT_AVAILABLE_TIME_SLOTS);
	settings.setValue("print-break", PRINT_BREAK_TIME_SLOTS);
	
	settings.setValue("use-gui-colors", USE_GUI_COLORS);

	settings.setValue("show-subgroups-in-combo-boxes", SHOW_SUBGROUPS_IN_COMBO_BOXES);
	settings.setValue("show-subgroups-in-activity-planning", SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	
	settings.setValue("write-timetable-conflicts", WRITE_TIMETABLE_CONFLICTS);

	settings.setValue("write-timetables-statistics", WRITE_TIMETABLES_STATISTICS);
	settings.setValue("write-timetables-xml", WRITE_TIMETABLES_XML);
	settings.setValue("write-timetables-days-horizontal", WRITE_TIMETABLES_DAYS_HORIZONTAL);
	settings.setValue("write-timetables-days-vertical", WRITE_TIMETABLES_DAYS_VERTICAL);
	settings.setValue("write-timetables-time-horizontal", WRITE_TIMETABLES_TIME_HORIZONTAL);
	settings.setValue("write-timetables-time-vertical", WRITE_TIMETABLES_TIME_VERTICAL);

	settings.setValue("write-timetables-subgroups", WRITE_TIMETABLES_SUBGROUPS);
	settings.setValue("write-timetables-groups", WRITE_TIMETABLES_GROUPS);
	settings.setValue("write-timetables-years", WRITE_TIMETABLES_YEARS);
	settings.setValue("write-timetables-teachers", WRITE_TIMETABLES_TEACHERS);
	settings.setValue("write-timetables-teachers-free-periods", WRITE_TIMETABLES_TEACHERS_FREE_PERIODS);
	settings.setValue("write-timetables-rooms", WRITE_TIMETABLES_ROOMS);
	settings.setValue("write-timetables-subjects", WRITE_TIMETABLES_SUBJECTS);
	settings.setValue("write-timetables-activity-tags", WRITE_TIMETABLES_ACTIVITY_TAGS);
	settings.setValue("write-timetables-activities", WRITE_TIMETABLES_ACTIVITIES);

	settings.setValue("enable-activity-tag-max-hours-daily", ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY);
	settings.setValue("enable-students-max-gaps-per-day", ENABLE_STUDENTS_MAX_GAPS_PER_DAY);
	settings.setValue("warn-if-using-not-perfect-constraints", SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS);
	settings.setValue("warn-subgroups-with-the-same-activities", SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES);
	settings.setValue("enable-students-min-hours-daily-with-allow-empty-days", ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);
	settings.setValue("warn-if-using-students-min-hours-daily-with-allow-empty-days", SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS);

	settings.setValue("enable-group-activities-in-initial-order", ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER);
	settings.setValue("warn-if-using-group-activities-in-initial-order", SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER);

	//main form
	settings.setValue("FetMainForm/show-tooltips-for-constraints-with-tables", SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES);
}
#endif

static void initLanguagesSet()
{
	//This is one of the two places to insert a new language in the sources (the other one is in fetmainform.cpp).
	languagesSet.clear();
	languagesSet.insert("ar");
	languagesSet.insert("ca");
	languagesSet.insert("de");
	languagesSet.insert("el");
	languagesSet.insert("en_GB");
	languagesSet.insert("en_US");
	languagesSet.insert("es");
	languagesSet.insert("fr");
	languagesSet.insert("hu");
	languagesSet.insert("id");
	languagesSet.insert("it");
	languagesSet.insert("lt");
	languagesSet.insert("mk");
	languagesSet.insert("ms");
	languagesSet.insert("nl");
	languagesSet.insert("pl");
	languagesSet.insert("ro");
	languagesSet.insert("tr");
	languagesSet.insert("ru");
	languagesSet.insert("fa");
	languagesSet.insert("uk");
	languagesSet.insert("pt_BR");
	languagesSet.insert("da");
	languagesSet.insert("si");
	languagesSet.insert("sk");
	languagesSet.insert("he");
	languagesSet.insert("sr");
	languagesSet.insert("gl");
	languagesSet.insert("vi");
	languagesSet.insert("uz");
	languagesSet.insert("sq");
	languagesSet.insert("zh_CN");
	languagesSet.insert("zh_TW");
	languagesSet.insert("eu");
	languagesSet.insert("cs");
	languagesSet.insert("ja");
}

void setLanguage(QWidget* parent)
{
	static int cntTranslators=0;
	
	if(cntTranslators>0){
		QCoreApplication::removeTranslator(&translator);
		cntTranslators=0;
	}

	//translator stuff
	
	if(FET_LANGUAGE!="en_US" && languagesSet.contains(FET_LANGUAGE)){
		QString lang_filename = "fet_"+FET_LANGUAGE;
		QStringList lang_dirs;
		lang_dirs << QDir::currentPath()
			<< QCoreApplication::applicationDirPath()
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
			<< QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
#else
			<< QStandardPaths::standardLocations(QStandardPaths::DataLocation);
#endif

		bool translation_loaded=false;
		for (int i=0; !translation_loaded && i < lang_dirs.size(); i++) {
			translation_loaded=translator.load(lang_filename, lang_dirs[i]);
			if (!translation_loaded)
				translation_loaded=translator.load(lang_filename, lang_dirs[i]+"/translations");
		}

		if(!translation_loaded){
			QString message("Translation for specified language not loaded - maybe the translation file is missing - setting the language to en_US (US English)");
			message += "\n\n";
			message += QString("FET searched for the translation file '%1' in the following directory list, "
					   "but could not find it:\n")
					   .arg("fet_"+FET_LANGUAGE+".qm");
			for (int i=0; !translation_loaded && i < lang_dirs.size(); i++) {
				message += QDir::toNativeSeparators(lang_dirs[i]) + "\n";
				message += QDir::toNativeSeparators(lang_dirs[i] + "/translations") + "\n";
			}
			FetMessage::warning(parent, QString("FET warning"), message);

			FET_LANGUAGE="en_US";
		}
	}
	else{
		if(FET_LANGUAGE!="en_US"){
			FetMessage::warning(parent, QString("FET warning"),
			 QString("Specified language is incorrect - making it en_US (US English)"));
			FET_LANGUAGE="en_US";
		}
	}
	
	QLocale::setDefault(QLocale(FET_LANGUAGE));

	if(FET_LANGUAGE=="ar" || FET_LANGUAGE=="he" || FET_LANGUAGE=="fa" || FET_LANGUAGE=="ur" /* and others? */){
		LANGUAGE_STYLE_RIGHT_TO_LEFT=true;
	}
	else{
		LANGUAGE_STYLE_RIGHT_TO_LEFT=false;
	}
	
	if(FET_LANGUAGE=="zh_CN"){
		LANGUAGE_FOR_HTML="zh-Hans";
	}
	else if(FET_LANGUAGE=="zh_TW"){
		LANGUAGE_FOR_HTML="zh-Hant";
	}
	/*else if(FET_LANGUAGE=="en_US"){
		LANGUAGE_FOR_HTML=FET_LANGUAGE.left(2);
	}*/
	else{
		LANGUAGE_FOR_HTML=FET_LANGUAGE;
		LANGUAGE_FOR_HTML.replace(QString("_"), QString("-"));
	}
	
	assert(cntTranslators==0);
	if(FET_LANGUAGE!="en_US"){
		QCoreApplication::installTranslator(&translator);
		cntTranslators=1;
	}
	
#ifndef FET_COMMAND_LINE
	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
		QGuiApplication::setLayoutDirection(Qt::RightToLeft);
	
	//retranslate
	QList<QWidget*> tlwl = QApplication::topLevelWidgets();

	foreach(QWidget* wi, tlwl)
		if(wi->isVisible()){
			FetMainForm* mainform=qobject_cast<FetMainForm*>(wi);
			if(mainform!=NULL){
				mainform->retranslateUi(mainform);
				continue;
			}

			//help
			HelpAboutForm* aboutf=qobject_cast<HelpAboutForm*>(wi);
			if(aboutf!=NULL){
				aboutf->retranslateUi(aboutf);
				continue;
			}

			HelpFaqForm* faqf=qobject_cast<HelpFaqForm*>(wi);
			if(faqf!=NULL){
				faqf->retranslateUi(faqf);
				faqf->setText();
				continue;
			}

			HelpTipsForm* tipsf=qobject_cast<HelpTipsForm*>(wi);
			if(tipsf!=NULL){
				tipsf->retranslateUi(tipsf);
				tipsf->setText();
				continue;
			}

			HelpInstructionsForm* instrf=qobject_cast<HelpInstructionsForm*>(wi);
			if(instrf!=NULL){
				instrf->retranslateUi(instrf);
				instrf->setText();
				continue;
			}
			//////
			
			//timetable
			TimetableViewStudentsDaysHorizontalForm* vsdf=qobject_cast<TimetableViewStudentsDaysHorizontalForm*>(wi);
			if(vsdf!=NULL){
				vsdf->retranslateUi(vsdf);
				vsdf->updateStudentsTimetableTable();
				continue;
			}

			TimetableViewStudentsTimeHorizontalForm* vstf=qobject_cast<TimetableViewStudentsTimeHorizontalForm*>(wi);
			if(vstf!=NULL){
				vstf->retranslateUi(vstf);
				vstf->updateStudentsTimetableTable();
				continue;
			}

			TimetableViewTeachersDaysHorizontalForm* vtchdf=qobject_cast<TimetableViewTeachersDaysHorizontalForm*>(wi);
			if(vtchdf!=NULL){
				vtchdf->retranslateUi(vtchdf);
				vtchdf->updateTeachersTimetableTable();
				continue;
			}

			TimetableViewTeachersTimeHorizontalForm* vtchtf=qobject_cast<TimetableViewTeachersTimeHorizontalForm*>(wi);
			if(vtchtf!=NULL){
				vtchtf->retranslateUi(vtchtf);
				vtchtf->updateTeachersTimetableTable();
				continue;
			}

			TimetableViewRoomsDaysHorizontalForm* vrdf=qobject_cast<TimetableViewRoomsDaysHorizontalForm*>(wi);
			if(vrdf!=NULL){
				vrdf->retranslateUi(vrdf);
				vrdf->updateRoomsTimetableTable();
				continue;
			}

			TimetableViewRoomsTimeHorizontalForm* vrtf=qobject_cast<TimetableViewRoomsTimeHorizontalForm*>(wi);
			if(vrtf!=NULL){
				vrtf->retranslateUi(vrtf);
				vrtf->updateRoomsTimetableTable();
				continue;
			}

			TimetableShowConflictsForm* scf=qobject_cast<TimetableShowConflictsForm*>(wi);
			if(scf!=NULL){
				scf->retranslateUi(scf);
				continue;
			}
		}
#endif
}

void SomeQtTranslations()
{
	//This function is never actually used
	//It just contains some commonly used Qt strings, so that some Qt strings of FET are translated.
	QString s1=QCoreApplication::translate("QDialogButtonBox", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s1);
	QString s2=QCoreApplication::translate("QDialogButtonBox", "OK");
	Q_UNUSED(s2);
	
	QString s3=QCoreApplication::translate("QDialogButtonBox", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s3);
	QString s4=QCoreApplication::translate("QDialogButtonBox", "Cancel");
	Q_UNUSED(s4);
	
	QString s5=QCoreApplication::translate("QDialogButtonBox", "&Yes", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s5);
	QString s6=QCoreApplication::translate("QDialogButtonBox", "Yes to &All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s6);
	QString s7=QCoreApplication::translate("QDialogButtonBox", "&No", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s7);
	QString s8=QCoreApplication::translate("QDialogButtonBox", "N&o to All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s8);

	QString s9=QCoreApplication::translate("QDialogButtonBox", "Help");
	Q_UNUSED(s9);

	//It seems that Qt 5 uses other context
	QString s10=QCoreApplication::translate("QPlatformTheme", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s10);
	QString s11=QCoreApplication::translate("QPlatformTheme", "OK");
	Q_UNUSED(s11);
	
	QString s12=QCoreApplication::translate("QPlatformTheme", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s12);
	QString s13=QCoreApplication::translate("QPlatformTheme", "Cancel");
	Q_UNUSED(s13);
	
	QString s14=QCoreApplication::translate("QPlatformTheme", "&Yes", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s14);
	QString s15=QCoreApplication::translate("QPlatformTheme", "Yes to &All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s15);
	QString s16=QCoreApplication::translate("QPlatformTheme", "&No", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s16);
	QString s17=QCoreApplication::translate("QPlatformTheme", "N&o to All", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different. Please keep the translation short.");
	Q_UNUSED(s17);

	QString s18=QCoreApplication::translate("QPlatformTheme", "Help");
	Q_UNUSED(s18);

	QString s19=QCoreApplication::translate("QGnomeTheme", "&OK", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s19);
	QString s20=QCoreApplication::translate("QGnomeTheme", "&Cancel", "Accelerator key (letter after ampersand) for &OK, &Cancel, &Yes, Yes to &All, &No, N&o to All, must be different");
	Q_UNUSED(s20);
	
}

/**
FET starts here
*/
int main(int argc, char **argv)
{
#ifndef FET_COMMAND_LINE
	QApplication app(argc, argv);
#else
	QCoreApplication app(argc, argv);
#endif
	Q_UNUSED(app);

	const QString PROGRAM("fettimetabling");
	const QString COMPANY("fet");
	QCoreApplication::setApplicationName(PROGRAM);
	QCoreApplication::setOrganizationName(COMPANY);
	QCoreApplication::setApplicationVersion(FET_VERSION);

	initLanguagesSet();

	VERBOSE=false;

	CachedSchedule::invalidate();

#ifndef FET_COMMAND_LINE
	QObject::connect(QGuiApplication::instance(), SIGNAL(lastWindowClosed()), QGuiApplication::instance(), SLOT(quit()));
#endif

	RandomKnuth::init();

	OUTPUT_DIR=QDir::homePath()+FILE_SEP+"fet-results";
	
	QStringList _args=QCoreApplication::arguments();

#ifndef FET_COMMAND_LINE
	if(_args.count()==1){
		readSimulationParameters();
	
		QDir dir;
	
		bool t=true;

		//make sure that the output directory exists
		if(!dir.exists(OUTPUT_DIR))
			t=dir.mkpath(OUTPUT_DIR);

		if(!t){
			QMessageBox::critical(NULL, FetTranslate::tr("FET critical"), FetTranslate::tr("Cannot create or use %1 directory (where the results should be stored) - you can continue operation, but you might not be able to work with FET."
			 " Maybe you can try to change the output directory from the 'Settings' menu. If this is a bug - please report it.").arg(QDir::toNativeSeparators(OUTPUT_DIR)));
		}
		
		QString testFileName=OUTPUT_DIR+FILE_SEP+"test_write_permissions_1.tmp";
		QFile test(testFileName);
		bool existedBefore=test.exists();
		bool t_t=test.open(QIODevice::ReadWrite);
		if(!t_t){
			QMessageBox::critical(NULL, FetTranslate::tr("FET critical"), FetTranslate::tr("You don't have write permissions in the output directory "
			 "(FET cannot open or create file %1) - you might not be able to work correctly with FET. Maybe you can try to change the output directory from the 'Settings' menu."
			 " If this is a bug - please report it.").arg(testFileName));
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}

		setLanguage(NULL);

		FetMainForm fetMainForm;
		fetMainForm.show();

		int tmp2=QCoreApplication::exec();
	
		writeSimulationParameters();
		fetMainForm.saveSettings();
	
		if(VERBOSE){
			cout<<"Settings saved"<<endl;
		}
	
		return tmp2;
	}
	else{
		QMessageBox::warning(NULL, FetTranslate::tr("FET warning"), FetTranslate::tr("To start FET in interface mode, please do"
		 " not give any command-line parameters to the FET executable"));
		
		return 1;
	}
#else
	/////////////////////////////////////////////////
	//begin command line
	if(_args.count()>1){
		int randomSeedX=-1;
		int randomSeedY=-1;
		bool randomSeedXSpecified=false;
		bool randomSeedYSpecified=false;
	
		QString outputDirectory="";
	
		INPUT_FILENAME_XML="";
		
		QString filename="";
		
		int secondsLimit=2000000000;
		
		TIMETABLE_HTML_LEVEL=2;
		
		TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=true;

		PRINT_DETAILED_HTML_TIMETABLES=true;

		PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=true;

		FET_LANGUAGE="en_US";
		
		PRINT_NOT_AVAILABLE_TIME_SLOTS=true;
		
		PRINT_BREAK_TIME_SLOTS=true;
		
		WRITE_TIMETABLE_CONFLICTS=true;
	
		WRITE_TIMETABLES_STATISTICS=true;
		WRITE_TIMETABLES_XML=true;
		WRITE_TIMETABLES_DAYS_HORIZONTAL=true;
		WRITE_TIMETABLES_DAYS_VERTICAL=true;
		WRITE_TIMETABLES_TIME_HORIZONTAL=true;
		WRITE_TIMETABLES_TIME_VERTICAL=true;

		WRITE_TIMETABLES_SUBGROUPS=true;
		WRITE_TIMETABLES_GROUPS=true;
		WRITE_TIMETABLES_YEARS=true;
		WRITE_TIMETABLES_TEACHERS=true;
		WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=true;
		WRITE_TIMETABLES_ROOMS=true;
		WRITE_TIMETABLES_SUBJECTS=true;
		WRITE_TIMETABLES_ACTIVITY_TAGS=true;
		WRITE_TIMETABLES_ACTIVITIES=true;

		DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=false;
		
		TIMETABLE_HTML_REPEAT_NAMES=false;

		PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=false;
		
		QStringList unrecognizedOptions;
		
		SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=true;
		
		SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=true;
		
		SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=true;
		
		SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER=true;
		
		bool EXPORT_CSV=false;
		Export csv_export(gt);

		bool showVersion=false;
		
		for(int i=1; i<_args.count(); i++){
			QString s=_args[i];
			
			if(s.left(12)=="--inputfile=")
				filename=QDir::fromNativeSeparators(s.right(s.length()-12));
			else if(s.left(19)=="--timelimitseconds=")
				secondsLimit=s.right(s.length()-19).toInt();
			else if(s.left(21)=="--timetablehtmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-21).toInt();
			else if(s.left(12)=="--htmllevel=")
				TIMETABLE_HTML_LEVEL=s.right(s.length()-12).toInt();
			else if(s.left(20)=="--printactivitytags="){
				if(s.right(5)=="false")
					TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=false;
			}
			else if(s.left(26)=="--printdetailedtimetables="){
				if(s.right(5)=="false")
					PRINT_DETAILED_HTML_TIMETABLES=false;
			}
			else if(s.left(45)=="--printdetailedteachersfreeperiodstimetables="){
				if(s.right(5)=="false")
					PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS=false;
			}
			else if(s.left(11)=="--language=")
				FET_LANGUAGE=s.right(s.length()-11);
			else if(s.left(20)=="--printnotavailable="){
				if(s.right(5)=="false")
					PRINT_NOT_AVAILABLE_TIME_SLOTS=false;
			}
			else if(s.left(13)=="--printbreak="){
				if(s.right(5)=="false")
					PRINT_BREAK_TIME_SLOTS=false;
			}
			else if(s.left(23)=="--dividetimeaxisbydays="){
				if(s.right(4)=="true")
					DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS=true;
			}
			else if(s.left(27)=="--duplicateverticalheaders="){
				if(s.right(4)=="true")
					TIMETABLE_HTML_REPEAT_NAMES=true;
			}
			else if(s.left(12)=="--outputdir="){
				outputDirectory=QDir::fromNativeSeparators(s.right(s.length()-12));
			}
			else if(s.left(30)=="--printsimultaneousactivities="){
				if(s.right(4)=="true")
					PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME=true;
			}
			else if(s.left(14)=="--randomseedx="){
				randomSeedXSpecified=true;
				randomSeedX=s.right(s.length()-14).toInt();
			}
			else if(s.left(14)=="--randomseedy="){
				randomSeedYSpecified=true;
				randomSeedY=s.right(s.length()-14).toInt();
			}
			else if(s.left(35)=="--warnifusingnotperfectconstraints="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS=false;
			}
			else if(s.left(37)=="--warnsubgroupswiththesameactivities="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES=false;
			}
			else if(s.left(53)=="--warnifusingstudentsminhoursdailywithallowemptydays="){
				if(s.right(5)=="false")
					SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS=false;
			}
			else if(s.left(10)=="--verbose="){
				if(s.right(4)=="true")
					VERBOSE=true;
			}
			else if(s=="--version"){
				showVersion=true;
			}
			///
			else if(s.left(26)=="--writetimetableconflicts="){
				if(s.right(5)=="false")
					WRITE_TIMETABLE_CONFLICTS=false;
			}
			//
			else if(s.left(28)=="--writetimetablesstatistics="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_STATISTICS=false;
			}
			else if(s.left(21)=="--writetimetablesxml="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_XML=false;
			}
			else if(s.left(32)=="--writetimetablesdayshorizontal="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_DAYS_HORIZONTAL=false;
			}
			else if(s.left(30)=="--writetimetablesdaysvertical="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_DAYS_VERTICAL=false;
			}
			else if(s.left(32)=="--writetimetablestimehorizontal="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TIME_HORIZONTAL=false;
			}
			else if(s.left(30)=="--writetimetablestimevertical="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TIME_VERTICAL=false;
			}
			//
			else if(s.left(27)=="--writetimetablessubgroups="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_SUBGROUPS=false;
			}
			else if(s.left(24)=="--writetimetablesgroups="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_GROUPS=false;
			}
			else if(s.left(23)=="--writetimetablesyears="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_YEARS=false;
			}
			else if(s.left(26)=="--writetimetablesteachers="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TEACHERS=false;
			}
			else if(s.left(37)=="--writetimetablesteachersfreeperiods="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_TEACHERS_FREE_PERIODS=false;
			}
			else if(s.left(23)=="--writetimetablesrooms="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ROOMS=false;
			}
			else if(s.left(26)=="--writetimetablessubjects="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_SUBJECTS=false;
			}
			else if(s.left(30)=="--writetimetablesactivitytags="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ACTIVITIES=false;
			}
			else if(s.left(28)=="--writetimetablesactivities="){
				if(s.right(5)=="false")
					WRITE_TIMETABLES_ACTIVITIES=false;
			}
			//Export CSV
			else if(s.left(12)=="--exportcsv="){
				if(s.right(4)=="true")
					EXPORT_CSV=true;
			}
			else if(s.left(15)=="--overwritecsv="){
				if(s.right(4)=="true")
					csv_export.setOverwrite(Export::OVERWRITE_ALL);
			}
			else if(s.left(24)=="--firstlineisheadingcsv="){
				if(s.right(5)=="false")
					csv_export.setHeader(false);
			}
			else if(s.left(12)=="--quotescsv="){
				if(s.right(12)=="singlequotes")
					csv_export.setTextQuote("'");
				else if(s.right(4)=="none")
					csv_export.setTextQuote("");
			}
			else if(s.left(20)=="--fieldseparatorcsv="){
				if(s.right(9)=="semicolon")
					csv_export.setFieldSeparator(";");
				else if(s.right(11)=="verticalbar")
					csv_export.setFieldSeparator("|");
			}
			///
			else
				unrecognizedOptions.append(s);
		}
		
		INPUT_FILENAME_XML=filename;
		
		QString initialDir=outputDirectory;
		if(initialDir!="")
			initialDir.append(FILE_SEP);
			
		QString csvOutputDirectory=outputDirectory;
		csvOutputDirectory.append(FILE_SEP + "csv");
		//cout<<"csvOutputDirectory="<<qPrintable(csvOutputDirectory)<<endl;
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);
		outputDirectory.append("timetables");

		/*if(csvOutputDirectory!="")
			csvOutputDirectory.append(FILE_SEP);
		csvOutputDirectory.append("csv");*/

		//////////
		if(INPUT_FILENAME_XML!=""){
			outputDirectory.append(FILE_SEP);
			outputDirectory.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
			if(outputDirectory.right(4)==".fet")
				outputDirectory=outputDirectory.left(outputDirectory.length()-4);

			/*csvOutputDirectory.append(FILE_SEP);
			csvOutputDirectory.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
			if(csvOutputDirectory.right(4)==".fet")
				csvOutputDirectory=csvOutputDirectory.left(csvOutputDirectory.length()-4);*/
		}
		//////////
		
		QDir dir;
		QString logsDir=initialDir+"logs";
		if(!dir.exists(logsDir))
			dir.mkpath(logsDir);
		logsDir.append(FILE_SEP);
		
		////////
		QFile logFile(logsDir+"result.txt");
		bool tttt=logFile.open(QIODevice::WriteOnly);
		if(!tttt){
			cerr<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(logsDir)<<"result.txt)."
			 " If this is a bug - please report it."<<endl;
			return 1;
		}
		QTextStream out(&logFile);
		///////
		
		setLanguage(NULL);
		
		if(showVersion){
			out<<"This file contains the result (log) of last operation"<<endl<<endl;
		
			QDate dat=QDate::currentDate();
			QTime tim=QTime::currentTime();
			QLocale loc;
			QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
			out<<"FET command line request for version started on "<<qPrintable(sTime)<<endl<<endl;
	
			//QString qv=qVersion();
			out<<"FET version "<<qPrintable(FET_VERSION)<<endl;
			out<<"Free timetabling software, licensed under the GNU Affero General Public License version 3 or later"<<endl;
			out<<"Copyright (C) 2002-2018 Liviu Lalescu, Volker Dirr"<<endl;
			out<<"Homepage: https://lalescu.ro/liviu/fet/"<<endl;
			//out<<" (Using Qt version "<<qPrintable(qv)<<")"<<endl;
			cout<<"FET version "<<qPrintable(FET_VERSION)<<endl;
			cout<<"Free timetabling software, licensed under the GNU Affero General Public License version 3 or later"<<endl;
			cout<<"Copyright (C) 2002-2018 Liviu Lalescu, Volker Dirr"<<endl;
			cout<<"Homepage: https://lalescu.ro/liviu/fet/"<<endl;
			//cout<<" (Using Qt version "<<qPrintable(qv)<<")"<<endl;

			if(unrecognizedOptions.count()>0){
				out<<endl;
				cout<<endl;
				foreach(QString s, unrecognizedOptions){
					cout<<"Unrecognized option: "<<qPrintable(s)<<endl;
					out<<"Unrecognized option: "<<qPrintable(s)<<endl;
				}
			}

			logFile.close();
			return 0;
		}
		
		QFile maxPlacedActivityFile(logsDir+"max_placed_activities.txt");
		maxPlacedActivityFile.open(QIODevice::WriteOnly);
		QTextStream maxPlacedActivityStream(&maxPlacedActivityFile);
		maxPlacedActivityStream.setCodec("UTF-8");
		maxPlacedActivityStream.setGenerateByteOrderMark(true);
		maxPlacedActivityStream<<FetTranslate::tr("This is the list of max placed activities, chronologically. If FET could reach maximum n-th activity, look at the n+1-st activity"
			" in the initial order of the activities")<<endl<<endl;
				
		QFile initialOrderFile(logsDir+"initial_order.txt");
		initialOrderFile.open(QIODevice::WriteOnly);
		QTextStream initialOrderStream(&initialOrderFile);
		initialOrderStream.setCodec("UTF-8");
		initialOrderStream.setGenerateByteOrderMark(true);
						
		out<<"This file contains the result (log) of last operation"<<endl<<endl;
		
		QDate dat=QDate::currentDate();
		QTime tim=QTime::currentTime();
		QLocale loc;
		QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
		out<<"FET command line simulation started on "<<qPrintable(sTime)<<endl<<endl;
		
		if(unrecognizedOptions.count()>0){
			foreach(QString s, unrecognizedOptions){
				cout<<"Unrecognized option: "<<qPrintable(s)<<endl;
				out<<"Unrecognized option: "<<qPrintable(s)<<endl;
			}
			cout<<endl;
			out<<endl;
		}
		
		if(outputDirectory!="")
			if(!dir.exists(outputDirectory))
				dir.mkpath(outputDirectory);
		
		if(outputDirectory!="")
			outputDirectory.append(FILE_SEP);
			
		QFile test(outputDirectory+"test_write_permissions_2.tmp");
		bool existedBefore=test.exists();
		bool t_t=test.open(QIODevice::ReadWrite);
		if(!t_t){
			cerr<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(outputDirectory)<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<endl;
			out<<"fet: critical error - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(outputDirectory)<<"test_write_permissions_2.tmp)."
			 " If this is a bug - please report it."<<endl;
			return 1;
		}
		else{
			test.close();
			if(!existedBefore)
				test.remove();
		}

		if(filename==""){
			usage(&out, QString("Input file not specified"));
			logFile.close();
			return 1;
		}
		if(secondsLimit==0){
			usage(&out, QString("Time limit is 0 seconds"));
			logFile.close();
			return 1;
		}
		if(TIMETABLE_HTML_LEVEL>7 || TIMETABLE_HTML_LEVEL<0){
			usage(&out, QString("Html level must be 0, 1, 2, 3, 4, 5, 6, or 7"));
			logFile.close();
			return 1;
		}
		if(randomSeedXSpecified != randomSeedYSpecified){
			if(randomSeedXSpecified){
				usage(&out, QString("If you want to specify the random seed, you need to specify both the X and the Y components, not only the X component"));
			}
			else{
				assert(randomSeedYSpecified);
				usage(&out, QString("If you want to specify the random seed, you need to specify both the X and the Y components, not only the Y component"));
			}
			logFile.close();
			return 1;
		}
		assert(randomSeedXSpecified==randomSeedYSpecified);
		if(randomSeedXSpecified){
			if(randomSeedX<=0 || randomSeedX>=RandomKnuth::getMM()){
				usage(&out, QString("Random seed X component must be at least 1 and at most %1").arg(RandomKnuth::getMM()-1));
				logFile.close();
				return 1;
			}
		}
		if(randomSeedYSpecified){
			if(randomSeedY<=0 || randomSeedY>=RandomKnuth::getMMM()){
				usage(&out, QString("Random seed Y component must be at least 1 and at most %1").arg(RandomKnuth::getMMM()-1));
				logFile.close();
				return 1;
			}
		}
		
		if(randomSeedXSpecified){
			assert(randomSeedYSpecified);
			if(randomSeedX>0 && randomSeedX<RandomKnuth::getMM() && randomSeedY>0 && randomSeedY<RandomKnuth::getMMM()){
				RandomKnuth::XX=randomSeedX;
				RandomKnuth::YY=randomSeedY;
			}
		}
		
		if(TIMETABLE_HTML_LEVEL>7 || TIMETABLE_HTML_LEVEL<0)
			TIMETABLE_HTML_LEVEL=2;
	
		ErrorList errors=gt.rules.read(filename, initialDir);
		renderErrorList(errors);
		if(errors.hasFatal()){
			cerr<<"fet: cannot read input file (not existing or in use) - aborting"<<endl;
			out<<"Cannot read input file (not existing or in use) - aborting"<<endl;
			logFile.close();
			return 1;
		}
		errors.clear();
		
		cout << QCoreApplication::translate("Rules", "Computing internal structure", "Title of a progress dialog").toStdString() << endl;
		out << "Computing internal structure" << endl;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QCoreApplication::connect(&gt.rules, &Rules::internalStructureComputationStepChanged, [&out](RulesComputationStep step) {
			const char * stepText = "";
			switch (step) {
			case RulesComputationStep::ACTIVITIES:
				stepText = "Processing internally the activities ... please wait";
				break;
			case RulesComputationStep::TIME_CONSTRAINTS:
				stepText = "Processing internally the time constraints ... please wait";
				break;
			case RulesComputationStep::SPACE_CONSTRAINTS:
				stepText = "Processing internally the space constraints ... please wait";
				break;
			}
			out << stepText << endl;
			cout << QCoreApplication::translate("Rules", stepText).toStdString() << endl;
		});
#endif
		errors = gt.rules.computeInternalStructure();
		renderErrorList(errors);
		if (errors.hasError()){
			cerr<<"Cannot compute internal structure - aborting"<<endl;
			out<<"Cannot compute internal structure - aborting"<<endl;
			logFile.close();
			return 1;
		}
		errors.clear();
	
		Generate gen(gt);

		terminateGeneratePointer=&gen;
		signal(SIGTERM, terminate);
	
		bool ok=gen.precompute(NULL, &initialOrderStream);
		
		initialOrderFile.close();
		
		if(!ok){
			cerr<<"Cannot precompute - data is wrong - aborting"<<endl;
			out<<"Cannot precompute - data is wrong - aborting"<<endl;
			logFile.close();
			return 1;
		}
		
		cout<<"Starting timetable generation..."<<endl;
		out<<"Starting timetable generation..."<<endl;
		if(VERBOSE){
			cout<<"secondsLimit=="<<secondsLimit<<endl;
		}
		//out<<"secondsLimit=="<<secondsLimit<<endl;
				
		ErrorCode erc = TimetableExport::writeRandomSeedCommandLine(outputDirectory, true); //true represents 'before' state
		if (erc) {
			IrreconcilableCriticalMessage::critical(NULL, erc.getSeverityTitle(), erc.message);
		}

		Generate::Status status = gen.generate(secondsLimit, false, &maxPlacedActivityStream); //false means no thread
		
		maxPlacedActivityFile.close();
	
		if(status == Generate::IMPOSSIBLE){
			cerr<<"Impossible"<<endl;
			out<<"Impossible"<<endl;
			
			//2016-11-17 - suggested by thanhnambkhn, FET will write the impossible activity and the current and highest-stage timetables
			//(which should be identical)

			Solution& cc=gen.getSolution();

			//needed to find the conflicts strings
			QString tmp;
			cc.fitness(gt.rules, &tmp);

			CachedSchedule::update(cc);

			QString toc=outputDirectory;
			if(toc!="" && toc.count()>=1 && toc.endsWith(FILE_SEP)){
				toc.chop(1);
				toc+=QString("-current"+FILE_SEP);
			}
			else if(toc==""){
				toc=QString("current"+FILE_SEP);
			}
			
			if(toc!="")
				if(!dir.exists(toc))
					dir.mkpath(toc);

			ErrorList errors = TimetableExport::writeSimulationResultsCommandLine(toc);
			renderErrorList(errors);
			
			QString s;

			s+=TimetableExport::tr("Please check the constraints related to the "
			 "activity below, which might be impossible to schedule:");
			s+="\n\n";
			const std::vector<int>& difficultActivities = gen.getDifficultActivities();
			for(std::vector<int>::size_type i=0; i<difficultActivities.size(); i++){
				int ai=difficultActivities[i];

				s+=TimetableExport::tr("No: %1").arg(i+1);

				s+=", ";

				s+=TimetableExport::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
					.arg(gt.rules.internalActivitiesList[ai].id)
					.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

				s+="\n";
			}

			QFile difficultActivitiesFile(logsDir+"difficult_activities.txt");
			bool t=difficultActivitiesFile.open(QIODevice::WriteOnly);
			if(!t){
				cerr<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(logsDir)<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
			difficultActivitiesOut.setCodec("UTF-8");
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
			difficultActivitiesOut<<s<<endl;
			
			//2011-11-11 (2)
			//write highest stage timetable
			Solution& ch=gen.getHighestStageSolution();

			//needed to find the conflicts strings
			QString tmp2;
			ch.fitness(gt.rules, &tmp2);

			CachedSchedule::update(ch);

			QString toh=outputDirectory;
			if(toh!="" && toh.count()>=1 && toh.endsWith(FILE_SEP)){
				toh.chop(1);
				toh+=QString("-highest"+FILE_SEP);
			}
			else if(toh==""){
				toh=QString("highest"+FILE_SEP);
			}
			
			if(toh!="")
				if(!dir.exists(toh))
					dir.mkpath(toh);

			errors = TimetableExport::writeSimulationResultsCommandLine(toh);
			renderErrorList(errors);

			if (EXPORT_CSV) {
				QString oldDir=csv_export.getDirectoryCSV();
				csv_export.setDirectoryCSV(csvOutputDirectory);
				csv_export.exportCSV(&gen.getHighestStageSolution(), &gen.getSolution());
				csv_export.setDirectoryCSV(oldDir);
			}
		}
		//2012-01-24 - suggestion and code by Ian Holden (ian@ianholden.com), to write best and current timetable on time exceeded
		//previously, FET saved best and current timetable only on receiving SIGTERM
		//by Ian Holden (begin)
		else if(status == Generate::TIMEOUT || status == Generate::ABORTED){
			if(status == Generate::TIMEOUT){
				cerr<<"Time exceeded"<<endl;
				out<<"Time exceeded"<<endl;
			}
			else if(status == Generate::ABORTED){
				cout<<"Simulation stopped"<<endl;
				out<<"Simulation stopped"<<endl;
			}
			//by Ian Holden (end)
			
			//2011-11-11 (1)
			//write current stage timetable
			Solution& cc=gen.getSolution();

			//needed to find the conflicts strings
			QString tmp;
			cc.fitness(gt.rules, &tmp);

			CachedSchedule::update(cc);

			QString toc=outputDirectory;
			if(toc!="" && toc.count()>=1 && toc.endsWith(FILE_SEP)){
				toc.chop(1);
				toc+=QString("-current"+FILE_SEP);
			}
			else if(toc==""){
				toc=QString("current"+FILE_SEP);
			}
			
			if(toc!="")
				if(!dir.exists(toc))
					dir.mkpath(toc);

			ErrorList errors = TimetableExport::writeSimulationResultsCommandLine(toc);
			renderErrorList(errors);
			
			QString s;
			const int maxActivitiesPlaced = gen.getMaxActivitiesPlaced();
			if(maxActivitiesPlaced>=0 && maxActivitiesPlaced<gt.rules.nInternalActivities 
			 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[maxActivitiesPlaced]<gt.rules.nInternalActivities){
				s=FetTranslate::tr("FET managed to schedule correctly the first %1 most difficult activities."
				 " You can see initial order of placing the activities in the corresponding output file. The activity which might cause problems"
				 " might be the next activity in the initial order of evaluation. This activity is listed below:").arg(maxActivitiesPlaced);
				s+=QString("\n\n");
			
				int ai=initialOrderOfActivitiesIndices[maxActivitiesPlaced];

				s+=FetTranslate::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
			}
			else
				s=FetTranslate::tr("Difficult activity cannot be computed - please report possible bug");
			
			s+=QString("\n\n----------\n\n");
			
			s+=FetTranslate::tr("Here are the placed activities which lead to an inconsistency, "
			 "in order from the first one to the last (the last one FET failed to schedule "
			 "and the last ones are most likely impossible):");
			s+="\n\n";
			const std::vector<int>& difficultActivities = gen.getDifficultActivities();
			for(std::vector<int>::size_type i=0; i<difficultActivities.size(); i++){
				int ai=difficultActivities[i];

				s+=FetTranslate::tr("No: %1").arg(i+1);
		
				s+=", ";

				s+=FetTranslate::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
					.arg(gt.rules.internalActivitiesList[ai].id)
					.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

				s+="\n";
			}
			
			QFile difficultActivitiesFile(logsDir+"difficult_activities.txt");
			bool t=difficultActivitiesFile.open(QIODevice::WriteOnly);
			if(!t){
				cerr<<"FET critical - you don't have write permissions in the output directory - (FET cannot open or create file "<<qPrintable(logsDir)<<"difficult_activities.txt)."
				 " If this is a bug - please report it."<<endl;
				return 1;
			}
			QTextStream difficultActivitiesOut(&difficultActivitiesFile);
			difficultActivitiesOut.setCodec("UTF-8");
			difficultActivitiesOut.setGenerateByteOrderMark(true);
			
			difficultActivitiesOut<<s<<endl;
			
			//2011-11-11 (2)
			//write highest stage timetable
			Solution& ch=gen.getHighestStageSolution();

			//needed to find the conflicts strings
			QString tmp2;
			ch.fitness(gt.rules, &tmp2);

			CachedSchedule::update(ch);

			QString toh=outputDirectory;
			if(toh!="" && toh.count()>=1 && toh.endsWith(FILE_SEP)){
				toh.chop(1);
				toh+=QString("-highest"+FILE_SEP);
			}
			else if(toh==""){
				toh=QString("highest"+FILE_SEP);
			}
			
			if(toh!="")
				if(!dir.exists(toh))
					dir.mkpath(toh);

			errors = TimetableExport::writeSimulationResultsCommandLine(toh);
			renderErrorList(errors);

			if (EXPORT_CSV) {
				QString oldDir=csv_export.getDirectoryCSV();
				csv_export.setDirectoryCSV(csvOutputDirectory);
				csv_export.exportCSV(&gen.getHighestStageSolution(), &gen.getSolution());
				csv_export.setDirectoryCSV(oldDir);
			}
		}
		else{
			cout<<"Simulation successful"<<endl;
			out<<"Simulation successful"<<endl;
		
			ErrorCode erc = TimetableExport::writeRandomSeedCommandLine(outputDirectory, false); //false represents 'before' state
			if (erc) {
				IrreconcilableCriticalMessage::critical(NULL, erc.getSeverityTitle(), erc.message);
			}

			Solution& c=gen.getSolution();

			//needed to find the conflicts strings
			QString tmp;
			c.fitness(gt.rules, &tmp);
			
			CachedSchedule::update(c);

			ErrorList errors = TimetableExport::writeSimulationResultsCommandLine(outputDirectory);
			renderErrorList(errors);
			
			if (EXPORT_CSV) {
				QString oldDir=csv_export.getDirectoryCSV();
				csv_export.setDirectoryCSV(csvOutputDirectory);
				csv_export.exportCSV(&c);
				csv_export.setDirectoryCSV(oldDir);
			}
		}
	
		logFile.close();
		return 0;
	}
	else{
		usage(NULL, QString("No arguments given"));
		return 1;
	}
	//end command line
#endif
	/////////////////////////////////////////////////
}
