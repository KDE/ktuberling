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
#include <qlist.h>

#include "todraw.h"
#include "action.h"

class TopLevel;

class PlayGround : public QWidget
{
  Q_OBJECT

public:

  PlayGround( TopLevel *parent, const char *name );
  ~PlayGround();

  void reset();
  void repaintAll();
  bool undo();
  bool redo();
  bool loadFrom(const QString &);
  bool saveAs(const QString &);
  bool printPicture(KPrinter &) const;

  inline bool isFirstAction() const { return currentAction == 0; }
  inline bool isLastAction() const { return currentAction >= history.count(); }
  inline QPixmap grabWindow() const { return QPixmap::grabWindow
        (winId(),
         editableArea.left(),
         editableArea.top(),
         editableArea.width(),
         editableArea.height()); }

protected:

  virtual void paintEvent( QPaintEvent *event );
  virtual void mousePressEvent( QMouseEvent *event );
  virtual void mouseReleaseEvent( QMouseEvent *event );

  bool loadBitmaps();
  void setupGeometry();

private:

  bool zone(QPoint &);
  void drawText(QPainter &, QRect &, int) const;
  void playSound(int) const;

private:

  QPixmap gameboard;		// Picture of the game board
  QBitmap masks;		// Pictures of the objects' shapes
  QRect editableArea;           // Part of the gameboard where the player can lay down objects
  int editableSound;            // Sound associated with this area
  int texts,                    // Number of categories of objects names
      decorations;              // Number of draggable objects on the right side of the gameboard
  QRect *textsLayout,           // Positions of the categories names
        *objectsLayout,         // Position of the draggable objects on right side of the gameboard
        *shapesLayout;          // Position of the shapes of these objects in the masks file
  int *textsList,               // List of the message numbers associated with categories
      *soundsList;              // List of sounds associated with each object

  QCursor *draggedCursor;       // Cursor's shape for currently dragged object
  ToDraw draggedObject;         // Object currently dragged
  int draggedZOrder;            // Z-order (in to-draw buffer) of this object

  QList<ToDraw> toDraw;         // List of objects in z-order
  QList<Action> history;        // List of actions in chronological order
  unsigned int currentAction;   // Number of current action (not the last one if used "undo" button!)

  TopLevel *topLevel;		// Top-level window
};

#endif
