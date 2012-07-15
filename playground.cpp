/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007-2008 by Albert Astals Cid <aacid@kde.org>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/* Play ground widget */

#include "playground.h"

#include <klocale.h>
#include <kstandarddirs.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include <QCursor>
#include <QDataStream>
#include <QDomDocument>
#include <QFile>
#include <QGraphicsSvgItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPrinter>
#include <QFileInfo>

#include <kstandardaction.h>
#include <kactioncollection.h>
#include <kstandardshortcut.h>
#include <kicon.h>
#include <QAction>

#include "action.h"
#include "toplevel.h"
#include "todraw.h"

static const char *saveGameTextScaleTextMode = "KTuberlingSaveGameV2";
static const char *saveGameTextTextMode = "KTuberlingSaveGameV3";
static const char *saveGameText = "KTuberlingSaveGameV4";

// Constructor
PlayGround::PlayGround(TopLevel *parent)
    : QGraphicsView(parent), m_dragItem(0), m_scene(0), m_nextZValue(1), m_lockAspect(false)
{
  m_topLevel = parent;
  setFrameStyle(QFrame::NoFrame);
  setOptimizationFlag(QGraphicsView::DontSavePainterState, true); // all items here save the painter state
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

// Destructor
PlayGround::~PlayGround()
{
  foreach (QGraphicsScene *scene, m_sceneCache)
    delete scene;
  foreach (QUndoStack *undoStack, m_undoCache)
    delete undoStack;
}

// Reset the play ground
void PlayGround::reset()
{
  foreach(QGraphicsItem *item, m_scene->items())
  {
    ToDraw *currentObject = qgraphicsitem_cast<ToDraw *>(item);
    delete currentObject;
  }

  m_undoStack->clear();
}

// Save objects laid down on the editable area
bool PlayGround::saveAs(const QString & name)
{
  QFile f(name);
  if (!f.open( QIODevice::WriteOnly ) )
      return false;

  QFileInfo gameBoard(m_gameboardFile);
  QDataStream out(&f);
  out.setVersion(QDataStream::Qt_4_5);
  out << QString::fromLatin1(saveGameText);
  out << gameBoard.fileName();
  foreach(QGraphicsItem *item, m_scene->items())
  {
    ToDraw *currentObject = qgraphicsitem_cast<ToDraw *>(item);
    if (currentObject != NULL) currentObject->save(out);
  }

  return (f.error() == QFile::NoError);
}

// Print gameboard's picture
bool PlayGround::printPicture(QPrinter &printer)
{
  QPainter artist;
  QPixmap picture(getPicture());

  if (!artist.begin(&printer)) return false;
  artist.drawPixmap(QPoint(32, 32), picture);
  if (!artist.end()) return false;
  return true;
}

// Get a pixmap containing the current picture
QPixmap PlayGround::getPicture()
{
  QPixmap result(mapFromScene(backgroundRect()).boundingRect().size());
  QPainter artist(&result);
  scene()->render(&artist, QRectF(), backgroundRect(), Qt::IgnoreAspectRatio);
  artist.end();
  return result;
}

void PlayGround::connectRedoAction(QAction *action)
{
  connect(action, SIGNAL(triggered()), &m_undoGroup, SLOT(redo()));
  connect(&m_undoGroup, SIGNAL(canRedoChanged(bool)), action, SLOT(setEnabled(bool)));
}

void PlayGround::connectUndoAction(QAction *action)
{
  connect(action, SIGNAL(triggered()), &m_undoGroup, SLOT(undo()));
  connect(&m_undoGroup, SIGNAL(canUndoChanged(bool)), action, SLOT(setEnabled(bool)));
}

// Mouse pressed event
void PlayGround::mousePressEvent(QMouseEvent *event)
{
  if (!m_scene) return;

  if (event->button() != Qt::LeftButton) return;

  if (m_dragItem) placeDraggedItem(event->pos());
  else if (!m_pickedElement.isNull()) placeNewItem(event->pos());
  else
  {
    // see if the user clicked on the warehouse of items
    QPointF scenePos = mapToScene(event->pos());
    QMap<QString, QString>::const_iterator it, itEnd;
    it = m_objectsNameSound.constBegin();
    itEnd = m_objectsNameSound.constEnd();
    QString foundElem;
    QRectF bounds;
    for( ; foundElem.isNull() && it != itEnd; ++it)
    {
      bounds = m_SvgRenderer.boundsOnElement(it.key());
      if (bounds.contains(scenePos)) foundElem = it.key();
    }

    if (!foundElem.isNull())
    {
      bounds = mapFromScene(bounds).boundingRect();
      double objectScale = m_objectsNameRatio.value(foundElem);
      int width = qRound(bounds.width() * objectScale);
      int height = qRound(bounds.height() * objectScale);

      m_topLevel->playSound(m_objectsNameSound.value(foundElem));
      setCursor(QCursor(toPixmap(foundElem, width, height, &m_SvgRenderer)));
      m_pickedElement = foundElem;
    }
    else
    {
      // see if the user clicked on an already existent item
      QGraphicsItem *dragItem = m_scene->itemAt(mapToScene(event->pos()));
      m_dragItem = qgraphicsitem_cast<ToDraw*>(dragItem);
      if (m_dragItem)
      {
        QRectF rect = m_dragItem->unclippedRect();
        rect = m_dragItem->transform().mapRect(rect);
        QPolygon poly = mapFromScene(rect);
        QSize size = poly.boundingRect().size(); // the polygon should be a rect...
        QString elem = m_dragItem->elementId();
        setCursor(QCursor(toPixmap(elem, size.width(), size.height(), &m_SvgRenderer)));

        m_scene->removeItem(m_dragItem);
        m_topLevel->playSound(m_objectsNameSound.value(elem));
      }
    }
  }
}

bool PlayGround::insideBackground(const QSizeF &size, const QPointF &pos) const
{
  return backgroundRect().intersects(QRectF(pos, size));
}

QRectF PlayGround::backgroundRect() const
{
  return m_SvgRenderer.boundsOnElement(QLatin1String( "background" ));
}

void PlayGround::placeDraggedItem(const QPoint &pos)
{
  QPointF itemPos = mapToScene(pos);
  const QSizeF &elementSize = m_dragItem->transform().mapRect(m_dragItem->unclippedRect()).size();
  itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);

  if (insideBackground(elementSize, itemPos))
  {
    m_scene->addItem(m_dragItem);
    m_undoStack->push(new ActionMove(m_dragItem, itemPos, m_nextZValue, m_scene));
    m_nextZValue++;
  }
  else
  {
    m_undoStack->push(new ActionRemove(m_dragItem, m_scene));
  }

  setCursor(QCursor());
  m_dragItem = 0;
}

