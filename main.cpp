/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>

#include "toplevel.h"


static const KLocalizedString description = ki18n("Potato game for kids");
static const KLocalizedString text = ki18n("A program original by <email address='%1'>Éric Bischoff</email>\nand John Calhoun.\n\nThis program is dedicated to my daughter Sunniva.").subs(QLatin1String("ebischoff@nerim.net"));

static const char version[] = "0.9.0";

// Main function
int main(int argc, char *argv[])
{

  KAboutData aboutData( "ktuberling", 0, ki18n("KTuberling"), 
    version, description, KAboutData::License_GPL, 
    ki18n("(c) 1999-2009, The KTuberling Developers"), text, "http://games.kde.org/ktuberling" );
  aboutData.addAuthor(ki18n("Albert Astals Cid"), ki18n("Maintainer"), "aacid@kde.org");
  aboutData.addAuthor(ki18n("Éric Bischoff"), ki18n("Former Developer"), "ebischoff@nerim.net");
  aboutData.addCredit(ki18n("John Calhoun"), ki18n("Original concept and artwork"));
  aboutData.addCredit(ki18n("Agnieszka Czajkowska"), ki18n("New artwork"), "agnieszka@imagegalaxy.de");
  aboutData.addCredit(ki18n("Bas Willems"), ki18n("New artwork"), "cybersurfer@euronet.nl");
  aboutData.addCredit(ki18n("Roger Larsson"), ki18n("Sounds tuning"), "roger.larsson@norran.net");
  aboutData.addCredit(ki18n("Dolores Almansa"), ki18n("New artwork"), "dolores.almansa@corazondemaria.org");
  KCmdLineArgs::init(argc, argv, &aboutData);

  KCmdLineOptions options;
  options.add("+<tuberling-file>", ki18n("Potato to open"));
  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;
  KGlobal::locale()->insertCatalog( QLatin1String( "libkdegames" ));

  TopLevel *toplevel=0;

  if (app.isSessionRestored())
    RESTORE(TopLevel)
  else {
    toplevel = new TopLevel();
    toplevel->show();
    KCmdLineArgs *args  = KCmdLineArgs::parsedArgs();
    if (args->count())
       toplevel->open(args->url(0));
    args->clear();
  }

  return app.exec();
}
