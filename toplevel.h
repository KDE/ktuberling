/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kmainwindow.h>

class PlayGround;

class TopLevel : public KMainWindow
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

  void enableUndo(bool) const;
  void enableRedo(bool) const;
  void registerGameboard(const QString &, const char *);
  void changeGameboard( uint newGameboard );

  inline bool isSoundEnabled() const {return soundEnabled;}

protected:

  void readOptions();
  void writeOptions();
  void setupKAction();

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
  void optionsGameboard0();
  void optionsGameboard1();
  void optionsGameboard2();
  void optionsGameboard3();
  void optionsGameboard4();
  void optionsGameboard5();
  void optionsGameboard6();
  void optionsGameboard7();

private:

  int                           // Menu items identificators
      newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      soundID;
  enum {                        // Tool bar buttons identificators
      ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT,
      ID_UNDO, ID_REDO,
      ID_HELP };

  bool soundEnabled;            // True if the sound is enabled by user, even if there is no audio server
  uint selectedGameboard,	// Number of currently selected gameboard
       gameboards;		// Total number of gameboards
  QString gameboardActions[8];	// Name of actions for registered gameboards

  PlayGround *playGround;	// Play ground central widget
};

#endif
