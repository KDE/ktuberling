/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <qmessagebox.h>
#include <qpainter.h>
#include <qimage.h>
#include <qlist.h>
#include <qclipboard.h>
#include <qprintdialog.h>
#include <qprinter.h>
#include <qjpegio.h>
#include <qpngio.h>

#include <kiconloader.h>
#include <kfiledialog.h>
#include <kstdaccel.h>

#include "toplevel.h"
#include "categories.h"

#define XMARGIN 5
#define YMARGIN 5
#define XORIGIN 0
#define YORIGIN 50

// Constructor
TopLevel::TopLevel()
  : KTMainWindow()
{
  setBackgroundColor(white);
  readOptions();
  setupSound();
  setupMenuBar();
  setupToolBar();
  qInitJpegIO();
  qInitPngIO();
  objectsLayout = shapesLayout = 0;
  textsList = soundsList = 0;
  toDraw.setAutoDelete(true);
  if (!loadBitmaps())
  {
    QMessageBox::critical
	(this,
         i18n("Fatal error"), 
         i18n("I could not load the pictures. I'll quit")); 
    exit(-1);
  }
  setupGeometry();
  draggedCursor = 0;
}

// Destructor
TopLevel::~TopLevel()
{
  if (audioServer) delete audioServer;
  if (soundsList) delete soundsList;
  if (textsList) delete textsList;
  if (objectsLayout) delete objectsLayout;
  if (shapesLayout) delete shapesLayout;
  if (draggedCursor) delete draggedCursor;
}

// Set up sound
void TopLevel::setupSound()
{
  audioServer = new KAudio;
  if (audioServer->serverStatus())
  {
    delete audioServer;
    audioServer = NULL;
  }
}

// Read options from preferences file
void TopLevel::readOptions()
{
  KConfig *config;
  QString option;

  config = KApplication::getKApplication()->getConfig();

  config->setGroup("General");
  option = config->readEntry("Sound", "on");
  soundEnabled = option.find("on") == 0;
}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfig *config;

  config = KApplication::getKApplication()->getConfig();

  config->setGroup("General");
  config->writeEntry("Sound", soundEnabled? "on": "off");
  config->sync();
}

// Menubar initialization
void TopLevel::setupMenuBar()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  KStdAccel acc(config);

  menubar = new KMenuBar(this);
  fileMenu = new QPopupMenu();
  editMenu = new QPopupMenu();
  optionsMenu = new QPopupMenu();
  helpMenu = new QPopupMenu();

  newID = fileMenu->insertItem(i18n("&New"));
  fileMenu->connectItem(newID, this, SLOT(fileNew()));
  fileMenu->setAccel(acc.openNew(), newID);

  openID = fileMenu->insertItem(i18n("&Open..."));
  fileMenu->connectItem(openID, this, SLOT(fileOpen()));
  fileMenu->setAccel(acc.open(), openID);

  saveID = fileMenu->insertItem(i18n("&Save..."));
  fileMenu->connectItem(saveID, this, SLOT(fileSave()));
  fileMenu->setAccel(acc.save(), saveID);

  pictureID = fileMenu->insertItem(i18n("Save &as picture..."));
  fileMenu->connectItem(pictureID, this, SLOT(filePicture()));
  fileMenu->insertSeparator();

  printID = fileMenu->insertItem(i18n("&Print"));
  fileMenu->connectItem(printID, this, SLOT(filePrint()));
  fileMenu->setAccel(acc.print(), printID);
  fileMenu->insertSeparator();

  quitID = fileMenu->insertItem(i18n("&Quit"));
  fileMenu->connectItem(quitID, kapp, SLOT(quit()));
  fileMenu->setAccel(acc.quit(), quitID);

  copyID = editMenu->insertItem(i18n("&Copy"));
  editMenu->connectItem(copyID, this, SLOT(editCopy()));
  editMenu->setAccel(acc.copy(), copyID);
  editMenu->insertSeparator();

  undoID = editMenu->insertItem(i18n("&Undo"));
  editMenu->connectItem(undoID, this, SLOT(editUndo()));
  editMenu->setAccel(acc.undo(), undoID);

  redoID = editMenu->insertItem(i18n("&Redo"));
  editMenu->connectItem(redoID, this, SLOT(editRedo()));
  editMenu->setAccel(CTRL | Key_A, redoID);

  soundID = optionsMenu->insertItem(i18n("&Sound"));
  optionsMenu->setItemChecked(soundID, soundEnabled);
  if (!audioServer) optionsMenu->setItemEnabled(soundID, false);
  optionsMenu->connectItem(soundID, this, SLOT(optionsSound()));

  contentsID = helpMenu->insertItem(i18n("&Contents"));
  helpMenu->connectItem(contentsID, kapp, SLOT(appHelpActivated()));
  helpMenu->setAccel(acc.help(), contentsID);
  helpMenu->insertSeparator();

  aboutID = helpMenu->insertItem(i18n("&About ktuberling ..."));
  helpMenu->connectItem(aboutID, this, SLOT(aboutApp()));

  aboutKDEID = helpMenu->insertItem(i18n("About &KDE..."));
  helpMenu->connectItem(aboutKDEID, kapp, SLOT(aboutKDE()));

  menubar->insertItem(i18n("&File"), fileMenu);
  menubar->insertItem(i18n("&Edit"), editMenu);
  menubar->insertItem(i18n("&Options"), optionsMenu);
  menubar->insertSeparator(-1);
  menubar->insertItem(i18n("&Help"), helpMenu);

  setMenu(menubar);
}

