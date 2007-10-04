/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
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
#include <kprinter.h>

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QPainter>
#include <QCursor>
#include <QDomDocument>
#include <QGraphicsSvgItem>
#include <QMouseEvent>

#include "action.h"
#include "toplevel.h"
#include "todraw.h"

static const char *saveGameText = "KTuberlingSaveGameV2";

// Constructor
PlayGround::PlayGround(TopLevel *parent)
    : QGraphicsView(parent), m_dragItem(0), m_scene(0), m_nextZValue(1)
{
  m_topLevel = parent;
  setFrameStyle(QFrame::NoFrame);
}

// Destructor
PlayGround::~PlayGround()
{
  delete m_scene;
}

// Reset the play ground
void PlayGround::reset()
{
  foreach(QGraphicsItem *item, m_scene->items())
  {
    ToDraw *currentObject = qgraphicsitem_cast<ToDraw *>(item);
    if (currentObject != NULL) delete currentObject;
  }

  m_undoStack.clear();
}

// Save objects laid down on the editable area
bool PlayGround::saveAs(const QString & name)
{
  QFile f(QFile::encodeName(name));
  if (!f.open( QIODevice::WriteOnly | QIODevice::Text ) )
      return false;

  QDataStream out(&f);
  out << QString(saveGameText);
  out << m_gameboardFile;
  foreach(QGraphicsItem *item, m_scene->items())
  {
    ToDraw *currentObject = qgraphicsitem_cast<ToDraw *>(item);
    if (currentObject != NULL) currentObject->save(out);
  }

  return (f.error() == QFile::NoError);
}

// Print gameboard's picture
bool PlayGround::printPicture(KPrinter &printer)
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
  QPixmap result(backgroundRect().size().toSize());
  QPainter artist(&result);
  render(&artist, QRectF(), backgroundRect().toRect());
  return result;
}

QAction *PlayGround::createRedoAction(KActionCollection *ac)
{
  return m_undoStack.createRedoAction(ac);
}

QAction *PlayGround::createUndoAction(KActionCollection *ac)
{
  return m_undoStack.createUndoAction(ac);
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
    QSize defaultSize = m_SvgRenderer.defaultSize();
    QSize currentSize = size();
    double xFactor = (double)defaultSize.width() / (double)currentSize.width();
    double yFactor = (double)defaultSize.height() / (double)currentSize.height();
    QMap<QString, QString>::const_iterator it, itEnd;
    it = m_objectsNameSound.constBegin();
    itEnd = m_objectsNameSound.constEnd();
    QString foundElem;
    QRectF bounds;
    for( ; foundElem.isNull() && it != itEnd; ++it)
    {
      bounds = m_SvgRenderer.boundsOnElement(it.key());
      bounds.setRect(bounds.x() / xFactor,
                     bounds.y() / yFactor,
                     bounds.width() / xFactor,
                     bounds.height() / yFactor);
      if (bounds.contains(event->pos())) foundElem = it.key();
    }

    if (!foundElem.isNull()) 
    {
      double objectScale = m_objectsNameRatio.value(foundElem);
      int width = qRound(bounds.width() * objectScale);
      int height = qRound(bounds.height() * objectScale);

      m_topLevel->playSound(m_objectsNameSound.value(foundElem));
      setCursor(QCursor(QPixmap::fromImage(toImage(foundElem, width, height, &m_SvgRenderer))));
      m_pickedElement = foundElem;
    }
    else
    {
      // see if the user clicked on an already existant item
      QGraphicsItem *dragItem = m_scene->itemAt(event->pos());
      m_dragItem = qgraphicsitem_cast<ToDraw*>(dragItem);
      if (m_dragItem)
      {
        QRectF rect = m_dragItem->boundingRect();
        rect = m_dragItem->transform().mapRect(rect);
        QSize size = rect.size().toSize();
        QString elem = m_dragItem->elementId();
        setCursor(QCursor(QPixmap::fromImage(toImage(elem, size.width(), size.height(), &m_SvgRenderer))));

        m_scene->removeItem(m_dragItem);
      }
    }
  }
}

bool PlayGround::insideBackground(const QPoint &pos) const
{
  return backgroundRect().contains(pos);
}

QRectF PlayGround::backgroundRect() const
{
  QSize defaultSize = m_SvgRenderer.defaultSize();
  QSize currentSize = size();
  QRectF bounds = m_SvgRenderer.boundsOnElement("background");
  bounds.setRect(bounds.x() / (double)defaultSize.width() * (double)currentSize.width(),
                 bounds.y() / (double)defaultSize.height() * (double)currentSize.height(),
                 bounds.width() / (double)defaultSize.width() * (double)currentSize.width(),
                 bounds.height() / (double)defaultSize.height() * (double)currentSize.height());
  return bounds;
}

void PlayGround::placeDraggedItem(const QPoint &pos)
{
  if (insideBackground(pos))
  {
    QPoint itemPos(pos.x() - cursor().pixmap().size().width() / 2,
                   pos.y() - cursor().pixmap().size().height() / 2);
    m_scene->addItem(m_dragItem);
    m_undoStack.push(new ActionMove(m_dragItem, itemPos, m_nextZValue, m_scene));
    m_nextZValue++;
  }
  else
  {
    m_undoStack.push(new ActionRemove(m_dragItem, m_scene));
  }

  setCursor(QCursor());
  m_dragItem = 0;
}

