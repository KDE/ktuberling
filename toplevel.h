/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
  ~TopLevel() override;

  void open(const QUrl &url);
  void registerGameboard(const QString& menuText, const QString& boardFile, const QPixmap& pixmap) override;
  void registerLanguage(const QString &code, const QString &soundFile, bool enabled) override;
  void changeLanguage(const QString &langCode);
  void playSound(const QString &ref) override;

  bool isSoundEnabled() const override;

  void changeGameboard(const QString &gameboard) final;

protected:
  void readOptions(QString &board, QString &language);
  void writeOptions();
  void setupKAction();

protected slots:
  void saveNewToolbarConfig() override;

private slots:

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
