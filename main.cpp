/* -------------------------------------------------------------
   KDE Tuberling
   Main program
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <kapp.h>
#include <ktmainwindow.h>
#include <kimgio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "toplevel.h"


static const char *description = I18N_NOOP("Potato game for kids");

static const char *version = "v0.0.1";


int main(int argc, char *argv[])
{

  KAboutData aboutData( "ktuberling", I18N_NOOP("KTuberling"), 
    version, description, KAboutData::License_GPL, 
    "(c) 1999, The KTuberling Developers");
  aboutData.addAuthor("Eric Bischoff",0, "ebisch@cybercable.tm.fr");
  aboutData.addAuthor("John Calhoun");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;
  kimgioRegister();

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
