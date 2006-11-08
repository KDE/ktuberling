/* -------------------------------------------------------------
   KDE Tuberling
   Main program
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */

#include <kapplication.h>
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>

#include "toplevel.h"

static KCmdLineOptions options[] = {
   { "+<tuberling-file>", I18N_NOOP("Potato to open"), 0 },
   KCmdLineLastOption
};
                  


static const char description[] = I18N_NOOP("Potato game for kids");
static const char text[] = I18N_NOOP("A program by Éric Bischoff <ebischoff@nerim.net>\nand John Calhoun.\n\nThis program is dedicated to my daughter Sunniva.");

static const char version[] = "0.5.1";

// Main function
int main(int argc, char *argv[])
{

  KAboutData aboutData( "ktuberling", I18N_NOOP("KTuberling"), 
    version, description, KAboutData::License_GPL, 
    "(c) 1999-2006, The KTuberling Developers", text);
  aboutData.addAuthor("Éric Bischoff", I18N_NOOP("Developer"), "ebischoff@nerim.net");
  aboutData.addAuthor("John Calhoun", I18N_NOOP("Original concept and artwork"));
  aboutData.addCredit("Agnieszka Czajkowska", I18N_NOOP("New artwork"), "agnieszka@imagegalaxy.de");
  aboutData.addCredit("Bas Willems", I18N_NOOP("New artwork"), "cybersurfer@euronet.nl");
  aboutData.addCredit("Roger Larsson", I18N_NOOP("Sounds tuning"), "roger.larsson@norran.net");
  aboutData.addCredit("Dolores Almansa", I18N_NOOP("New artwork"), "dolores.almansa@corazondemaria.org");
  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;
  KGlobal::locale()->insertCatalog("libkdegames");

  TopLevel *toplevel=0;

  if (app.isSessionRestored())
    RESTORE(TopLevel)
  else {
    toplevel = new TopLevel();
    toplevel->show();
    KCmdLineArgs *args  = KCmdLineArgs::parsedArgs();
    if (args->count())
       toplevel->open(args->url(0));
  }

  return app.exec();
}
