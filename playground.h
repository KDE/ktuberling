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

#include <KSvgRenderer>
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
  void resizeEvent(QResizeEvent *event);

private:
  QRectF backgroundRect() const;
  bool insideBackground(const QSizeF &size, const QPointF &pos) const;
  void placeDraggedItem(const QPoint &pos);
  void placeNewItem(const QPoint &pos);

  void recenterView();

  QString m_gameboardFile;				// the file the board
  QMap<QString, QString> m_objectsNameSound;		// map between element name and sound
  QMap<QString, double> m_objectsNameRatio;		// map between element name and scaling ratio

  QUndoStack *m_undoStack;				// the command stack
  TopLevel *m_topLevel;					// Top-level window

  QString m_pickedElement;				// the SVG element the cursor is
  ToDraw *m_dragItem;					// the item we are dragging
  QGraphicsScene *m_scene;				// the graphicsScene
  KSvgRenderer m_SvgRenderer;				// the SVG renderer
  int m_nextZValue;					// the next Z value to use

  bool m_lockAspect;					// whether we are locking aspect ratio
  QMap <QString, QGraphicsScene *> m_sceneCache;
  QMap <QString, QUndoStack *> m_undoCache;                 // caches the items of each playground
  QUndoGroup m_undoGroup;
};

#endif