void PlayGround::placeNewItem(const QPoint &pos)
{
  if (insideBackground(pos))
  {
    QPoint itemPos(pos.x() - cursor().pixmap().size().width() / 2,
                   pos.y() - cursor().pixmap().size().height() / 2);
    ToDraw *item = new ToDraw();
    m_allCreatedItems << item;
    item->setElementId(m_pickedElement);
    item->setPos(itemPos);
    item->setSharedRenderer(&m_SvgRenderer);
    item->setZValue(m_nextZValue);
    m_nextZValue++;
    QTransform t;
    QSize defaultSize = m_SvgRenderer.defaultSize();
    double objectScale = m_objectsNameRatio.value(m_pickedElement);
    t.scale(objectScale, objectScale);
    t.scale((double)size().width() / (double)defaultSize.width(),
          (double)size().height() / (double)defaultSize.height());
    item->setTransform(t);

    m_undoStack.push(new ActionAdd(item, m_scene));
  }

  setCursor(QCursor());
  m_pickedElement.clear();
}

void PlayGround::resizeEvent(QResizeEvent *event)
{
  adjustItems(event->size(), event->oldSize(), true);
}

void PlayGround::adjustItems(const QSize &size, const QSize &oldSize, bool changePos)
{
  if (!m_scene) return;

  m_scene->setSceneRect(QRect(QPoint(0, 0), size));

  QSize defaultSize = m_SvgRenderer.defaultSize();
  double xScale = (double)size.width() / (double)defaultSize.width();
  double yScale = (double)size.height() / (double)defaultSize.height();

  QTransform t;
  t.scale(xScale, yScale);

  double xPositionScale, yPositionScale;
  if (changePos)
  {
    xPositionScale = (double)size.width() / (double)oldSize.width();
    yPositionScale = (double)size.height() / (double)oldSize.height();
  }

  foreach(QGraphicsItem *item, m_allCreatedItems)
  {
    QGraphicsSvgItem *svg = qgraphicsitem_cast<QGraphicsSvgItem *>(item);
    if (svg) item->setTransform(t); // just the background
    else
    {
      svg = qgraphicsitem_cast<ToDraw *>(item);
      if (svg)
      {
        QTransform t2 = t;
        double objectScale = m_objectsNameRatio.value(svg->elementId());
        t2.scale(objectScale, objectScale);
        item->setTransform(t2);
      }
    }

    if (changePos) item->setPos(item->x() * xPositionScale, item->y() * yPositionScale);
  }
}

// Register the various playgrounds
void PlayGround::registerPlayGrounds()
{
  QStringList list = KGlobal::dirs()->findAllResources("appdata", "pics/*.theme");

  foreach(const QString &theme, list)
  {
    QFile layoutFile(theme);
    if (layoutFile.open(QIODevice::ReadOnly))
    {
      QDomDocument layoutDocument;
      if (layoutDocument.setContent(&layoutFile))
      {
        QString desktop = layoutDocument.documentElement().attribute("desktop");
        KConfig c(KStandardDirs::locate("appdata", "pics/" + desktop));
        KConfigGroup cg = c.group("KTuberlingTheme");
        QString gameboard = layoutDocument.documentElement().attribute("gameboard");
        m_topLevel->registerGameboard(cg.readEntry("Name"), theme);
      }
    }
  }
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

  QString gameboardName = playGroundElement.attribute("gameboard");

  if (!m_SvgRenderer.load(KStandardDirs::locate("appdata", "pics/" + gameboardName)))
    return false;

  objectsList = playGroundElement.elementsByTagName("object");
  if (objectsList.count() < 1)
    return false;

  m_objectsNameSound.clear();
  for (int decoration = 0; decoration < objectsList.count(); decoration++)
  {
    objectElement = (const QDomElement &) objectsList.item(decoration).toElement();

    const QString &objectName = objectElement.attribute("name");
    if (m_SvgRenderer.elementExists(objectName))
    {
      m_objectsNameSound.insert(objectName, objectElement.attribute("sound"));
      m_objectsNameRatio.insert(objectName, objectElement.attribute("scale", "1").toDouble());
    }
    else
    {
      kWarning() << objectName << "does not exist. Check" << gameboardFile;
    }
  }

  delete m_scene;
  m_allCreatedItems.clear();
  m_scene = new QGraphicsScene();
  setScene(m_scene);

  m_gameboardFile = gameboardFile;

  QGraphicsSvgItem *background = new QGraphicsSvgItem();
  background->setPos(QPoint(0,0));
  background->setSharedRenderer(&m_SvgRenderer);
  background->setZValue(0);
  m_scene->addItem(background);
  m_allCreatedItems << background;

  adjustItems(size(), QSize(), false);

  return true;
}

QString PlayGround::currentGameboard() const
{
  return m_gameboardFile;
}

// Load objects and lay them down on the editable area
PlayGround::LoadError PlayGround::loadFrom(const QString &name)
{
  QFile f( QFile::encodeName(name) );
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      return OtherError;

  QDataStream in(&f);
  
  QString magicText;
  in >> magicText;
  if (saveGameText != magicText)
      return OldFileVersionError;

  if (in.atEnd())
    return OtherError;

  QString board;
  in >> board;

  if (in.atEnd())
    return OtherError;
  
  m_topLevel->changeGameboard(board);

  QTransform t;
  QSize defaultSize = m_SvgRenderer.defaultSize();
  t.scale((double)size().width() / (double)defaultSize.width(),
          (double)size().height() / (double)defaultSize.height());
  while ( !in.atEnd() )
  {
    ToDraw *obj = new ToDraw();
    if (!obj->load(in))
    {
      delete obj;
      return OtherError;
    }
    m_allCreatedItems << obj;
    obj->setSharedRenderer(&m_SvgRenderer);
    QTransform t2 = t;
    double objectScale = m_objectsNameRatio.value(obj->elementId());
    t2.scale(objectScale, objectScale);
    obj->setTransform(t2);
    m_undoStack.push(new ActionAdd(obj, m_scene));
  }
  if (f.error() == QFile::NoError) return NoError;
  else return OtherError;
}

#include "playground.moc"
