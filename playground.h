/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */


#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

#include <QGraphicsView>
#include <QMap>
#include <QUndoStack>

#include <KSvgRenderer>

class QDomDocument;

class KPrinter;

class Action;
class ToDraw;
class TopLevel;

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
  bool printPicture(KPrinter &printer);
  QPixmap getPicture();

  QAction *getRedoAction();
  QAction *getUndoAction();

  bool registerPlayGrounds(QDomDocument &layoutDocument);
  bool loadPlayGround(QDomDocument &layoutDocument, const QString &gameboardName);
  
  QString currentGameboard() const;

protected:

  void mousePressEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);

private:
  QRectF backgroundRect() const;
  bool insideBackground(const QPoint &pos) const;
  void placeDraggedItem(const QPoint &pos);
  void placeNewItem(const QPoint &pos);
  void adjustItems(const QSize &size, const QSize &oldSize, bool changePos);
  void fontSize(const QString &text, const QRectF &rect, int *fontSize, QRect *usedRect);

  QString m_gameboardName;						// the name of the board file
  QMap<QString, QString> m_objectsNameSound;	// map between element name and sound

  QUndoStack m_undoStack;						// the command stack
  TopLevel *m_topLevel;							// Top-level window

  QString m_pickedElement;						// the SVG element the cursor is
  ToDraw *m_dragItem;							// the item we are dragging
  QGraphicsScene *m_scene;						// the graphicsScene
  KSvgRenderer m_SvgRenderer;					// the SVG renderer
};

#endif
