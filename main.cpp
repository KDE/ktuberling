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

#include "toplevel.h"


static const char *description = I18N_NOOP("KDE Game for kids");

static const char *version = "v0.0.1";


int main(int argc, char *argv[])
{
  KCmdLineArgs::init(argc, argv, "ktuberling", description, version);

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
