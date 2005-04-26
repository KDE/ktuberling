/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include <stdlib.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kprinter.h>

#include <qfile.h>
#include <qpainter.h>
#include <qimage.h>
#include <qcursor.h>
#include <qdom.h>

#include "playground.moc"
#include "toplevel.h"

#define XMARGIN 5
#define YMARGIN 5

// Constructor
PlayGround::PlayGround(TopLevel *parent, const char *name, uint selectedGameboard)
    : QWidget(parent, name)
{
  topLevel = parent;

  textsLayout = objectsLayout = 0;
  textsList = soundsList = 0;
  draggedCursor = 0;

  toDraw.setAutoDelete(true);
  history.setAutoDelete(true);

  setBackgroundColor(white);

  QDomDocument layoutsDocument;
  bool ok = topLevel->loadLayout(layoutsDocument);
  if (ok) ok = registerPlayGrounds(layoutsDocument);
  if (ok) ok = loadPlayGround(layoutsDocument, selectedGameboard);
  if (!ok) loadFailure();

  currentAction = 0;
  setupGeometry();
}

// Destructor
PlayGround::~PlayGround()
{
  delete [] textsLayout;
  delete [] objectsLayout;

  delete [] textsList;
  delete [] soundsList;

  delete draggedCursor;
}

// Reset the play ground
void PlayGround::reset()
{
  toDraw.clear();
  history.clear();
  currentAction = 0;
}

// Change the gameboard
void PlayGround::change(uint selectedGameboard)
{
  QDomDocument layoutsDocument;
  bool ok = topLevel->loadLayout(layoutsDocument);
  if (ok) ok = loadPlayGround(layoutsDocument, selectedGameboard);
  if (!ok) loadFailure();

  toDraw.clear();
  history.clear();
  currentAction = 0;

  setupGeometry();

  update();
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
    newObject = new ToDraw(undone->DrawnBefore());
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
    newObject = new ToDraw(undone->DrawnAfter());
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

  if (!(fp = fopen((const char *) QFile::encodeName(name), "w"))) return false;

  fprintf(fp, "%d\n", topLevel->getSelectedGameboard());
  for (currentObject = toDraw.first(); currentObject; currentObject = toDraw.next())
    currentObject->save(fp);

  return !fclose(fp);
}

// Print gameboard's picture
bool PlayGround::printPicture(KPrinter &printer) const
{
  QPainter artist;
  QPixmap picture(getPicture());

  if (!artist.begin(&printer)) return false;
  artist.drawPixmap(QPoint(32, 32), picture);
  if (!artist.end()) return false;
  return true;
}

// Get a pixmap containing the current picture
QPixmap PlayGround::getPicture() const
{
  QPixmap result(editableArea.size());
  QPainter artist(&result);
  QRect transEditableArea(editableArea);

  transEditableArea.moveBy(-XMARGIN, -YMARGIN);
  artist.translate(XMARGIN - editableArea.left(),
                   YMARGIN - editableArea.top());
  drawGameboard(artist, transEditableArea);
  return result;
}

// Draw some text
void PlayGround::drawText(QPainter &artist, QRect &area, QString &textId) const
{
  QString label;

  label=i18n(textId.latin1());

  artist.drawText(area, AlignCenter, label);
}

// Paint the current picture to the given device
void PlayGround::drawGameboard( QPainter &artist, const QRect &area ) const
{
  artist.drawPixmap(area.topLeft(), gameboard, area);

  artist.setPen(white);
  for (int text = 0; text < texts; text++)
    drawText(artist, textsLayout[text], textsList[text]);

  artist.setPen(black);
  for (QPtrListIterator<ToDraw> currentObject(toDraw);
       currentObject.current();
       ++currentObject)
    currentObject.current()->draw(artist, area, objectsLayout, &gameboard, &masks);
}