void PlayGround::placeNewItem(const QPoint &pos)
{
  double objectScale = m_objectsNameRatio.value(m_pickedElement);
  QSizeF elementSize = m_SvgRenderer.boundsOnElement(m_pickedElement).size() * objectScale;
  QPointF itemPos = mapToScene(pos);

  itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);

  if (insideBackground(elementSize, itemPos))
  {
    ToDraw *item = new ToDraw;
    item->setElementId(m_pickedElement);
    item->setPos(itemPos);
    item->setSharedRenderer(&m_SvgRenderer);
    item->setZValue(m_nextZValue);
    m_nextZValue++;
    item->scale(objectScale, objectScale);

    m_undoStack->push(new ActionAdd(item, m_scene));
  }

  setCursor(QCursor());
  m_pickedElement.clear();
}

void PlayGround::recenterView()
{
  // Cannot use sceneRect() because sometimes items get placed
  // with pos() outside rect (e.g. pizza theme)
  fitInView(QRect(QPoint(0,0), m_SvgRenderer.defaultSize()),
      m_lockAspect ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);
}

void PlayGround::resizeEvent(QResizeEvent *)
{
  recenterView();
}

void PlayGround::lockAspectRatio(bool lock)
{
  if (m_lockAspect != lock)
  {
    m_lockAspect = lock;
    recenterView();
  }
}

bool PlayGround::isAspectRatioLocked() const
{
  return m_lockAspect;
}

// Register the various playgrounds
void PlayGround::registerPlayGrounds()
{
  const QStringList list = KGlobal::dirs()->findAllResources("appdata", QLatin1String( "pics/*.theme" ));

  foreach(const QString &theme, list)
  {
    QFile layoutFile(theme);
    if (layoutFile.open(QIODevice::ReadOnly))
    {
      QDomDocument layoutDocument;
      if (layoutDocument.setContent(&layoutFile))
      {
        QString desktop = layoutDocument.documentElement().attribute(QLatin1String( "desktop" ));
        KConfig c( KStandardDirs::locate("appdata", QLatin1String( "pics/" ) ) + desktop);
        KConfigGroup cg = c.group("KTuberlingTheme");
        QString gameboard = layoutDocument.documentElement().attribute(QLatin1String( "gameboard" ));
        QPixmap pixmap(200,100);
        pixmap.fill(Qt::transparent);
        playGroundPixmap(gameboard,pixmap);
        m_topLevel->registerGameboard(cg.readEntry("Name"), theme, pixmap);
      }
    }
  }
}

void PlayGround::playGroundPixmap(const QString &playgroundName, QPixmap &pixmap)
{
  m_SvgRenderer.load(KStandardDirs::locate("appdata", QLatin1String( "pics/" ) + playgroundName));
  QPainter painter(&pixmap);
  m_SvgRenderer.render(&painter,QLatin1String( "background" ));
}

