/* -------------------------------------------------------------
   KDE Tuberling
   Top level window
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */


#ifndef _TOPLEVEL_H_
#define _TOPLEVEL_H_

#include <kxmlguiwindow.h>
#include <kurl.h>

class QActionGroup;
class QDomDocument;
class PlayGround;
class SoundFactory;

class TopLevel : public KXmlGuiWindow
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

  void loadFailure();

  void open(const KUrl &url);
  void registerGameboard(const QString &menuText, const QString &board);
  void registerLanguage(const QString &menuItem, const QString &code, bool enabled);
  void changeLanguage(const QString &langCode);
  bool loadLayout(QDomDocument &layoutDocument);
  void playSound(const QString &ref) const;

  inline bool isSoundEnabled() const {return soundEnabled;}

  void changeGameboard(const QString &gameboard);

protected:
  void readOptions(QString &board, QString &language);
  void writeOptions();
  void setupKAction();

private slots:

  void fileNew();
  void fileOpen();
  void fileSave();
  void filePicture();
  void filePrint();
  void editCopy();
  void soundOff();
  void changeGameboard();
  void changeLanguage();

private:
  int                           // Menu items identificators
      newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      gameboardID, speechID;
  enum {                        // Tool bar buttons identificators
      ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT,
      ID_UNDO, ID_REDO,
      ID_HELP };


  QActionGroup *playgroundsGroup, *languagesGroup;
  bool soundEnabled;            // True if the sound is enabled by user, even if there is no audio server

  PlayGround *playGround;	// Play ground central widget
  SoundFactory *soundFactory;	// Speech organ
};

#endif
