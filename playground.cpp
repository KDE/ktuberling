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
    : QGraphicsView(parent), m_newItem(0), m_dragItem(0), m_nextZValue(1), m_lockAspect(false)
{
  m_topLevel = parent;
  setFrameStyle(QFrame::NoFrame);
  setOptimizationFlag(QGraphicsView::DontSavePainterState, true); // all items here save the painter state
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMouseTracking(true);
}

// Destructor
PlayGround::~PlayGround()
{
  foreach (const SceneData &data, m_scenes)
  {
    delete data.scene;
    delete data.undoStack;
  }
}

// Reset the play ground
void PlayGround::reset()
{
    
  foreach(QGraphicsItem *item, scene()->items())
  {
    ToDraw *currentObject = qgraphicsitem_cast<ToDraw *>(item);
    delete currentObject;
  }

  undoStack()->clear();
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
  foreach(QGraphicsItem *item, scene()->items())
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
  if (m_gameboardFile.isEmpty()) return;

  if (event->button() != Qt::LeftButton) return;

  if (m_dragItem) placeDraggedItem(event->pos());
  else if (m_newItem) placeNewItem(event->pos());
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
      const double objectScale = m_objectsNameRatio.value(foundElem);
      const QSizeF elementSize = m_SvgRenderer.boundsOnElement(foundElem).size() * objectScale;
      QPointF itemPos = mapToScene(event->pos());
      itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);

      m_topLevel->playSound(m_objectsNameSound.value(foundElem));

      m_newItem = new ToDraw;
      m_newItem->setBeingDragged(true);
      m_newItem->setPos(clipPos(itemPos, m_newItem));
      m_newItem->setSharedRenderer(&m_SvgRenderer);
      m_newItem->setElementId(foundElem);
      m_newItem->setZValue(m_nextZValue);
      m_nextZValue++;
      m_newItem->scale(objectScale, objectScale);

      scene()->addItem(m_newItem);
      setCursor(Qt::BlankCursor);
    }
    else
    {
      // see if the user clicked on an already existent item
      QGraphicsItem *dragItem = scene()->itemAt(mapToScene(event->pos()));
      m_dragItem = qgraphicsitem_cast<ToDraw*>(dragItem);
      if (m_dragItem)
      {
        QString elem = m_dragItem->elementId();

        m_topLevel->playSound(m_objectsNameSound.value(elem));
        setCursor(Qt::BlankCursor);
        m_dragItem->setBeingDragged(true);
        m_itemDraggedPos = m_dragItem->pos();

        const QSizeF elementSize = m_dragItem->transform().mapRect(m_dragItem->unclippedRect()).size();
        QPointF itemPos = mapToScene(event->pos());
        itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);
        m_dragItem->setPos(clipPos(itemPos, m_dragItem));
      }
    }
  }
}

void PlayGround::mouseMoveEvent(QMouseEvent *event)
{
  if (m_newItem) {
    QPointF itemPos = mapToScene(event->pos());
    const QSizeF elementSize = m_newItem->transform().mapRect(m_newItem->unclippedRect()).size();
    itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);

    m_newItem->setPos(clipPos(itemPos, m_newItem));
  } else if (m_dragItem) {
    QPointF itemPos = mapToScene(event->pos());
    const QSizeF elementSize = m_dragItem->transform().mapRect(m_dragItem->unclippedRect()).size();
    itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);

    m_dragItem->setPos(clipPos(itemPos, m_dragItem));
  }
}

bool PlayGround::insideBackground(const QSizeF &size, const QPointF &pos) const
{
  return backgroundRect().intersects(QRectF(pos, size));
}

QPointF PlayGround::clipPos(const QPointF &p, ToDraw *item) const
{
  const qreal objectScale = m_objectsNameRatio.value(item->elementId());

  QPointF res = p;
  res.setX(qMax(qreal(0), res.x()));
  res.setY(qMax(qreal(0), res.y()));
  res.setX(qMin(m_SvgRenderer.defaultSize().width() - item->boundingRect().width() * objectScale, res.x()));
  res.setY(qMin(m_SvgRenderer.defaultSize().height()- item->boundingRect().height() * objectScale, res.y()));
  return res;
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
    m_dragItem->setBeingDragged(false);
    undoStack()->push(new ActionMove(m_dragItem, m_itemDraggedPos, m_nextZValue, scene()));
    m_nextZValue++;
  }
  else
  {
    undoStack()->push(new ActionRemove(m_dragItem, scene()));
  }

  setCursor(QCursor());
  m_dragItem = 0;
}

void PlayGround::placeNewItem(const QPoint &pos)
{
  const QSizeF elementSize = m_newItem->transform().mapRect(m_newItem->unclippedRect()).size();
  QPointF itemPos = mapToScene(pos);
  itemPos -= QPointF(elementSize.width()/2, elementSize.height()/2);
  if (insideBackground(elementSize, itemPos))
  {
    m_newItem->setBeingDragged(false);
    undoStack()->push(new ActionAdd(m_newItem, scene()));
  } else {
    m_newItem->deleteLater();
  }
  m_newItem = 0;
  setCursor(QCursor());
}

void PlayGround::recenterView()
{
  // Cannot use sceneRect() because sometimes items get placed
  // with pos() outside rect (e.g. pizza theme)
  fitInView(QRect(QPoint(0,0), m_SvgRenderer.defaultSize()),
      m_lockAspect ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);
}

QGraphicsScene *PlayGround::scene() const
{
  return m_scenes[m_gameboardFile].scene;
}

QUndoStack *PlayGround::undoStack() const
{
  return m_scenes[m_gameboardFile].undoStack;
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

  // create scene data if needed
  if(!m_scenes.contains(gameboardFile))
  {
    SceneData &data = m_scenes[gameboardFile];
    data.scene = new QGraphicsScene();
    data.undoStack = new QUndoStack();

    QGraphicsSvgItem *background = new QGraphicsSvgItem();
    background->setPos(QPoint(0,0));
    background->setSharedRenderer(&m_SvgRenderer);
    background->setZValue(0);
    data.scene->addItem(background);

    m_undoGroup.addStack(data.undoStack);
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
  setScene(scene());

  recenterView();

  m_undoGroup.setActiveStack(undoStack());

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
    scene()->addItem(obj);
    undoStack()->push(new ActionAdd(obj, scene()));
  }
  if (f.error() == QFile::NoError) return NoError;
  else return OtherError;
}

/* kate: replace-tabs on; indent-width 2; */
