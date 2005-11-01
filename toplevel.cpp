/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include <kapplication.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kstdgameaction.h>

#include <kprinter.h>
#include <qclipboard.h>

#include "toplevel.moc"
#include "playground.h"
#include "soundfactory.h"

// Constructor
TopLevel::TopLevel()
  : KMainWindow(0)
{
  readOptions();

  gameboards = 0;
  languages = 0;
  playGround = new PlayGround(this, "playground", selectedGameboard);
  soundFactory = new SoundFactory(this, "sounds", selectedLanguage);

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
  KToggleAction *t = 0;

  switch (gameboards)
  {
  	case 0: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard0()), actionCollection(), actionId);
		break;
  	case 1: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard1()), actionCollection(), actionId);
		break;
  	case 2: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard2()), actionCollection(), actionId);
		break;
  	case 3: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard3()), actionCollection(), actionId);
		break;
  	case 4: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard4()), actionCollection(), actionId);
		break;
  	case 5: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard5()), actionCollection(), actionId);
		break;
  	case 6: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard6()), actionCollection(), actionId);
		break;
  	case 7: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(gameboard7()), actionCollection(), actionId);
		break;
  }

  if( t ) {
      if (gameboards == selectedGameboard) t->setChecked(true);
      gameboardActions[gameboards] = actionId;
      gameboards++;
  }
}

// Register an available language
void TopLevel::registerLanguage(const QString &menuItem, const char *actionId, bool enabled)
{
  KToggleAction *t = 0;

  switch (languages)
  {
  	case 0: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language0()), actionCollection(), actionId);
		break;
  	case 1: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language1()), actionCollection(), actionId);
		break;
  	case 2: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language2()), actionCollection(), actionId);
		break;
  	case 3: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language3()), actionCollection(), actionId);
		break;
  	case 4: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language4()), actionCollection(), actionId);
		break;
  	case 5: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language5()), actionCollection(), actionId);
		break;
  	case 6: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language6()), actionCollection(), actionId);
		break;
  	case 7: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language7()), actionCollection(), actionId);
		break;
  	case 8: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language8()), actionCollection(), actionId);
		break;
  	case 9: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language9()), actionCollection(), actionId);
		break;
  	case 10: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language10()), actionCollection(), actionId);
		break;
  	case 11: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language11()), actionCollection(), actionId);
		break;
  	case 12: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language12()), actionCollection(), actionId);
		break;
  	case 13: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language13()), actionCollection(), actionId);
		break;
  	case 14: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language14()), actionCollection(), actionId);
		break;
  	case 15: t = new KToggleAction(i18n(menuItem.latin1()), 0, this, SLOT(language15()), actionCollection(), actionId);
		break;
  }

  if( t ) {
      if (languages == selectedLanguage) t->setChecked(true);
      t->setEnabled(enabled);
      languageActions[languages] = actionId;
      languages++;
  }
}

// Switch to another gameboard
void TopLevel::changeGameboard(uint newGameboard)
{
  // Do not accept to switch to same gameboard
  if (newGameboard == selectedGameboard) {
    // select this gameboard again
    ((KToggleAction*) actionCollection()->action(gameboardActions[newGameboard].latin1()))->setChecked(true);
    return;
  }

  // Unselect preceding gameboard
  ((KToggleAction*) actionCollection()->action(gameboardActions[selectedGameboard].latin1()))->setChecked(false);

  // Change gameboard in the remembered options
  selectedGameboard = newGameboard;
  writeOptions();

  if( !((KToggleAction*) actionCollection()->action(gameboardActions[selectedGameboard].latin1()))->isChecked() )
    ((KToggleAction*) actionCollection()->action(gameboardActions[selectedGameboard].latin1()))->setChecked(true);
  
  // Change gameboard effectively
  playGround->change(newGameboard);

  // Disable undo and redo actions
  enableUndo(false);
  enableRedo(false);
}

// Switch to another language
void TopLevel::changeLanguage(uint newLanguage)
{
  // Do not accept to switch to same language
  if (newLanguage == selectedLanguage && soundEnabled) {
    // newLanguage should stay checked
    ((KToggleAction*) actionCollection()->action(languageActions[newLanguage].latin1()))->setChecked(true);
    return;
  }

  // Unselect preceding language
  if (!soundEnabled) ((KToggleAction*) actionCollection()->action("speech_no_sound"))->setChecked(false);
  ((KToggleAction*) actionCollection()->action(languageActions[selectedLanguage].latin1()))->setChecked(false);

  // Change language in the remembered options
  soundEnabled = true;
  selectedLanguage = newLanguage;
  writeOptions();

  // Change language effectively
  soundFactory->change(newLanguage);
}

// Load the layouts file
bool TopLevel::loadLayout(QDomDocument &layoutDocument)
{
  QFile layoutFile(QFile::encodeName(locate("data", "ktuberling/pics/layout.xml")));
  if (!layoutFile.open(IO_ReadOnly))
     return false;

  if (!layoutDocument.setContent(&layoutFile))
  {
     layoutFile.close();
     return false;
  }
  layoutFile.close();

  return true;
}

