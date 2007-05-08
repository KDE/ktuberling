/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

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
