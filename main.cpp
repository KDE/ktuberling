/* -------------------------------------------------------------
   KDE Tuberling
   Main program
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include <kapplication.h>
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "toplevel.h"


static const char *description = I18N_NOOP("Potato game for kids");
static const char *text = I18N_NOOP("A program by Eric Bischoff <e.bischoff@noos.fr>\nand John Calhoun.\n\nThis program is dedicated to my daughter Sunniva.");

static const char *version = "v0.2.4";


int main(int argc, char *argv[])
{

  KAboutData aboutData( "ktuberling", I18N_NOOP("KTuberling"), 
    version, description, KAboutData::License_GPL, 
    "(c) 1999-2001, The KTuberling Developers", text);
  aboutData.addAuthor("Eric Bischoff", I18N_NOOP("Developer"), "e.bischoff@noos.fr");
  aboutData.addAuthor("John Calhoun", I18N_NOOP("Original concept and artwork"));
  aboutData.addAuthor("Bas Willems", I18N_NOOP("New artwork"), "next@euronet.nl");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;
  KGlobal::locale()->insertCatalogue("libkdegames");
  KImageIO::registerFormats();

  TopLevel *toplevel=0;

  if (app.isRestored())
    RESTORE(TopLevel)
  else {
    // no session management: just create one window
    toplevel = new TopLevel();
    toplevel->show();
  }
  toplevel->show();

  return app.exec();
}