// Play a sound
void TopLevel::playSound(const QString &ref) const
{
  soundFactory->playSound(ref);
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
  if (selectedGameboard <= 0) selectedGameboard = 0;
  if (selectedGameboard > 7) selectedGameboard = 7;

  option = config->readEntry("LanguageNumber", "2");
  selectedLanguage = option.toInt();
  if (selectedLanguage <= 0) selectedLanguage = 0;
  if (selectedLanguage > 15) selectedLanguage = 15;

}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfig *config;

  config = KApplication::kApplication()->config();

  config->setGroup("General");
  config->writeEntry("Sound", soundEnabled? "on": "off");

  config->writeEntry("GameboardNumber", selectedGameboard);

  config->writeEntry("LanguageNumber", selectedLanguage);

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
  (void) new KAction(i18n("Save &as Picture..."), 0, this, SLOT(filePicture()), actionCollection(), "game_save_picture");

//Edit
  KStdAction::copy(this, SLOT(editCopy()), actionCollection());
  KStdAction::undo(this, SLOT(editUndo()), actionCollection());
  KStdAction::redo(this, SLOT(editRedo()), actionCollection());
  enableUndo(false);
  enableRedo(false);

//Speech
  KToggleAction* t = new KToggleAction(i18n("&No Sound"), 0, this, SLOT(soundOff()), actionCollection(), "speech_no_sound");
  if (!soundEnabled) t->setChecked(true);

  setupGUI();
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
  QString dir = locate("data", "ktuberling/museum/miss.tuberling");
  dir.truncate(dir.findRev('/') + 1);

  KURL url = KFileDialog::getOpenURL(dir, "*.tuberling");

  open(url);
}

void TopLevel::open(const KURL &url)
{
  if (url.isEmpty())
    return;

  QString name;

  KIO::NetAccess::download(url, name, this);

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
                ( QString::null,
		 "*.tuberling");

  if (url.isEmpty())
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files is currently "
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
  QPixmap picture(playGround->getPicture());

  KURL url = KFileDialog::getSaveURL
                ( QString::null,
                 i18n(  "*.xpm|UNIX Pixmaps (*.xpm)\n"
                        "*.jpg|JPEG Compressed Files (*.jpg)\n"
                        "*.png|Next Generation Pictures (*.png)\n"
                        "*.bmp|Windows Bitmaps (*.bmp)\n"
                        "*|All Picture Formats"));

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files is currently "
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

  ok = printer.setup(this, i18n("Print %1").arg(actionCollection()->action(gameboardActions[selectedGameboard].latin1())->plainText()));
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
  QPixmap picture(playGround->getPicture());

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

// Switch to gameboard #0
void TopLevel::gameboard0()
{
  changeGameboard(0);
}

// Switch to gameboard #1
void TopLevel::gameboard1()
{
  changeGameboard(1);
}

// Switch to gameboard #2
void TopLevel::gameboard2()
{
  changeGameboard(2);
}

// Switch to gameboard #3
void TopLevel::gameboard3()
{
  changeGameboard(3);
}

// Switch to gameboard #4
void TopLevel::gameboard4()
{
  changeGameboard(4);
}

// Switch to gameboard #5
void TopLevel::gameboard5()
{
  changeGameboard(5);
}

// Switch to gameboard #6
void TopLevel::gameboard6()
{
  changeGameboard(6);
}

// Switch to gameboard #7
void TopLevel::gameboard7()
{
  changeGameboard(7);
}

// Toggle sound off
void TopLevel::soundOff()
{
  if (!soundEnabled) return;

  soundEnabled = false;
  ((KToggleAction*) actionCollection()->action(languageActions[selectedLanguage].latin1()))->setChecked(false);
  ((KToggleAction*) actionCollection()->action("speech_no_sound"))->setChecked(true);

  writeOptions();
}

// Switch to language #0
void TopLevel::language0()
{
  changeLanguage(0);
}

// Switch to language #1
void TopLevel::language1()
{
  changeLanguage(1);
}

// Switch to language #2
void TopLevel::language2()
{
  changeLanguage(2);
}

// Switch to language #3
void TopLevel::language3()
{
  changeLanguage(3);
}

// Switch to language #4
void TopLevel::language4()
{
  changeLanguage(4);
}

// Switch to language #5
void TopLevel::language5()
{
  changeLanguage(5);
}

// Switch to language #6
void TopLevel::language6()
{
  changeLanguage(6);
}

// Switch to language #7
void TopLevel::language7()
{
  changeLanguage(7);
}

// Switch to language #8
void TopLevel::language8()
{
  changeLanguage(8);
}

// Switch to language #9
void TopLevel::language9()
{
  changeLanguage(9);
}

// Switch to language #10
void TopLevel::language10()
{
  changeLanguage(10);
}

// Switch to language #11
void TopLevel::language11()
{
  changeLanguage(11);
}

// Switch to language #12
void TopLevel::language12()
{
  changeLanguage(12);
}

// Switch to language #13
void TopLevel::language13()
{
  changeLanguage(13);
}

// Switch to language #14
void TopLevel::language14()
{
  changeLanguage(14);
}

// Switch to language #15
void TopLevel::language15()
{
  changeLanguage(15);
}