// Toolbar initialization
void TopLevel::setupToolBar()
{
  KIconLoader iconLoader;
  toolbar = new KToolBar(this);

  addToolBar(toolbar);

  toolbar->insertButton(iconLoader.loadIcon("filenew.xpm"), ID_NEW, SIGNAL(pressed()), this, SLOT(fileNew()));
  toolbar->insertButton(iconLoader.loadIcon("fileopen.xpm"), ID_OPEN, SIGNAL(pressed()), this, SLOT(fileOpen()));
  toolbar->insertButton(iconLoader.loadIcon("filefloppy.xpm"), ID_SAVE, SIGNAL(pressed()), this, SLOT(fileSave()));
  toolbar->insertButton(iconLoader.loadIcon("fileprint.xpm"), ID_PRINT, SIGNAL(pressed()), this, SLOT(filePrint()));
  toolbar->insertSeparator();

  toolbar->insertButton(iconLoader.loadIcon("undo.xpm"), ID_UNDO, SIGNAL(pressed()), this, SLOT(editUndo()));
  toolbar->insertButton(iconLoader.loadIcon("redo.xpm"), ID_REDO, SIGNAL(pressed()), this, SLOT(editRedo()));
  toolbar->insertSeparator();

  toolbar->insertButton(iconLoader.loadIcon("help.xpm"), ID_HELP, SIGNAL(pressed()), kapp, SLOT(appHelpActivated()));
}

