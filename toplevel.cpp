/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebisch@cybercable.tm.fr
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

#include <qprinter.h>
#include <qprintdialog.h>
#include <qclipboard.h>

#include <stdlib.h>

#include "toplevel.h"
#include "playground.h"
#include "categories.h"

// Constructor
TopLevel::TopLevel()
  : KTMainWindow()
{
  readOptions();

  playGround = new PlayGround( this, "playground" );
  setCentralWidget( playGround );

  setupMenuBar();
  setupToolBar();
}

// Destructor
TopLevel::~TopLevel()
{
}

// Enable or disable "undo" button and menu item
void TopLevel::enableUndo(bool enable) const
{
  editMenu->setItemEnabled(undoID, enable);
  toolbar->setItemEnabled(ID_UNDO, enable);
}

// Enable or disable "redo" button and menu item
void TopLevel::enableRedo(bool enable) const
{
  editMenu->setItemEnabled(redoID, enable);
  toolbar->setItemEnabled(ID_REDO, enable);
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

// Menubar initialization
void TopLevel::setupMenuBar()
{
  menubar = menuBar();

  fileMenu = new QPopupMenu( this );
  editMenu = new QPopupMenu( this );
  optionsMenu = new QPopupMenu( this );

  newID = fileMenu->insertItem(i18n("&New"));
  fileMenu->connectItem(newID, this, SLOT(fileNew()));
  fileMenu->setAccel(KStdAccel::key(KStdAccel::New), newID);

  openID = fileMenu->insertItem(i18n("&Open..."));
  fileMenu->connectItem(openID, this, SLOT(fileOpen()));
  fileMenu->setAccel(KStdAccel::key(KStdAccel::Open), openID);

  saveID = fileMenu->insertItem(i18n("&Save..."));
  fileMenu->connectItem(saveID, this, SLOT(fileSave()));
  fileMenu->setAccel(KStdAccel::key(KStdAccel::Save), saveID);

  pictureID = fileMenu->insertItem(i18n("Save &as picture..."));
  fileMenu->connectItem(pictureID, this, SLOT(filePicture()));
  fileMenu->insertSeparator();

  printID = fileMenu->insertItem(i18n("&Print"));
  fileMenu->connectItem(printID, this, SLOT(filePrint()));
  fileMenu->setAccel(KStdAccel::key(KStdAccel::Print), printID);
  fileMenu->insertSeparator();

  quitID = fileMenu->insertItem(i18n("&Quit"));
  fileMenu->connectItem(quitID, kapp, SLOT(quit()));
  fileMenu->setAccel(KStdAccel::key(KStdAccel::Quit), quitID);

  copyID = editMenu->insertItem(i18n("&Copy"));
  editMenu->connectItem(copyID, this, SLOT(editCopy()));
  editMenu->setAccel(KStdAccel::key(KStdAccel::Copy), copyID);
  editMenu->insertSeparator();

  undoID = editMenu->insertItem(i18n("&Undo"));
  editMenu->setItemEnabled(undoID, false);
  editMenu->connectItem(undoID, this, SLOT(editUndo()));
  editMenu->setAccel(KStdAccel::key(KStdAccel::Undo), undoID);


  redoID = editMenu->insertItem(i18n("&Redo"));
  editMenu->setItemEnabled(redoID, false);
  editMenu->connectItem(redoID, this, SLOT(editRedo()));
  editMenu->setAccel(KStdAccel::key(KStdAccel::Redo), redoID);

  soundID = optionsMenu->insertItem(i18n("&Sound"));
  optionsMenu->setItemChecked(soundID, soundEnabled);
  optionsMenu->connectItem(soundID, this, SLOT(optionsSound()));

  QString about = i18n("A program by Eric Bischoff (ebisch@cybercable.tm.fr)\n"
                       "and John Calhoun.\n\n"
                       "This program is dedicated to my daughter Sunniva.");

  menubar->insertItem(i18n("&File"), fileMenu);
  menubar->insertItem(i18n("&Edit"), editMenu);
  menubar->insertItem(i18n("&Options"), optionsMenu);
  menubar->insertSeparator(-1);
  menubar->insertItem(i18n("&Help"), helpMenu(about));

}

// Toolbar initialization
void TopLevel::setupToolBar()
{
  toolbar = toolBar();

  toolbar->insertButton("filenew", ID_NEW, SIGNAL(pressed()), this, SLOT(fileNew()), true, i18n("New"));
  toolbar->insertButton("fileopen", ID_OPEN, SIGNAL(pressed()), this, SLOT(fileOpen()), true, i18n("Open"));
  toolbar->insertButton("filesave", ID_SAVE, SIGNAL(pressed()), this, SLOT(fileSave()), true, i18n("Save"));
  toolbar->insertButton("fileprint", ID_PRINT, SIGNAL(pressed()), this, SLOT(filePrint()), true, i18n("Print"));
  toolbar->insertSeparator();

  toolbar->insertButton("undo", ID_UNDO, SIGNAL(pressed()), this, SLOT(editUndo()), false, i18n("Undo"));
  toolbar->insertButton("redo", ID_REDO, SIGNAL(pressed()), this, SLOT(editRedo()), false, i18n("Redo"));
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

  toolbar->getButton(ID_OPEN)->setDown(false);

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

  toolbar->getButton(ID_SAVE)->setDown(false);

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
  QPrinter printer;
  bool ok;

  ok = QPrintDialog::getPrinterSetup(&printer);
  toolbar->getButton(ID_PRINT)->setDown(false);
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
  toolbar->getButton(ID_UNDO)->setDown(false);

  if (!playGround->undo()) return;

  if (playGround->isFirstAction()) enableUndo(false);
  enableRedo(true);

  playGround->repaintAll();
}

// Redo last action
void TopLevel::editRedo()
{
  if (playGround->isLastAction()) return;
  toolbar->getButton(ID_REDO)->setDown(false);

  if (!playGround->redo()) return;

  if (playGround->isLastAction()) enableRedo(false);
  enableUndo(true);

  playGround->repaintAll();
}

// Toggle sound on/off
void TopLevel::optionsSound()
{
  soundEnabled = !soundEnabled;
  optionsMenu->setItemChecked( soundID, soundEnabled);
  writeOptions();
}
