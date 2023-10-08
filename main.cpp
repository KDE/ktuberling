/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KLocalizedString>

#include "toplevel.h"
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>

#include "ktuberling_version.h"

// Main function
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  KLocalizedString::setApplicationDomain("ktuberling");

  KAboutData aboutData( QStringLiteral("ktuberling"), i18n("KTuberling"), 
          KTUBERLING_VERSION_STRING, i18n("Potato game for kids"), KAboutLicense::GPL,
          i18n("(c) 1999-2009, The KTuberling Developers"));
  aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Maintainer"), QStringLiteral("aacid@kde.org"));
  aboutData.addAuthor(i18n("Éric Bischoff"), i18n("Former Developer"), QStringLiteral("ebischoff@nerim.net"));
  aboutData.addCredit(i18n("John Calhoun"), i18n("Original concept and artwork"));
  aboutData.addCredit(i18n("Agnieszka Czajkowska"), i18n("New artwork"), QStringLiteral("agnieszka@imagegalaxy.de"));
  aboutData.addCredit(i18n("Bas Willems"), i18n("New artwork"), QStringLiteral("cybersurfer@euronet.nl"));
  aboutData.addCredit(i18n("Roger Larsson"), i18n("Sounds tuning"), QStringLiteral("roger.larsson@norran.net"));
  aboutData.addCredit(i18n("Dolores Almansa"), i18n("New artwork"), QStringLiteral("dolores.almansa@corazondemaria.org"));
  aboutData.setHomepage(QStringLiteral("https://apps.kde.org/ktuberling"));

  KAboutData::setApplicationData(aboutData);
  QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("ktuberling")));

  KCrash::initialize();

  QCommandLineParser parser;
  parser.addPositionalArgument(QStringLiteral("tuberling-file"), i18n("Potato to open"));
  aboutData.setupCommandLine(&parser);
  parser.process(app);
  aboutData.processCommandLine(&parser);

  KDBusService service;
  TopLevel *toplevel=nullptr;

  if (app.isSessionRestored())
      kRestoreMainWindows<TopLevel>();
  else {
      toplevel = new TopLevel();
      toplevel->show();
      if (!parser.positionalArguments().isEmpty())
          toplevel->open(QUrl::fromUserInput(parser.positionalArguments().at(0), QDir::currentPath()));

  }

  return app.exec();
}