// Load background and draggable objects masks
bool TopLevel::loadBitmaps()
{
  FILE *layoutFile;
  char comment[512];
  int editableLeft, editableTop, editableRight, editableBottom;
  int textLeft, textTop, textRight, textBottom;
  int objectLeft, objectTop, objectRight, objectBottom;
  int shapeLeft, shapeTop, shapeRight, shapeBottom;
  QRect *text, *object, *shape;
  int *label, *sound;

  if (!gameboard.load((const char *) (KApplication::kde_datadir() + "/ktuberling/pics/gameboard.xpm")))
	return false;

  if (!masks.load((const char *) (KApplication::kde_datadir() + "/ktuberling/pics/masks.xpm")))
	return false;

  if (!(layoutFile = fopen((const char *) (KApplication::kde_datadir() + "/ktuberling/pics/layout.txt"), "r")))
	return false;

  if (fscanf(layoutFile, "%d %d %d %d %d",
	     &editableLeft, &editableTop, &editableRight, &editableBottom,
             &editableSound) != 5) { fclose(layoutFile); return false; }
  if (!fgets(comment, sizeof(comment), layoutFile))
	{ fclose(layoutFile); return false; }
  editableArea.setCoords
	(XMARGIN + XORIGIN + editableLeft,
         YMARGIN + YORIGIN + editableTop,
         XMARGIN + XORIGIN + editableRight,
         YMARGIN + YORIGIN + editableBottom);

  if (fscanf(layoutFile, "%d", &texts) != 1)
	{ fclose(layoutFile); return false; }
  if (!fgets(comment, sizeof(comment), layoutFile))
	{ fclose(layoutFile); return false; }
  if (texts < 1)
	{ fclose(layoutFile); return false; }

  if (!(textsLayout = new QRect[texts])) { fclose(layoutFile); return false; }
  if (!(textsList = new int[texts])) { fclose(layoutFile); return false; }
  for (text = textsLayout, label = textsList;
       text < textsLayout + texts;
       text++, label++)
  {
    if (fscanf(layoutFile,
               "%d %d %d %d %d",
               &textLeft, &textTop, &textRight, &textBottom,
	       label) != 5) { fclose(layoutFile); return false; }
    if (!fgets(comment, sizeof(comment), layoutFile))
		 { fclose(layoutFile); return false; }
    text->setCoords(textLeft, textTop, textRight, textBottom);
  }
 
  if (fscanf(layoutFile, "%d", &decorations) != 1)
	{ fclose(layoutFile); return false; }
  if (!fgets(comment, sizeof(comment), layoutFile))
	{ fclose(layoutFile); return false; }
  if (decorations < 1)
	{ fclose(layoutFile); return false; }

  if (!(objectsLayout = new QRect[decorations])) { fclose(layoutFile); return false; }
  if (!(shapesLayout = new QRect[decorations])) { fclose(layoutFile); return false; }
  if (!(soundsList = new int[decorations])) { fclose(layoutFile); return false; }
  for (object = objectsLayout, shape = shapesLayout, sound = soundsList;
       object < objectsLayout + decorations;
       object++, shape++, sound++)
  {
    if (fscanf(layoutFile,
               "%d %d %d %d  %d %d %d %d  %d",
               &objectLeft, &objectTop, &objectRight, &objectBottom,
               &shapeLeft, &shapeTop, &shapeRight, &shapeBottom,
	       sound) != 9)
		{ fclose(layoutFile); return false; }
    if (!fgets(comment, sizeof(comment), layoutFile))
		{ fclose(layoutFile); return false; }
    object->setCoords(objectLeft, objectTop, objectRight, objectBottom);
    shape->setCoords(shapeLeft, shapeTop, shapeRight, shapeBottom);
  }
  if (fclose(layoutFile)) return false;

  return true;
}

// Set up top level window's geometry
void TopLevel::setupGeometry()
{
  int width = gameboard.width() + XORIGIN + 2 * XMARGIN,
      height = gameboard.height() + YORIGIN + 2 * YMARGIN;
  setMinimumWidth(width);
  setMaximumWidth(width);
  setMinimumHeight(height);
  setMaximumHeight(height);
}

// Reset gameboard
void TopLevel::fileNew()
{
  toDraw.clear();

  QRect dirtyArea
	(editableArea.left() - 10,
         editableArea.top() - 10,
         editableArea.width() + 20,
         editableArea.height() + 20);
  repaint(dirtyArea, false);
}