// Load background and draggable objects masks
bool PlayGround::loadPlayGround(const QString &gameboardFile)
{
  QDomNodeList playGroundsList,
               editableAreasList, objectsList,
               gameAreasList, maskAreasList, soundNamesList, labelsList;
  QDomElement playGroundElement,
              editableAreaElement, objectElement,
              gameAreaElement, maskAreaElement, soundNameElement, labelElement;
  QDomAttr gameboardAttribute, masksAttribute,
           leftAttribute, topAttribute, rightAttribute, bottomAttribute,
           refAttribute;

  QFile layoutFile(gameboardFile);
  if (!layoutFile.open(QIODevice::ReadOnly)) return false;

  QDomDocument layoutDocument;
  if (!layoutDocument.setContent(&layoutFile)) return false;

  playGroundElement = layoutDocument.documentElement();

  QString gameboardName = playGroundElement.attribute(QLatin1String( "gameboard" ));

  QColor bgColor = QColor(playGroundElement.attribute(QLatin1String( "bgcolor" ), QLatin1String( "#fff" ) ) );
  if (!bgColor.isValid())
    bgColor = Qt::white;

  if (!m_SvgRenderer.load(KStandardDirs::locate("appdata", QLatin1String( "pics/" ) + gameboardName)))
    return false;

  objectsList = playGroundElement.elementsByTagName(QLatin1String( "object" ));
  if (objectsList.count() < 1)
    return false;

  m_objectsNameSound.clear();

  //restore scene
  if(m_sceneCache.contains(gameboardFile))
  {
    m_scene = m_sceneCache[gameboardFile];
  }
  else
  {
    m_scene = new QGraphicsScene();
    m_sceneCache[gameboardFile] =  m_scene;

    QGraphicsSvgItem *background = new QGraphicsSvgItem();
    background->setPos(QPoint(0,0));
    background->setSharedRenderer(&m_SvgRenderer);
    background->setZValue(0);
    m_scene->addItem(background);
  }

  for (int decoration = 0; decoration < objectsList.count(); decoration++)
  {
    objectElement = (const QDomElement &) objectsList.item(decoration).toElement();

    const QString &objectName = objectElement.attribute(QLatin1String( "name" ));
    if (m_SvgRenderer.elementExists(objectName))
    {
      m_objectsNameSound.insert(objectName, objectElement.attribute(QLatin1String( "sound" )));
      m_objectsNameRatio.insert(objectName, objectElement.attribute(QLatin1String( "scale" ), QLatin1String( "1" )).toDouble());
    }
    else
    {
      kWarning() << objectName << "does not exist. Check" << gameboardFile;
    }
  }

  setBackgroundBrush(bgColor);
  m_gameboardFile = gameboardFile;
  setScene(m_scene);

  recenterView();

  //restore undo stack
  if(m_undoCache.contains(m_gameboardFile))
  {
    m_undoStack = m_undoCache[m_gameboardFile];
  }
  else
  {
    m_undoStack = new QUndoStack();
    m_undoCache[m_gameboardFile] = m_undoStack;
    m_undoGroup.addStack(m_undoStack);
  }

  m_undoGroup.setActiveStack(m_undoStack);

  return true;
}

QString PlayGround::currentGameboard() const
{
  return m_gameboardFile;
}

// Load objects and lay them down on the editable area
PlayGround::LoadError PlayGround::loadFrom(const QString &name)
{
  QFile f(name);
  if (!f.open(QIODevice::ReadOnly))
      return OtherError;

  QDataStream in(&f);
  in.setVersion(QDataStream::Qt_4_5);

  bool scale = false;
  bool reopenInTextMode = false;
  QString magicText;
  in >> magicText;
  if ( QLatin1String( saveGameTextScaleTextMode ) == magicText) {
      scale = true;
      reopenInTextMode = true;
  } else if (QLatin1String( saveGameTextTextMode ) == magicText) {
      reopenInTextMode = true;
  } else if ( QLatin1String( saveGameText ) != magicText) {
      return OldFileVersionError;
  }

  if (reopenInTextMode) {
      f.close();
      if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return OtherError;
      in.setDevice(&f);
      in.setVersion(QDataStream::Qt_4_5);
      in >> magicText;
  }

  sceneRect();

  if (in.atEnd())
    return OtherError;

  QString board;
  in >> board;

  qreal xFactor = 1.0;
  qreal yFactor = 1.0;
  m_topLevel->changeGameboard(board);

  reset();

  if (scale) {
    QSize defaultSize = m_SvgRenderer.defaultSize();
    QSize currentSize = size();
    xFactor = (qreal)defaultSize.width() / (qreal)currentSize.width();
    yFactor = (qreal)defaultSize.height() / (qreal)currentSize.height();
  }

  while ( !in.atEnd() )
  {
    ToDraw *obj = new ToDraw;
    if (!obj->load(in))
    {
      delete obj;
      return OtherError;
    }
    obj->setSharedRenderer(&m_SvgRenderer);
    double objectScale = m_objectsNameRatio.value(obj->elementId());
    obj->scale(objectScale, objectScale);
    if (scale) { // Mimic old behavior
      QPointF storedPos = obj->pos();
      storedPos.setX(storedPos.x() * xFactor);
      storedPos.setY(storedPos.y() * yFactor);
      obj->setPos(storedPos);
    }
    m_undoStack->push(new ActionAdd(obj, m_scene));
  }
  if (f.error() == QFile::NoError) return NoError;
  else return OtherError;
}

