/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kmainwindow.h>
#include <kurl.h>

class QDomDocument;
class PlayGround;
class SoundFactory;

class TopLevel : public KMainWindow
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

  void open(const KURL &url);
  void enableUndo(bool enable) const;
  void enableRedo(bool enable) const;
  void registerGameboard(const QString &menuItem, const char *actionId);
  void registerLanguage(const QString &menuItem, const char *actionId, bool enabled);
  void changeGameboard(uint newGameboard);
  void changeLanguage(uint newLanguage);
  bool loadLayout(QDomDocument &layoutDocument);
  void playSound(const QString &ref) const;

  inline bool isSoundEnabled() const {return soundEnabled;}
  inline uint getSelectedGameboard() const {return selectedGameboard;}

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
  void gameboard0();
  void gameboard1();
  void gameboard2();
  void gameboard3();
  void gameboard4();
  void gameboard5();
  void gameboard6();
  void gameboard7();
  void soundOff();
  void language0();
  void language1();
  void language2();
  void language3();
  void language4();
  void language5();
  void language6();
  void language7();
  void language8();
  void language9();
  void language10();
  void language11();
  void language12();
  void language13();
  void language14();
  void language15();

private:

  int                           // Menu items identificators
      newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      gameboardID, speechID;
  enum {                        // Tool bar buttons identificators
      ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT,
      ID_UNDO, ID_REDO,
      ID_HELP };

  bool soundEnabled;            // True if the sound is enabled by user, even if there is no audio server
  uint selectedGameboard,	// Number of currently selected gameboard
       gameboards;		// Total number of gameboards
  uint selectedLanguage,	// Number of selected language
       languages;		// Total number of languages
  QString gameboardActions[8],	// Name of actions for registered gameboards
          languageActions[16];	// Name of actions for registered languages

  PlayGround *playGround;	// Play ground central widget
  SoundFactory *soundFactory;	// Speech organ
};

#endif