// Load gameboard
void TopLevel::fileOpen()
{
  QString name;

  name = KFileDialog::getOpenFileName((const char *)
	(KApplication::kde_datadir() + "/ktuberling/museum"), "*.tuberling");
  toolbar->getButton(ID_OPEN)->setDown(false);
  if (!name.isEmpty())
  {
    QRect dirtyArea
	(editableArea.left() - 10,
         editableArea.top() - 10,
         editableArea.width() + 20,
         editableArea.height() + 20);
    toDraw.clear();
    if (!loadFrom(name))
      QMessageBox::warning
        (this,
         "KTuberling",
         i18n("Could not load file"));
    repaint(dirtyArea, false);
  }

}

// Save gameboard
void TopLevel::fileSave()
{
  QString name;

  name = KFileDialog::getSaveFileName((const char *)
	(KApplication::kde_datadir() + "/ktuberling/museum"), "*.tuberling");
  toolbar->getButton(ID_SAVE)->setDown(false);
  if (!name.isEmpty())
  {
    if (!saveAs(name))
      QMessageBox::warning
        (this,
         "KTuberling",
         i18n("Could not save file"));
  }
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  QString name;
  QPixmap picture(QPixmap::grabWindow
	(winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()));

  name = KFileDialog::getSaveFileName
		(getenv("HOME"),
		 i18n(	"*.xpm|Unix pixmaps (*.xpm)\n"
			"*.jpg|JPEG compressed files (*.jpg)\n"
			"*.png|Next generation pictures (*.png)\n"
			"*.bmp|Windows bitmaps (*.bmp)\n"
		 	"*|All picture formats"));
  if (!name.isEmpty())
  {
    char *format;
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
      QMessageBox::warning
        (this,
         "KTuberling",
         i18n("Unknown picture format"));
      return;
    }

    if (!picture.save(name, format))
      QMessageBox::warning
        (this,
         "KTuberling",
         i18n("Could not save file"));
  }
}

// Save gameboard as picture
void TopLevel::filePrint()
{
//  bool ok;

//  ok = QPrintDialog::getPrinterSetup(&printer);
  toolbar->getButton(ID_PRINT)->setDown(false);
//  if (!ok) return;
  if (!printPicture())
    QMessageBox::warning
        (this,
         "KTuberling",
         i18n("Could not print picture"));
  else
    QMessageBox::information
        (this,
         "KTuberling",
         i18n("Picture successfully printed"));

}

// Copy modified area to clipboard
void TopLevel::editCopy()
{
  QClipboard *clipboard = QApplication::clipboard();
  QPixmap picture(QPixmap::grabWindow
	(winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()));

  clipboard->setPixmap(picture);
}

// Undo last action
void TopLevel::editUndo()
{
}

// Redo last action
void TopLevel::editRedo()
{
}

// Toggle sound on/off
void TopLevel::optionsSound()
{
  soundEnabled = !soundEnabled;
  optionsMenu->setItemChecked( soundID, soundEnabled);
  writeOptions();
}

// Display about box
void TopLevel::aboutApp()
{
  QMessageBox::about
    (this,
     i18n("About ktuberling 0.1"),
     i18n("A program by Eric Bischoff (mailto:ebisch@cybercable.tm.fr)\n"
	  "and John Calhoun.\n\n"
	  "This program is dedicated to my daughter Sunniva."));
}

// Repaint event handling
void TopLevel::paintEvent(QPaintEvent *event)
{
  QPoint destination(event->rect().topLeft()),
         position(destination.x() - XMARGIN - XORIGIN,
                  destination.y() - YMARGIN - YORIGIN);
  QRect area(position, QSize(event->rect().size()));
  QBitmap cache(gameboard.size());
  QPainter artist(&cache);
  const ToDraw *currentObject;
  int text;

  if (destination.x() < XMARGIN + XORIGIN) destination.setX(XMARGIN + XORIGIN);
  if (destination.y() < YMARGIN + YORIGIN) destination.setY(YMARGIN + YORIGIN);
  area = QRect(0, 0, gameboard.width(), gameboard.height()).intersect(area);
  if (area.isEmpty()) return;

  artist.drawPixmap(area.topLeft(), gameboard, area);
  artist.setPen(color0);
  for (text = 0; text < texts; text++)
    drawText(artist, textsLayout[text], textsList[text]);

  artist.setPen(color1);

  for (currentObject = toDraw.first(); currentObject; currentObject = toDraw.next())
    currentObject->draw(artist, area, objectsLayout, shapesLayout, &gameboard, &masks);
  bitBlt(this, destination, &cache, area, CopyROP);
}

