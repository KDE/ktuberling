/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <ktmainwindow.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>

#include <qpopmenu.h>
#include <qaccel.h>
#include <qbitmap.h>

#include "todraw.h"
#include "action.h"

class MainWidget;

class TopLevel : public KTMainWindow
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

  void doPaintEvent(QPaintEvent *);
  void doMousePressEvent(QMouseEvent *);
  void doMouseReleaseEvent(QMouseEvent *);

protected:

  void readOptions();
  void writeOptions();
  void setupMenuBar();
  void setupToolBar();
  bool loadBitmaps();
  void setupGeometry();

  virtual void closeEvent(QCloseEvent *);

private:

  bool zone(QPoint &);
  bool loadFrom(const QString &);
  bool saveAs(const QString &);
  bool printPicture(QPrinter &) const;
  void drawText(QPainter &, QRect &, int) const;
  void playSound(int) const;
  void repaintAll();
  void enableUndo(bool) const;
  void enableRedo(bool) const;

private slots:

  void fileNew();
  void fileOpen();
  void fileSave();
  void filePicture();
  void filePrint();
  void editCopy();
  void editUndo();
  void editRedo();
  void optionsSound();
//  void aboutApp();

private:

  KMenuBar *menubar;            // Task window's menubar
  KToolBar *toolbar;            // Task window's tool bar
  QPopupMenu *fileMenu,         // Various menus
             *editMenu,
             *optionsMenu;
  int                           // Menu items identificators
      newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      soundID;
  int                           // Tool bar buttons identificators
      ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT,
      ID_UNDO, ID_REDO,
      ID_HELP;

  bool soundEnabled;            // true if the sound is enabled by user, even if there is no audio server

  QBitmap gameboard, masks;     // Bitmaps of the game board and the objects' shapes
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
  int draggedZOrder;            // Z-order (in 'toDraw buffer) of this object

  QList<ToDraw> toDraw;         // List of objects in z-order
  QList<Action> history;        // List of actions in chronological order
  unsigned int currentAction;   // Number of current action (not the last one if used "undo" button!)

    MainWidget *mainWidget;
};

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget( TopLevel *parent, const char *name );

protected:
    virtual void paintEvent( QPaintEvent *event );
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseReleaseEvent( QMouseEvent *event );

    TopLevel *topLevel;
};

#endif
