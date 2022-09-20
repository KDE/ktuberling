/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Play ground widget */

#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

#include <QGraphicsView>
#include <QMap>

#include <QSvgRenderer>
#include <QUndoGroup>


class Action;
class ToDraw;
class QPagedPaintDevice;

class PlayGroundCallbacks
{
public:
  virtual ~PlayGroundCallbacks() {}
  virtual void playSound(const QString &ref) = 0;
  virtual void changeGameboard(const QString &gameboard) = 0;
  virtual void registerGameboard(const QString& menuText, const QString& boardFile, const QPixmap& pixmap) = 0;
};

class PlayGround : public QGraphicsView
{
  Q_OBJECT

public:
  explicit PlayGround(PlayGroundCallbacks *callbacks, QWidget *parent = nullptr);
  ~PlayGround() override;

  enum LoadError { NoError, OldFileVersionError, OtherError };

  void reset();
  LoadError loadFrom(const QString &name);
  bool saveAs(const QString &name);
  bool printPicture(QPagedPaintDevice &printer);
  QPixmap getPicture();

  void connectRedoAction(QAction *action);
  void connectUndoAction(QAction *action);

  void registerPlayGrounds();
  bool loadPlayGround(const QString &gameboardFile);

  void setAllowOnlyDrag(bool allowOnlyDrag);

  QString currentGameboard() const;

  bool isAspectRatioLocked() const;

public Q_SLOTS:
  void lockAspectRatio(bool lock);

protected:

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

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

  PlayGroundCallbacks *m_callbacks;
  QString m_gameboardFile;				// the file the board
  QMap<QString, QString> m_objectsNameSound;		// map between element name and sound
  QMap<QString, double> m_objectsNameRatio;		// map between element name and scaling ratio

  QPoint m_mousePressPos;
  QPointF m_itemDraggedPos;
  ToDraw *m_newItem;				    // the new item we are moving
  ToDraw *m_dragItem;					// the existing item we are dragging
  QSvgRenderer m_SvgRenderer;				// the SVG renderer
  int m_nextZValue;					// the next Z value to use

  bool m_lockAspect;					// whether we are locking aspect ratio
  bool m_allowOnlyDrag;
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