// Painting event
void PlayGround::paintEvent( QPaintEvent *event )
{
  QPoint destination(event->rect().topLeft()),
         position(destination.x() - XMARGIN,
                  destination.y() - YMARGIN);
  QRect area(position, QSize(event->rect().size()));
  QPixmap cache(gameboard.size());
  QPainter artist(&cache);

  if (destination.x() < XMARGIN) destination.setX(XMARGIN);
  if (destination.y() < YMARGIN) destination.setY(YMARGIN);
  area = QRect(0, 0, gameboard.width(), gameboard.height()).intersect(area);
  if (area.isEmpty()) return;

  drawGameboard(artist, area);

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
  QPixmap object(objectsLayout[draggedNumber].size());
  QBitmap shape(objectsLayout[draggedNumber].size());
  bitBlt(&object, QPoint(0, 0), &gameboard, objectsLayout[draggedNumber], Qt::CopyROP);
  bitBlt(&shape, QPoint(0, 0), &masks, objectsLayout[draggedNumber], Qt::CopyROP);
  object.setMask(shape);

  draggedCursor = new QCursor(object, position.x(), position.y());
  setCursor(*draggedCursor);

  topLevel->playSound(soundsList[draggedNumber]);
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
    newAction = new Action(&draggedObject, draggedZOrder, 0, -1);
    history.append(newAction);
    currentAction++;
    topLevel->enableUndo(true);

    return;
  }

  // Register that we have one more object to draw
  newObject = new ToDraw(draggedNumber, position);
  toDraw.append(newObject);

  // Forget all subsequent actions in the undo buffer, and register object's addition (or its move)
  while (history.count() > currentAction) history.removeLast();
  newAction = new Action(&draggedObject, draggedZOrder, newObject, toDraw.count()-1);
  history.append(newAction);
  currentAction++;
  topLevel->enableUndo(true);

  // Repaint the editable area
  position.moveBy(XMARGIN, YMARGIN);
  repaint(position, false);

}

// Register the various playgrounds
bool PlayGround::registerPlayGrounds(QDomDocument &layoutDocument)
{
  QDomNodeList playGroundsList, menuItemsList, labelsList;
  QDomElement playGroundElement, menuItemElement, labelElement;
  QDomAttr actionAttribute;

  playGroundsList = layoutDocument.elementsByTagName("playground");
  if (playGroundsList.count() < 1)
    return false;

  for (uint i = 0; i < playGroundsList.count(); i++)
  {
    playGroundElement = (const QDomElement &) playGroundsList.item(i).toElement();

    menuItemsList = playGroundElement.elementsByTagName("menuitem");
    if (menuItemsList.count() != 1)
      return false;

    menuItemElement = (const QDomElement &) menuItemsList.item(0).toElement();

    labelsList = menuItemElement.elementsByTagName("label");
    if (labelsList.count() != 1)
      return false;

    labelElement = (const QDomElement &) labelsList.item(0).toElement();
    actionAttribute = menuItemElement.attributeNode("action");
    topLevel->registerGameboard(labelElement.text(), actionAttribute.value().latin1()); 
  }
 
  return true;
}

