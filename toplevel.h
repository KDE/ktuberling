/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kapp.h>
#include <ktmainwindow.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <mediatool.h>
#include <kaudio.h>

#include <qpopmenu.h>
#include <qaccel.h>
#include <qbitmap.h>

#include "todraw.h"

class TopLevel : public KTMainWindow
{
  Q_OBJECT
  
public:

  TopLevel();
  ~TopLevel();

protected:

  void readOptions();
  void writeOptions();
  void setupSound();
  void setupMenuBar();
  void setupToolBar();
  bool loadBitmaps();
  void setupGeometry();

  virtual void paintEvent(QPaintEvent *);
  virtual void closeEvent(QCloseEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);

private:

  bool zone(QPoint &);
  bool loadFrom(const char *);
  bool saveAs(const char *);
  bool printPicture() const;
  void drawText(QPainter &, QRect &, int) const;
  void playSound(int) const;

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
  void aboutApp();

private:

  KMenuBar *menubar;
  KToolBar *toolbar;
  QPopupMenu *fileMenu, *editMenu, *optionsMenu, *helpMenu;
  int newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      soundID,
      contentsID, aboutID, aboutKDEID;
  int ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT, ID_UNDO, ID_REDO, ID_HELP;

  KAudio *audioServer;
  bool soundEnabled;

  QBitmap gameboard, masks;
  QRect editableArea;
  int editableSound;
  int texts, decorations;
  QRect *textsLayout, *objectsLayout, *shapesLayout;
  int *textsList,
      *soundsList;
  QCursor *draggedCursor;
  int draggedNumber;
  QList<ToDraw> toDraw;

};

#endif
