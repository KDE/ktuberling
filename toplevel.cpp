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
#include <kstdgameaction.h>

#include <kprinter.h>
#include <qprintdialog.h>
#include <qclipboard.h>

#include <stdlib.h>
#include <unistd.h>

#include "toplevel.moc"
#include "playground.h"

// Constructor
TopLevel::TopLevel()
  : KMainWindow(0)
{
  readOptions();

  gameboards = 0;
  playGround = new PlayGround(this, "playground", selectedGameboard);

  setCentralWidget(playGround);

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

// Register an available gameboard
void TopLevel::registerGameboard(const QString &menuItem, const char *actionId)
{
  KToggleAction *t;

  switch (gameboards)
  {
  	case 0: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard0()), actionCollection(), actionId);
		break;
  	case 1: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard1()), actionCollection(), actionId);
		break;
  	case 2: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard2()), actionCollection(), actionId);
		break;
  	case 3: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard3()), actionCollection(), actionId);
		break;
  	case 4: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard4()), actionCollection(), actionId);
		break;
  	case 5: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard5()), actionCollection(), actionId);
		break;
  	case 6: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard6()), actionCollection(), actionId);
		break;
  	case 7: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(optionsGameboard7()), actionCollection(), actionId);
		break;
  }
  if (gameboards == selectedGameboard) t->setChecked(true);
  gameboardActions[gameboards] = actionId;
  gameboards++;
}

// Switch to another gameboard
void TopLevel::changeGameboard(uint newGameboard)
{
  // Do not accept to switch to same gameboard
  if (newGameboard == selectedGameboard)
    return;

  // Unselect preceeding gameboard
  ((KToggleAction*) actionCollection()->action(gameboardActions[selectedGameboard].latin1()))->setChecked(false);

  // Change gameboard in the remembered options
  selectedGameboard = newGameboard;
  writeOptions();

  // Change gameboard effectively
  playGround->change(newGameboard);
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

  option = config->readEntry("GameboardNumber", "0");
  selectedGameboard = option.toInt();
  if (selectedGameboard < 0) selectedGameboard = 0;
  if (selectedGameboard > 7) selectedGameboard = 7;
}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfig *config;

  config = KApplication::kApplication()->config();

  config->setGroup("General");
  config->writeEntry("Sound", soundEnabled? "on": "off");

  config->writeEntry("GameboardNumber", selectedGameboard);

  config->sync();
}

// KAction initialization (aka menubar + toolbar init)
void TopLevel::setupKAction()
{
//Game
  KStdGameAction::gameNew(this, SLOT(fileNew()), actionCollection());
  KStdGameAction::load(this, SLOT(fileOpen()), actionCollection());
  KStdGameAction::save(this, SLOT(fileSave()), actionCollection());
  KStdGameAction::print(this, SLOT(filePrint()), actionCollection());
  KStdGameAction::quit(kapp, SLOT(quit()), actionCollection());
  (void) new KAction(i18n("Save &as picture..."), 0, this, SLOT(filePicture()), actionCollection(), "game_save_picture");

//Edit
  KStdAction::copy(this, SLOT(editCopy()), actionCollection());
  KStdAction::undo(this, SLOT(editUndo()), actionCollection());
  KStdAction::redo(this, SLOT(editRedo()), actionCollection());
  enableUndo(false);
  enableRedo(false);
  
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
  KURL url = KFileDialog::getSaveURL
                (getenv("HOME"),
		 "*.tuberling");

  if (url.isEmpty())
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files currently "
                            "supported."));
    return;
  }

  QString name = url.path();
  int suffix;

  suffix = name.findRev('.');
  if (suffix == -1)
  {
    name += ".tuberling";
  }

  if( !playGround->saveAs( name ) )
    KMessageBox::error(this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  playGround->repaint(true);

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
    KMessageBox::sorry(this,
                       i18n("Only saving to local files currently "
                            "supported."));
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
  ((KToggleAction*) actionCollection()->action("options_sound"))->setChecked(soundEnabled);

  writeOptions();
}

// Switch to gameboard #0
void TopLevel::optionsGameboard0()
{
  changeGameboard(0);
}

// Switch to gameboard #1
void TopLevel::optionsGameboard1()
{
  changeGameboard(1);
}

// Switch to gameboard #2
void TopLevel::optionsGameboard2()
{
  changeGameboard(2);
}

// Switch to gameboard #3
void TopLevel::optionsGameboard3()
{
  changeGameboard(3);
}

// Switch to gameboard #4
void TopLevel::optionsGameboard4()
{
  changeGameboard(4);
}

// Switch to gameboard #5
void TopLevel::optionsGameboard5()
{
  changeGameboard(5);
}

// Switch to gameboard #6
void TopLevel::optionsGameboard6()
{
  changeGameboard(6);
}

// Switch to gameboard #7
void TopLevel::optionsGameboard7()
{
  changeGameboard(7);
}
