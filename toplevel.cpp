/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include <kapp.h>
#include <kmessagebox.h>
#include <ktoolbarbutton.h>
#include <kmenubar.h>
#include <kstdaccel.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <kio/netaccess.h>
#include <kaction.h>
#include <kstdaction.h>

#include <kprinter.h>
#include <qprintdialog.h>
#include <qclipboard.h>

#include <stdlib.h>

#include "toplevel.moc"
#include "playground.h"
#include "categories.h"

// Constructor
TopLevel::TopLevel()
  : KMainWindow(0)
{
  readOptions();

  playGround = new PlayGround( this, "playground" );
  setCentralWidget( playGround );

  setupKAction();
}

// Destructor
TopLevel::~TopLevel()
{
}

// Enable or disable "undo" button and menu item
void TopLevel::enableUndo(bool enable) const
{
  actionCollection()->action(KStdAction::stdName(KStdAction::Undo))->setEnabled(enable);
}

// Enable or disable "redo" button and menu item
void TopLevel::enableRedo(bool enable) const
{
  actionCollection()->action(KStdAction::stdName(KStdAction::Redo))->setEnabled(enable);
}

// Read options from preferences file
void TopLevel::readOptions()
{
  KConfig *config;
  QString option;

  config = KApplication::kApplication()->config();

  config->setGroup("General");
  option = config->readEntry("Sound", "on");
  soundEnabled = option.find("on") == 0;
}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfig *config;

  config = KApplication::kApplication()->config();

  config->setGroup("General");
  config->writeEntry("Sound", soundEnabled? "on": "off");
  config->sync();
}

// KAction initialization (aka menuba + toolbar init)
void TopLevel::setupKAction()
{
//Game
  KStdAction::openNew(this, SLOT(fileNew()), actionCollection(), "game_new");
  KStdAction::open(this, SLOT(fileOpen()), actionCollection(), "game_load");
  KStdAction::save(this, SLOT(fileSave()), actionCollection(), "game_save");
  KStdAction::print(this, SLOT(filePrint()), actionCollection(), "game_print");
  KStdAction::quit(kapp, SLOT(quit()), actionCollection(), "game_quit");
  (void)new KAction(i18n("Save &as picture..."), 0, this, SLOT(filePicture()), actionCollection(), "game_save_picture");

//Edit
  KStdAction::copy(this, SLOT(editCopy()), actionCollection());
  KStdAction::undo(this, SLOT(editUndo()), actionCollection());
  KStdAction::redo(this, SLOT(editRedo()), actionCollection());

//Settings
  KToggleAction* t = new KToggleAction(i18n("&Sound"), 0, this, SLOT(optionsSound()), actionCollection(), "options_sound");
  t->setChecked(soundEnabled);

  createGUI("ktuberlingui.rc");
}

// Reset gameboard
void TopLevel::fileNew()
{
  playGround->reset();

  enableUndo(false);
  enableRedo(false);

  playGround->repaintAll();
}

// Load gameboard
void TopLevel::fileOpen()
{
  QString name;

  QString dir = locate("data", "ktuberling/museum/miss.tuberling");
  dir.truncate(dir.findRev('/') + 1);

  KURL url = KFileDialog::getOpenURL(dir, "*.tuberling");

  if (url.isEmpty())
    return;

  KIO::NetAccess::download( url, name );

  playGround->reset();

  if (!playGround->loadFrom(name))
    KMessageBox::error(this, i18n("Could not load file."));

  enableUndo(!playGround->isFirstAction());
  enableRedo(false);

  playGround->repaintAll();

  KIO::NetAccess::removeTempFile( name );
}

// Save gameboard
void TopLevel::fileSave()
{
  QString name;

  QString dir = locate("data", "ktuberling/museum/miss.tuberling");
  dir.truncate(dir.find('/') + 1);

  KURL url = KFileDialog::getSaveURL(dir, "*.tuberling");

  if (url.isEmpty())
    return;


  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files currently "
                            "supported."));
    return;
  }

  if( !playGround->saveAs( url.path() ) )
    KMessageBox::error(this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  QPixmap picture(playGround->grabWindow());

  KURL url = KFileDialog::getSaveURL
                (getenv("HOME"),
                 i18n(  "*.xpm|Unix pixmaps (*.xpm)\n"
                        "*.jpg|JPEG compressed files (*.jpg)\n"
                        "*.png|Next generation pictures (*.png)\n"
                        "*.bmp|Windows bitmaps (*.bmp)\n"
                        "*|All picture formats"));
  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry( 0L, i18n( "Only saving to local files supported yet." ) );
    return;
  }

  QString name = url.path();
  const char *format;
  int suffix;
  QString end;

  suffix = name.findRev('.');
  if (suffix == -1)
  {
    name += ".xpm";
    end = "xpm";
  }
  else end = name.mid(suffix + 1, name.length());

  if (end == "xpm") format = "XPM";
  else if (end == "jpg") format = "JPEG";
  else if (end == "png") format = "PNG";
  else if (end == "bmp") format = "BMP";
  else
  {
    KMessageBox::error(this, i18n("Unknown picture format."));
    return;
  }

  if (!picture.save(name, format))
    KMessageBox::error
      (this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePrint()
{
  KPrinter printer;
  bool ok;

  ok = printer.setup(this);
  if (!ok) return;
  playGround->repaint(true);
  if (!playGround->printPicture(printer))
    KMessageBox::error(this,
                         i18n("Could not print picture."));
  else
    KMessageBox::information(this,
                             i18n("Picture successfully printed."));
}

// Copy modified area to clipboard
void TopLevel::editCopy()
{
  QClipboard *clipboard = QApplication::clipboard();
  QPixmap picture(playGround->grabWindow());

  clipboard->setPixmap(picture);
}

// Undo last action
void TopLevel::editUndo()
{
  if (playGround->isFirstAction()) return;

  if (!playGround->undo()) return;

  if (playGround->isFirstAction()) enableUndo(false);
  enableRedo(true);

  playGround->repaintAll();
}

// Redo last action
void TopLevel::editRedo()
{
  if (playGround->isLastAction()) return;

  if (!playGround->redo()) return;

  if (playGround->isLastAction()) enableRedo(false);
  enableUndo(true);

  playGround->repaintAll();
}

// Toggle sound on/off
void TopLevel::optionsSound()
{
  soundEnabled = !soundEnabled;
  ((KToggleAction*)actionCollection()->action("options_sound"))->setChecked(soundEnabled);
  writeOptions();
}
