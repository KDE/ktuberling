/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <kmessagebox.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kaudioplayer.h>

#include <qfile.h>
#include <qpainter.h>
#include <qimage.h>
#include <qprinter.h>

#include <stdlib.h>

#include "playground.h"
#include "toplevel.h"
#include "categories.h"

#define XMARGIN 5
#define YMARGIN 5

// Constructor
PlayGround::PlayGround( TopLevel *parent, const char *name )
    : QWidget( parent, name )
{
  topLevel = parent;

  objectsLayout = shapesLayout = 0;
  textsList = soundsList = 0;

  toDraw.setAutoDelete(true);
  history.setAutoDelete(true);

  setBackgroundColor(white);

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
}

// Destructor
PlayGround::~PlayGround()
{
  if (soundsList) delete soundsList;
  if (textsList) delete textsList;
  if (objectsLayout) delete objectsLayout;
  if (shapesLayout) delete shapesLayout;
  if (draggedCursor) delete draggedCursor;
}

// Reset the play ground
void PlayGround::reset()
{
  toDraw.clear();
  history.clear();
  currentAction = 0;
}

// Repaint all the editable area
void PlayGround::repaintAll()
{
  QRect dirtyArea
        (editableArea.left() - 10,
         editableArea.top() - 10,
         editableArea.width() + 20,
         editableArea.height() + 20);

  repaint(dirtyArea, false);
}

// Undo last action
// Returns true if everything went fine
bool PlayGround::undo()
{
  ToDraw *newObject;
  Action *undone;
  int zOrder;

  if (!(undone = history.at(--currentAction)))
    return false;

  zOrder = undone->ZOrderAfter();
  if (zOrder != -1)
  {
    // Undo an "add" or a "move" action
    if (!toDraw.remove(zOrder)) return false;
  }

  zOrder = undone->ZOrderBefore();
  if (zOrder != -1)
  {
    // Undo a "delete" or a "move" action
    if (!(newObject = new ToDraw(undone->DrawnBefore())))
      return false;
    if (!toDraw.insert(zOrder, newObject))
      return false;
  }

  return true;
}

// Redo next action
// Returns true if everything went fine
bool PlayGround::redo()
{
  ToDraw *newObject;
  Action *undone;
  int zOrder;

  if (!(undone = history.at(currentAction++)))
    return false;

  zOrder = undone->ZOrderBefore();
  if (zOrder != -1)
  {
    // Redo a "delete" or a "move" action
    if (!toDraw.remove(zOrder)) return false;
  }

  zOrder = undone->ZOrderAfter();
  if (zOrder != -1)
  {
    // Redo an "add" or a "move" action
    if (!(newObject = new ToDraw(undone->DrawnAfter())))
      return false;
    if (!toDraw.insert(zOrder, newObject))
      return false;
  }

  return true;
}

// Save objects laid down on the editable area
bool PlayGround::saveAs(const QString & name)
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
bool PlayGround::printPicture(QPrinter &printer) const
{
  QPainter artist;
  QPixmap picture(grabWindow());

  if (!artist.begin(&printer)) return false;
  artist.drawPixmap(QPoint(32, 32), picture);
  if (!artist.end()) return false;
  return true;
}

// Draw some text
void PlayGround::drawText(QPainter &artist, QRect &area, int textNumber) const
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
void PlayGround::playSound(int soundNumber) const
{
  QString soundName;

  if (!topLevel->isSoundEnabled()) return;
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

// Painting event
void PlayGround::paintEvent( QPaintEvent *event )
{
  QPoint destination(event->rect().topLeft()),
         position(destination.x() - XMARGIN,
                  destination.y() - YMARGIN);
  QRect area(position, QSize(event->rect().size()));
  QBitmap cache(gameboard.size());
  QPainter artist(&cache);
  const ToDraw *currentObject;
  int text;

  if (destination.x() < XMARGIN) destination.setX(XMARGIN);
  if (destination.y() < YMARGIN) destination.setY(YMARGIN);
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

// Mouse pressed event
void PlayGround::mousePressEvent( QMouseEvent *event )
{
  if (draggedCursor) return;

  QPoint position(event->x() - XMARGIN,
                  event->y() - YMARGIN);
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

// Mouse released event
void PlayGround::mouseReleaseEvent( QMouseEvent *event )
{
  // If we are not dragging an object, ignore the event
  if (!draggedCursor) return;

  QCursor arrow;
  int draggedNumber = draggedObject.getNumber();
  QRect position(
    event->x() - XMARGIN - draggedCursor->hotSpot().x(),
    event->y() - YMARGIN - draggedCursor->hotSpot().y(),
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
    topLevel->enableUndo(true);

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
  topLevel->enableUndo(true);

  // Repaint the editable area
  position.moveBy(XMARGIN, YMARGIN);
  repaint(position, false);

}

// Load background and draggable objects masks
bool PlayGround::loadBitmaps()
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
        (XMARGIN + editableLeft,
         YMARGIN + editableTop,
         XMARGIN + editableRight,
         YMARGIN + editableBottom);

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

// Set up play ground's geometry
void PlayGround::setupGeometry()
{
  int width = gameboard.width() + 2 * XMARGIN,
      height = gameboard.height() + 2 * YMARGIN;
  setFixedWidth( width );
  setFixedHeight( height );
}

// In which decorative object are we?
// On return, the position is the location of the cursor's hot spot
// Returns false if we aren't in any zone
bool PlayGround::zone(QPoint &position)
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
    toUpdate.moveBy(XMARGIN, YMARGIN);
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
bool PlayGround::loadFrom(const QString & name)
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