// Load background and draggable objects masks
bool PlayGround::loadPlayGround(QDomDocument &layoutDocument, uint toLoad)
{
  QDomNodeList playGroundsList,
               editableAreasList, categoriesList, objectsList,
               gameAreasList, maskAreasList, soundNamesList, labelsList;
  QDomElement playGroundElement,
              editableAreaElement, categoryElement, objectElement,
              gameAreaElement, maskAreaElement, soundNameElement, labelElement;
  QDomAttr gameboardAttribute, masksAttribute,
           leftAttribute, topAttribute, rightAttribute, bottomAttribute,
	   refAttribute;

  playGroundsList = layoutDocument.elementsByTagName("playground");
  if (toLoad >= playGroundsList.count())
    return false;

  playGroundElement = (const QDomElement &) playGroundsList.item(toLoad).toElement();

  gameboardAttribute = playGroundElement.attributeNode("gameboard");
  if (!gameboard.load(locate("data", "ktuberling/pics/" + gameboardAttribute.value())))
    return false;

  masksAttribute = playGroundElement.attributeNode("masks");
  if (!masks.load(locate("data", "ktuberling/pics/" + masksAttribute.value())))
    return false;

  editableAreasList = playGroundElement.elementsByTagName("editablearea");
  if (editableAreasList.count() != 1)
    return false;

  editableAreaElement = (const QDomElement &) editableAreasList.item(0).toElement();

  gameAreasList = editableAreaElement.elementsByTagName("position");
  if (gameAreasList.count() != 1)
    return false;

  gameAreaElement = (const QDomElement &) gameAreasList.item(0).toElement();
  leftAttribute = gameAreaElement.attributeNode("left");
  topAttribute = gameAreaElement.attributeNode("top");
  rightAttribute = gameAreaElement.attributeNode("right");
  bottomAttribute = gameAreaElement.attributeNode("bottom");

  editableArea.setCoords
        (XMARGIN + leftAttribute.value().toInt(),
         YMARGIN + topAttribute.value().toInt(),
         XMARGIN + rightAttribute.value().toInt(),
         YMARGIN + bottomAttribute.value().toInt());

  soundNamesList = editableAreaElement.elementsByTagName("sound");
  if (soundNamesList.count() != 1)
    return false;

  soundNameElement = (const QDomElement &) soundNamesList.item(0).toElement();
  refAttribute = soundNameElement.attributeNode("ref");

  editableSound = refAttribute.value();

  categoriesList = playGroundElement.elementsByTagName("category");
  texts = categoriesList.count();
  if (texts < 1)
    return false;

  delete[] textsLayout;
  textsLayout = new QRect[texts];
  delete[] textsList;
  textsList = new QString[texts];

  for (int text = 0; text < texts; text++)
  {
    categoryElement = (const QDomElement &) categoriesList.item(text).toElement();

    gameAreasList = categoryElement.elementsByTagName("position");
    if (gameAreasList.count() != 1)
      return false;

    gameAreaElement = (const QDomElement &) gameAreasList.item(0).toElement();
    leftAttribute = gameAreaElement.attributeNode("left");
    topAttribute = gameAreaElement.attributeNode("top");
    rightAttribute = gameAreaElement.attributeNode("right");
    bottomAttribute = gameAreaElement.attributeNode("bottom");

    textsLayout[text].setCoords
	    (leftAttribute.value().toInt(),
	     topAttribute.value().toInt(),
	     rightAttribute.value().toInt(),
	     bottomAttribute.value().toInt());

    labelsList = categoryElement.elementsByTagName("label");
    if (labelsList.count() != 1)
      return false;

    labelElement = (const QDomElement &) labelsList.item(0).toElement();

    textsList[text] = labelElement.text();
  }

  objectsList = playGroundElement.elementsByTagName("object");
  decorations = objectsList.count();
  if (decorations < 1)
    return false;

  delete[] objectsLayout;
  objectsLayout = new QRect[decorations];
  delete[] soundsList;
  soundsList = new QString[decorations];

  for (int decoration = 0; decoration < decorations; decoration++)
  {
    objectElement = (const QDomElement &) objectsList.item(decoration).toElement();

    gameAreasList = objectElement.elementsByTagName("position");
    if (gameAreasList.count() != 1)
      return false;

    gameAreaElement = (const QDomElement &) gameAreasList.item(0).toElement();
    leftAttribute = gameAreaElement.attributeNode("left");
    topAttribute = gameAreaElement.attributeNode("top");
    rightAttribute = gameAreaElement.attributeNode("right");
    bottomAttribute = gameAreaElement.attributeNode("bottom");

    objectsLayout[decoration].setCoords
	    (leftAttribute.value().toInt(),
	     topAttribute.value().toInt(),
	     rightAttribute.value().toInt(),
	     bottomAttribute.value().toInt());

    soundNamesList = objectElement.elementsByTagName("sound");
    if (soundNamesList.count() != 1)
      return false;

    soundNameElement = (const QDomElement &) soundNamesList.item(0).toElement();

    refAttribute = soundNameElement.attributeNode("ref");

    soundsList[decoration] = refAttribute.value();
  }

  return true;
}

// Report a load failure
void PlayGround::loadFailure()
{
  KMessageBox::error(topLevel, i18n("Fatal error:\n"
				    "Unable to load the pictures, aborting."));
  exit(-1);
}

// Set up play ground's geometry
void PlayGround::setupGeometry()
{
  int width = gameboard.width() + 2 * XMARGIN,
      height = gameboard.height() + 2 * YMARGIN;
  setFixedWidth(width);
  setFixedHeight(height);
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

    QBitmap shape(objectsLayout[draggedNumber].size());
    QPoint relative(position.x() - toUpdate.x(),
                    position.y() - toUpdate.y());
    bitBlt(&shape, QPoint(0, 0), &masks, objectsLayout[draggedNumber], Qt::CopyROP);
    if (!shape.convertToImage().pixelIndex(relative.x(), relative.y())) continue;

    toDraw.remove(draggedZOrder);
    toUpdate.moveBy(XMARGIN, YMARGIN);
    repaint(toUpdate, false);

    position = relative;

    return true;
  }

  // If we are on the gameboard itself, then play "tuberling" sound
  if (editableArea.contains(position))
        topLevel->playSound(editableSound);

  return false;
}

// Load objects and lay them down on the editable area
bool PlayGround::loadFrom(const QString &name)
{
  FILE *fp;
  bool eof = false;
  ToDraw readObject, *newObject;
  Action *newAction;

  if (!(fp = fopen(QFile::encodeName(name), "r"))) return false;

  uint newGameboard;
  int nitems = fscanf(fp, "%u\n", &newGameboard);
  if (nitems == EOF)
  {
    fclose(fp);
    return false;
  }
  topLevel->changeGameboard(newGameboard);

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
    newObject = new ToDraw(readObject);
    toDraw.append(newObject);
    newAction = new Action(0, -1, newObject, toDraw.count()-1);
    history.append(newAction);
    currentAction++;

  }
}
