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

#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

#include <QGraphicsView>
#include <QMap>

#include <QSvgRenderer>
#include <QUndoGroup>

class KActionCollection;

class Action;
class ToDraw;
class TopLevel;

class QGraphicsSvgItem;

class PlayGround : public QGraphicsView
{
  Q_OBJECT

public:
  PlayGround(TopLevel *parent);
  ~PlayGround();

  enum LoadError { NoError, OldFileVersionError, OtherError };

  void reset();
  LoadError loadFrom(const QString &name);
  bool saveAs(const QString &name);
  bool printPicture(QPrinter &printer);
  QPixmap getPicture();

  void connectRedoAction(QAction *action);
  void connectUndoAction(QAction *action);

  void registerPlayGrounds();
  bool loadPlayGround(const QString &gameboardFile);

  QString currentGameboard() const;

  bool isAspectRatioLocked() const;

public Q_SLOTS:
  void lockAspectRatio(bool lock);

protected:

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);

private:
  QPointF clipPos(const QPointF &p, ToDraw *item) const;
  QRectF backgroundRect() const;
  bool insideBackground(const QSizeF &size, const QPointF &pos) const;
  void placeDraggedItem(const QPoint &pos);
  void placeNewItem(const QPoint &pos);
  void playGroundPixmap(const QString &playgroundName, QPixmap &pixmap);

  void recenterView();
  
  QGraphicsScene *scene() const;
  QUndoStack *undoStack() const;

  QString m_gameboardFile;				// the file the board
  QMap<QString, QString> m_objectsNameSound;		// map between element name and sound
  QMap<QString, double> m_objectsNameRatio;		// map between element name and scaling ratio

  TopLevel *m_topLevel;					// Top-level window

  QPointF m_itemDraggedPos;
  ToDraw *m_newItem;				    // the new item we are moving
  ToDraw *m_dragItem;					// the existing item we are dragging
  QSvgRenderer m_SvgRenderer;				// the SVG renderer
  int m_nextZValue;					// the next Z value to use

  bool m_lockAspect;					// whether we are locking aspect ratio
  QUndoGroup m_undoGroup;
  
  class SceneData
  {
    public:
      QGraphicsScene *scene;
      QUndoStack *undoStack;
  };
  QMap <QString, SceneData> m_scenes;  // caches the items of each playground
};

#endif

/* kate: replace-tabs on; indent-width 2; */
