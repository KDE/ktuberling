/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */


#ifndef _PLAYGROUND_H_
#define _PLAYGROUND_H_

#include <kprinter.h>

#include <qwidget.h>
#include <qbitmap.h>
#include <qptrlist.h>

#include "todraw.h"
#include "action.h"

class QDomDocument;
class TopLevel;

class PlayGround : public QWidget
{
  Q_OBJECT

public:

  PlayGround(TopLevel *parent, const char *name, uint selectedGameboard);
  ~PlayGround();

  void reset();
  void change(uint selectedGameboard);
  void repaintAll();
  bool undo();
  bool redo();
  bool loadFrom(const QString &name);
  bool saveAs(const QString &name);
  bool printPicture(KPrinter &printer) const;
  QPixmap getPicture() const;

  inline bool isFirstAction() const { return currentAction == 0; }
  inline bool isLastAction() const { return currentAction >= history.count(); }

protected:

  virtual void paintEvent(QPaintEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

private:

  bool registerPlayGrounds(QDomDocument &layoutDocument);
  bool loadPlayGround(QDomDocument &layoutDocument, uint toLoad);
  void loadFailure();
  void setupGeometry();
  bool zone(QPoint &position);
  void drawText(QPainter &artist, QRect &area, QString &textId) const;
  void drawGameboard(QPainter &artist, const QRect &area) const;

private:

  QPixmap gameboard;		// Picture of the game board
  QBitmap masks;		// Pictures of the objects' shapes
  QRect editableArea;           // Part of the gameboard where the player can lay down objects
  QString menuItem,		// Menu item describing describing this gameboard
          editableSound;        // Sound associated with this area
  int texts,                    // Number of categories of objects names
      decorations;              // Number of draggable objects on the right side of the gameboard
  QRect *textsLayout,           // Positions of the categories names
        *objectsLayout;         // Position of the draggable objects on right side of the gameboard
  QString *textsList,           // List of the message numbers associated with categories
          *soundsList;          // List of sounds associated with each object

  QCursor *draggedCursor;       // Cursor's shape for currently dragged object
  ToDraw draggedObject;         // Object currently dragged
  int draggedZOrder;            // Z-order (in to-draw buffer) of this object

  QPtrList<ToDraw> toDraw;      // List of objects in z-order
  QPtrList<Action> history;     // List of actions in chronological order
  unsigned int currentAction;   // Number of current action (not the last one if used "undo" button!)

  TopLevel *topLevel;		// Top-level window
};

#endif
