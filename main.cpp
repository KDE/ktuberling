/* -------------------------------------------------------------
   KDE Tuberling
   Main program
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <kapp.h>
#include <ktopwidget.h>

#include "toplevel.h"

int main(int argc, char *argv[])
{
  KApplication app(argc, argv);

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