// Top level window close box interaction handling
void TopLevel::closeEvent(QCloseEvent *event)
{
  kapp->quit();
}

// Mouse button down event
void TopLevel::mousePressEvent(QMouseEvent *event)
{
  if (draggedCursor) return;

  QPoint position(event->x() - XMARGIN - XORIGIN,
                  event->y() - YMARGIN - YORIGIN);
  if (!zone(position)) return;

  QBitmap object(objectsLayout[draggedNumber].size()),
          shape(shapesLayout[draggedNumber].size());
  bitBlt(&object, QPoint(0, 0), &gameboard, objectsLayout[draggedNumber], CopyROP);
  bitBlt(&shape, QPoint(0, 0), &masks, shapesLayout[draggedNumber], CopyROP);

  if (!(draggedCursor = new QCursor
       (object, shape, position.x(), position.y()))) return;
  setCursor(*draggedCursor);

  playSound(soundsList[draggedNumber]);
}

// Mouse button up event
void TopLevel::mouseReleaseEvent(QMouseEvent *event)
{
  if (!draggedCursor) return;

  QCursor arrow;
  QRect position(
    event->x() - XMARGIN - XORIGIN - draggedCursor->hotSpot().x(),
    event->y() - YMARGIN - YORIGIN - draggedCursor->hotSpot().y(),
    objectsLayout[draggedNumber].width(),
    objectsLayout[draggedNumber].height()); 
  QRect dirtyArea
	(editableArea.left() - 10,
         editableArea.top() - 10,
         editableArea.width() + 20,
         editableArea.height() + 20);
  ToDraw *newObject;

  delete draggedCursor;
  draggedCursor = 0;
  setCursor(arrow);

  if (!dirtyArea.contains(event->pos())) return; 

  if (!(newObject = new ToDraw(draggedNumber, position))) return;
  toDraw.append(newObject);

  position.moveBy(XMARGIN + XORIGIN, YMARGIN + YORIGIN);
  repaint(position, false);

}

// In which decorative object are we?
// On return, the position is the location of the cursor's hot spot
// Returns false if we aren't in any zone
bool TopLevel::zone(QPoint &position)
{
  for (draggedNumber = 0;
       draggedNumber < decorations;
       draggedNumber++) if (objectsLayout[draggedNumber].contains(position))
  {
     position.setX(position.x() - objectsLayout[draggedNumber].x());
     position.setY(position.y() - objectsLayout[draggedNumber].y());

     return true;
  }

  const ToDraw *currentObject;

  for (currentObject = toDraw.last(); currentObject; currentObject = toDraw.prev())
  {
    if (!currentObject->getPosition().contains(position)) continue;

    QRect toUpdate(currentObject->getPosition());
    draggedNumber = currentObject->getNumber();

    QBitmap shape(shapesLayout[draggedNumber].size());
    QPoint relative(position.x() - toUpdate.x(),
                    position.y() - toUpdate.y());
    bitBlt(&shape, QPoint(0, 0), &masks, shapesLayout[draggedNumber], CopyROP);
    if (!shape.convertToImage().pixelIndex(relative.x(), relative.y())) continue;

    toDraw.remove(); 
    toUpdate.moveBy(XMARGIN + XORIGIN, YMARGIN + YORIGIN);
    repaint(toUpdate, false);

    position = relative;

    return true;
  }

  if (editableArea.contains(position))
	playSound(editableSound);

  return false;
}

