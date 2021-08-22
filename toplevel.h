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
#include <KComboBox>

#include "soundfactory.h"
#include "playground.h"

class QActionGroup;
class PlayGround;

class TopLevel : public KXmlGuiWindow, public SoundFactoryCallbacks, public PlayGroundCallbacks
{
  Q_OBJECT

public:

  TopLevel();
  ~TopLevel();

  void open(const QUrl &url);
  void registerGameboard(const QString& menuText, const QString& boardFile, const QPixmap& pixmap) override;
  void registerLanguage(const QString &code, const QString &soundFile, bool enabled) override;
  void changeLanguage(const QString &langCode);
  void playSound(const QString &ref) override;

  bool isSoundEnabled() const override;

  void changeGameboard(const QString &gameboard) override final;

protected:
  void readOptions(QString &board, QString &language);
  void writeOptions();
  void setupKAction();

protected Q_SLOTS:
  void saveNewToolbarConfig() override;

private Q_SLOTS:

  void fileNew();
  void fileOpen();
  void fileSave();
  void filePicture();
  void filePrint();
  void editCopy();
  void soundOff();
  void changeGameboardFromCombo(int index);
  void toggleFullScreen();
  void lockAspectRatio(bool lock);

private:
  bool upload(const QString &src, const QUrl &target);

  int                           // Menu items identificators
      newID, openID, saveID, pictureID, printID, quitID,
      copyID, undoID, redoID,
      gameboardID, speechID;
  enum {                        // Tool bar buttons identificators
      ID_NEW, ID_OPEN, ID_SAVE, ID_PRINT,
      ID_UNDO, ID_REDO,
      ID_HELP };
  enum { BOARD_THEME = Qt::UserRole + 1};


  QActionGroup *playgroundsGroup, *languagesGroup;
  KComboBox *playgroundCombo;

  PlayGround *playGround;	// Play ground central widget
  SoundFactory *soundFactory;	// Speech organ
  QMap<QString, QString> sounds; // language code, file
};

#endif
