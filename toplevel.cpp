/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <qpainter.h>
#include <qimage.h>
#include <qlist.h>
#include <qclipboard.h>
#include <qprintdialog.h>
#include <qprinter.h>

#include <kmessagebox.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kstdaccel.h>
#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <ktoolbarbutton.h>
#include <kglobal.h>
#include <kaudioplayer.h>

#include <kio/netaccess.h>

#include "toplevel.h"
#include "categories.h"
#include <kconfig.h>

#include <stdlib.h>

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
  objectsLayout = shapesLayout = 0;
  textsList = soundsList = 0;

  toDraw.setAutoDelete(true);
  history.setAutoDelete(true);

  if (!loadBitmaps())
  {
    KMessageBox::error(this,
		       i18n("Fatal error:\n" 
			    "I could not load the pictures. I'll quit."));
    exit(-1);
  }

  draggedCursor = 0;
  currentAction = 0;

  setupGeometry();
  setupMenuBar();
  setupToolBar();
}

// Destructor
TopLevel::~TopLevel()
{
  if (soundsList) delete soundsList;
  if (textsList) delete textsList;
  if (objectsLayout) delete objectsLayout;
  if (shapesLayout) delete shapesLayout;
  if (draggedCursor) delete draggedCursor;
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
  menubar->setGeometry(0, 0, width(), 24);

  fileMenu = new QPopupMenu();
  editMenu = new QPopupMenu();
  optionsMenu = new QPopupMenu();

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
  KIconLoader iconLoader;

  toolbar = toolBar();
  toolbar->setGeometry(0, 24, width(), 24);

  toolbar->insertButton(iconLoader.loadIcon("filenew",KIcon::Toolbar), ID_NEW, SIGNAL(pressed()), this, SLOT(fileNew()), true, i18n("New"));
  toolbar->insertButton(iconLoader.loadIcon("fileopen",KIcon::Toolbar), ID_OPEN, SIGNAL(pressed()), this, SLOT(fileOpen()), true, i18n("Open"));
  toolbar->insertButton(iconLoader.loadIcon("filefloppy",KIcon::Toolbar), ID_SAVE, SIGNAL(pressed()), this, SLOT(fileSave()), true, i18n("Save"));
  toolbar->insertButton(iconLoader.loadIcon("fileprint",KIcon::Toolbar), ID_PRINT, SIGNAL(pressed()), this, SLOT(filePrint()), true, i18n("Print"));
  toolbar->insertSeparator();

  toolbar->insertButton(iconLoader.loadIcon("undo",KIcon::Toolbar), ID_UNDO, SIGNAL(pressed()), this, SLOT(editUndo()), false, i18n("Undo"));
  toolbar->insertButton(iconLoader.loadIcon("redo",KIcon::Toolbar), ID_REDO, SIGNAL(pressed()), this, SLOT(editRedo()), false, i18n("Redo"));
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

  if (!gameboard.load(locate("data", "ktuberling/pics/gameboard.xpm")))
	return false;

  if (!masks.load(locate("data", "ktuberling/pics/masks.xpm")))
	return false;

  if (!(layoutFile = fopen(QFile::encodeName(locate("data", "ktuberling/pics/layout.txt")), "r")))
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
  history.clear();
  currentAction = 0;

  enableUndo(false);
  enableRedo(false);
  repaintAll();
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
  
  toDraw.clear();
  history.clear();
  currentAction = 0;

  if (!loadFrom(name))
    KMessageBox::error(this, i18n("Could not load file."));

  enableUndo(currentAction != 0);
  enableRedo(false);
  repaintAll();

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
 
  if( !saveAs( url.path() ) )
    KMessageBox::error(this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  QPixmap picture(QPixmap::grabWindow
	(winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()));

  KURL url = KFileDialog::getSaveURL
		(getenv("HOME"),
		 i18n(	"*.xpm|Unix pixmaps (*.xpm)\n"
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
  repaint(true);
  if (!printPicture(printer))
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
  ToDraw *newObject;
  Action *undone;
  int zOrder;

  if (!currentAction) return;
  toolbar->getButton(ID_UNDO)->setDown(false);
  if (!(undone = history.at(--currentAction)))
    return;

  zOrder = undone->ZOrderAfter();
  if (zOrder != -1)
  {
    // Undo an "add" or a "move" action
    if (!toDraw.remove(zOrder)) return;
  }

  zOrder = undone->ZOrderBefore();
  if (zOrder != -1)
  {
    // Undo a "delete" or a "move" action
    if (!(newObject = new ToDraw(undone->DrawnBefore())))
      return;
    if (!toDraw.insert(zOrder, newObject))
      return;
  }

  if (!currentAction) enableUndo(false);
  enableRedo(true);
  repaintAll();
}

// Redo last action
void TopLevel::editRedo()
{
  ToDraw *newObject;
  Action *undone;
  int zOrder;

  if (currentAction >= history.count()) return;
  toolbar->getButton(ID_REDO)->setDown(false);
  if (!(undone = history.at(currentAction++)))
    return;

  zOrder = undone->ZOrderBefore();
  if (zOrder != -1)
  {
    // Redo a "delete" or a "move" action
    if (!toDraw.remove(zOrder)) return;
  }

  zOrder = undone->ZOrderAfter();
  if (zOrder != -1)
  {
    // Redo an "add" or a "move" action
    if (!(newObject = new ToDraw(undone->DrawnAfter())))
      return;
    if (!toDraw.insert(zOrder, newObject))
      return;
  }

  if (currentAction >= history.count()) enableRedo(false);
  enableUndo(true);
  repaintAll();
}

// Toggle sound on/off
void TopLevel::optionsSound()
{
  soundEnabled = !soundEnabled;
  optionsMenu->setItemChecked( soundID, soundEnabled);
  writeOptions();
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
  bitBlt(this, destination, &cache, area, Qt::CopyROP);
}

// Top level window close box interaction handling
void TopLevel::closeEvent(QCloseEvent *)
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

  int draggedNumber = draggedObject.getNumber();
  QBitmap object(objectsLayout[draggedNumber].size()),
          shape(shapesLayout[draggedNumber].size());
  bitBlt(&object, QPoint(0, 0), &gameboard, objectsLayout[draggedNumber], Qt::CopyROP);
  bitBlt(&shape, QPoint(0, 0), &masks, shapesLayout[draggedNumber], Qt::CopyROP);

  if (!(draggedCursor = new QCursor
       (object, shape, position.x(), position.y()))) return;
  setCursor(*draggedCursor);

  playSound(soundsList[draggedNumber]);
}

// Mouse button up event
void TopLevel::mouseReleaseEvent(QMouseEvent *event)
{
  // If we are not dragging an object, ignore the event
  if (!draggedCursor) return;

  QCursor arrow;
  int draggedNumber = draggedObject.getNumber();
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
  Action *newAction;

  // We are not anymore dragging an object
  delete draggedCursor;
  draggedCursor = 0;
  setCursor(arrow);

  // If we are not moving the object to the editable area
  if (!dirtyArea.contains(event->pos()))
  {
    // ... then register its deletion (if coming from the editable area), and return
    if (draggedZOrder == -1) return;

    while (history.count() > currentAction) history.removeLast();
    if (!(newAction = new Action(&draggedObject, draggedZOrder, 0, -1))) return;
    history.append(newAction);
    currentAction++;
    enableUndo(true);

    return; 
  }

  // Register that we have one more object to draw
  if (!(newObject = new ToDraw(draggedNumber, position))) return;
  toDraw.append(newObject);

  // Forget all subsequent actions in the undo buffer, and register object's addition (or its move)
  while (history.count() > currentAction) history.removeLast();
  if (!(newAction = new Action(&draggedObject, draggedZOrder, newObject, toDraw.count()-1))) return;
  history.append(newAction);
  currentAction++;
  enableUndo(true);

  // Repaint the editable area
  position.moveBy(XMARGIN + XORIGIN, YMARGIN + YORIGIN);
  repaint(position, false);

}

// In which decorative object are we?
// On return, the position is the location of the cursor's hot spot
// Returns false if we aren't in any zone
bool TopLevel::zone(QPoint &position)
{
  // Scan all available decorative objects on right side because we may be adding one
  int draggedNumber;
  for (draggedNumber = 0;
       draggedNumber < decorations;
       draggedNumber++) if (objectsLayout[draggedNumber].contains(position))
  {
     position.setX(position.x() - objectsLayout[draggedNumber].x());
     position.setY(position.y() - objectsLayout[draggedNumber].y());

     draggedObject.setNumber(draggedNumber);
     draggedZOrder = -1;

     return true;
  }

  // Scan all decorative objects already layed down on editable are because we may be moving or removing one
  const ToDraw *currentObject;

  for (draggedZOrder = toDraw.count()-1; draggedZOrder >= 0; draggedZOrder--)
  {
    currentObject = toDraw.at(draggedZOrder);
    if (!currentObject->getPosition().contains(position)) continue;

    QRect toUpdate(currentObject->getPosition());
    draggedObject = *currentObject;
    draggedNumber = draggedObject.getNumber();

    QBitmap shape(shapesLayout[draggedNumber].size());
    QPoint relative(position.x() - toUpdate.x(),
                    position.y() - toUpdate.y());
    bitBlt(&shape, QPoint(0, 0), &masks, shapesLayout[draggedNumber], Qt::CopyROP);
    if (!shape.convertToImage().pixelIndex(relative.x(), relative.y())) continue;

    toDraw.remove(draggedZOrder); 
    toUpdate.moveBy(XMARGIN + XORIGIN, YMARGIN + YORIGIN);
    repaint(toUpdate, false);

    position = relative;

    return true;
  }

  // If we are on the gameboard itself, then play "tuberling" sound 
  if (editableArea.contains(position))
	playSound(editableSound);

  return false;
}

// Load objects and lay them down on the editable area
bool TopLevel::loadFrom(const QString & name)
{
  FILE *fp;
  bool eof = false;
  ToDraw readObject, *newObject;
  Action *newAction;

  if (!(fp = fopen(QFile::encodeName(name), "r"))) return false;
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
    if (!(newObject = new ToDraw(readObject))) return false;
    toDraw.append(newObject);
    if (!(newAction = new Action(0, -1, newObject, toDraw.count()-1))) return false;
    history.append(newAction);
    currentAction++;
    
  } 
}

// Save objects laid down on the editable area
bool TopLevel::saveAs(const QString & name)
{
  FILE *fp;
  const ToDraw *currentObject;

  if (!(fp = fopen(QFile::encodeName(name), "w"))) return false;
  for (currentObject = toDraw.first(); currentObject; currentObject = toDraw.next())
    if (!currentObject->save(fp))
    {
      fclose(fp);
      return false;
    }
  return !fclose(fp);
}

// Print gameboard's picture
bool TopLevel::printPicture(QPrinter &printer) const
{
  QPainter artist;
  QPixmap picture(QPixmap::grabWindow
	(winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()));

  if (!artist.begin(&printer)) return false;
  artist.drawPixmap(QPoint(32, 32), picture);
  if (!artist.end()) return false;
  return true;
}

// Draw some text
void TopLevel::drawText(QPainter &artist, QRect &area, int textNumber) const
{
  QString label;

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
	return;
  }
  artist.drawText(area, AlignCenter, label);
}

// Play some sound
void TopLevel::playSound(int soundNumber) const
{
  QString soundName;

  if (!soundEnabled) return;
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

  KAudioPlayer::play(locate("data", "ktuberling/sounds/" + soundName));
}

// Repaint all the editable area
void TopLevel::repaintAll()
{
  QRect dirtyArea
	(editableArea.left() - 10,
         editableArea.top() - 10,
         editableArea.width() + 20,
         editableArea.height() + 20);

  repaint(dirtyArea, false);
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