// Load objects and lay them down on the gameboard
bool TopLevel::loadFrom(const char *name)
{
  FILE *fp;
  bool eof = false;
  ToDraw readObject, *currentObject;

  if (!(fp = fopen(name, "r"))) return false;
  for (;;)
  {
    if (!readObject.load(fp, decorations, eof))
    {
      fclose(fp);
      return false;
    }
    if (eof)
    {
      return !fclose(fp);
    }
    if (!(currentObject = new ToDraw(readObject))) return false;
    toDraw.append(currentObject);
  } 
}

// Save objects laid down on the gameboard 
bool TopLevel::saveAs(const char *name)
{
  FILE *fp;
  const ToDraw *currentObject;

  if (!(fp = fopen(name, "w"))) return false;
  for (currentObject = toDraw.first(); currentObject; currentObject = toDraw.next())
    if (!currentObject->save(fp))
    {
      fclose(fp);
      return false;
    }
  return !fclose(fp);
}

// Print gameboard's picture
bool TopLevel::printPicture() const
{
  QPrinter printer;
  QPainter artist;
  QPixmap picture(QPixmap::grabWindow
	(winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()));

  if (!artist.begin(&printer)) return false;
  artist.drawPixmap(QPoint(0, 0), picture);
  if (!artist.end()) return false;
  return true;
}

// Draw some text
void TopLevel::drawText(QPainter &artist, QRect &area, int textNumber) const
{
  const char *label;

  switch (textNumber)
  {
      case TEXT_EYES:
	label = i18n("Eyes");
	break;
      case TEXT_EYEBROWS:
	label = i18n("Eyesbrows");
	break;
      case TEXT_NOSES:
	label = i18n("Noses");
	break;
      case TEXT_EARS:
	label = i18n("Ears");
	break;
      case TEXT_MOUTHS:
	label = i18n("Mouths");
	break;
      case TEXT_GOODIES:
	label = i18n("Goodies");
	break;
      default:
	label = 0;
  }
  if (label) artist.drawText(area, AlignCenter, label);
}

// Play some sound
void TopLevel::playSound(int soundNumber) const
{
  const char *soundName;

  if (!soundEnabled) return;
  if (!audioServer) return;
  switch (soundNumber)
  {
    case SOUND_TUBERLING:
	soundName = i18n("en/tuberling.wav");
        break;
    case SOUND_EYE:
	soundName = i18n("en/eye.wav");
        break;
    case SOUND_EYEBROW:
	soundName = i18n("en/eyebrow.wav");
        break;
    case SOUND_NOSE:
	soundName = i18n("en/nose.wav");
        break;
    case SOUND_EAR:
	soundName = i18n("en/ear.wav");
        break;
    case SOUND_MOUTH:
	soundName = i18n("en/mouth.wav");
        break;
    case SOUND_HAT:
	soundName = i18n("en/hat.wav");
        break;
    case SOUND_MOUSTACHE:
	soundName = i18n("en/moustache.wav");
        break;
    case SOUND_CIGAR:
	soundName = i18n("en/cigar.wav");
        break;
    case SOUND_TIE:
	soundName = i18n("en/tie.wav");
        break;
    case SOUND_SUNGLASSES:
	soundName = i18n("en/sunglasses.wav");
        break;
    case SOUND_SPECTACLES:
	soundName = i18n("en/spectacles.wav");
        break;
    case SOUND_BOW:
	soundName = i18n("en/bow.wav");
        break;
    case SOUND_EARRING:
	soundName = i18n("en/earring.wav");
        break;
    case SOUND_BADGE:
	soundName = i18n("en/badge.wav");
        break;
    case SOUND_WATCH:
	soundName = i18n("en/watch.wav");
        break;
    default:
	return;
  }

  audioServer->play
	(KApplication::kde_datadir() + "/ktuberling/sounds/" + soundName);
}

