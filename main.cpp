/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <KLocalizedString>

#include <KAboutData>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <KDBusService>
#include "toplevel.h"

static const QString description = i18n("Potato game for kids");
static const KLocalizedString text = ki18n("A program original by <email address='%1'>Éric Bischoff</email>\nand John Calhoun.\n\nThis program is dedicated to my daughter Sunniva.").subs(QLatin1String("ebischoff@nerim.net"));

static const char version[] = "0.9.0";

// Main function
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  KAboutData aboutData( "ktuberling", i18n("KTuberling"), 
    version, description, KAboutLicense::GPL, 
    i18n("(c) 1999-2009, The KTuberling Developers"), "http://games.kde.org/ktuberling" );
  aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Maintainer"), "aacid@kde.org");
  aboutData.addAuthor(i18n("Éric Bischoff"), i18n("Former Developer"), "ebischoff@nerim.net");
  aboutData.addCredit(i18n("John Calhoun"), i18n("Original concept and artwork"));
  aboutData.addCredit(i18n("Agnieszka Czajkowska"), i18n("New artwork"), "agnieszka@imagegalaxy.de");
  aboutData.addCredit(i18n("Bas Willems"), i18n("New artwork"), "cybersurfer@euronet.nl");
  aboutData.addCredit(i18n("Roger Larsson"), i18n("Sounds tuning"), "roger.larsson@norran.net");
  aboutData.addCredit(i18n("Dolores Almansa"), i18n("New artwork"), "dolores.almansa@corazondemaria.org");
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
  parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("+<tuberling-file>"), i18n("Potato to open")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

  KDBusService service;
  TopLevel *toplevel=0;

  if (app.isSessionRestored())
    RESTORE(TopLevel)
  else {
    toplevel = new TopLevel();
    toplevel->show();
    if (parser.positionalArguments().count())
       toplevel->open(QUrl::fromUserInput(parser.positionalArguments().at(0), QDir::currentPath()));
    
  }

  return app.exec();
}
