/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/* Top level window */

#include "toplevel.h"

#include <kmessagebox.h>
#include <KLocalizedString>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kstandardgameaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>
#include <kconfiggroup.h>
#include <kcombobox.h>
#include <ksharedconfig.h>
#include <kio/job.h>

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QPrintDialog>
#include <QPrinter>
#include <QTemporaryFile>
#include <QWidgetAction>

#include "filefactory.h"
#include "playground.h"
#include "soundfactory.h"
#include "playgrounddelegate.h"

// TODO kdelibs4support REMOVE
#include <KLocale>

static const char *DEFAULT_THEME = "default_theme.theme";

// Constructor
TopLevel::TopLevel()
  : KXmlGuiWindow(0)
{
  QString board, language;

  playGround = new PlayGround(this, this);
  playGround->setObjectName( QStringLiteral( "playGround" ) );

  soundFactory = new SoundFactory(this);

  setCentralWidget(playGround);

  playgroundsGroup = new QActionGroup(this);
  playgroundsGroup->setExclusive(true);

  languagesGroup = new QActionGroup(this);
  languagesGroup->setExclusive(true);

  setupKAction();

  playGround->registerPlayGrounds();
  soundFactory->registerLanguages();

  readOptions(board, language);
  changeGameboard(board);
  changeLanguage(language);
}

// Destructor
TopLevel::~TopLevel()
{
  delete soundFactory;
}

static bool actionSorterByName(QAction *a, QAction *b)
{
  return a->text().localeAwareCompare(b->text()) < 0;
}

// Register an available gameboard
void TopLevel::registerGameboard(const QString &menuText, const QString &board, const QPixmap &pixmap)
{
  KToggleAction *t = new KToggleAction(menuText, this);
  actionCollection()->addAction(board, t);
  t->setData(board);
  connect(t, SIGNAL(toggled(bool)), SLOT(changeGameboard()));
  playgroundsGroup->addAction(t);
  QList<QAction*> actionList = playgroundsGroup->actions();
  qSort(actionList.begin(), actionList.end(), actionSorterByName);
  unplugActionList( QStringLiteral( "playgroundList" ) );
  plugActionList( QStringLiteral( "playgroundList" ), actionList );

  playgroundCombo->addItem(menuText,QVariant(pixmap));
  playgroundCombo->setItemData(playgroundCombo->count()-1,QVariant(board),BOARD_THEME);
}

// Register an available language
void TopLevel::registerLanguage(const QString &code, const QString &soundFile, bool enabled)
{
  KToggleAction *t = new KToggleAction(KLocale::global()->languageCodeToName(code), this);
  t->setEnabled(enabled);
  actionCollection()->addAction(soundFile, t);
  t->setData(soundFile);
  sounds.insert(code, soundFile);
  connect(t, SIGNAL(toggled(bool)), SLOT(changeLanguage()));
  languagesGroup->addAction(t);
  QList<QAction*> actionList = languagesGroup->actions();
  actionList.removeAll(actionCollection()->action(QStringLiteral( "speech_no_sound" )));
  qSort(actionList.begin(), actionList.end(), actionSorterByName);
  unplugActionList( QStringLiteral( "languagesList" ) );
  plugActionList( QStringLiteral( "languagesList" ), actionList );
}

// Switch to another gameboard
void TopLevel::changeGameboardFromCombo(int index)
{
  QString newBoard = playgroundCombo->itemData(index,BOARD_THEME).toString();
  changeGameboard(newBoard);
}

void TopLevel::changeGameboard()
{
  QAction *action = qobject_cast<QAction*>(sender());
  // ignore toggling of "nonchecked" actions
  if (action->isChecked())
  {
    QString newGameBoard = action->data().toString();
    changeGameboard(newGameBoard);
  }
}

void TopLevel::changeGameboard(const QString &newGameBoard)
{
  if (newGameBoard == playGround->currentGameboard()) return;

  QString fileToLoad;
  QFileInfo fi(newGameBoard);
  if (fi.isRelative())
  {
    fileToLoad = FileFactory::locate(QLatin1String( "pics/" ) + newGameBoard);
  }
  else
  {
    fileToLoad = newGameBoard;
  }

  int index = playgroundCombo->findData(fileToLoad, BOARD_THEME);
  playgroundCombo->setCurrentIndex(index);
  QAction *action = actionCollection()->action(fileToLoad);
  if (action && playGround->loadPlayGround(fileToLoad))
  {
    action->setChecked(true);

    // Change gameboard in the remembered options
    writeOptions();
  }
  else
  {
    // Something bad just happened, try the default playground
    if (newGameBoard != QLatin1String(DEFAULT_THEME))
    {
      changeGameboard(QLatin1String(DEFAULT_THEME));
    }
    else
    {
      KMessageBox::error(this, i18n("Error while loading the playground."));
    }
  }
}

void TopLevel::changeLanguage()
{
  QAction *action = qobject_cast<QAction*>(sender());
  QString soundFile = action->data().toString();
  changeLanguage(soundFile);
}

// Switch to another language
void TopLevel::changeLanguage(const QString &soundFile)
{
  if (soundFile == soundFactory->currentSoundFile()) return;

  QString fileToLoad;
  QFileInfo fi(soundFile);
  if (fi.isRelative())
  {
    fileToLoad = FileFactory::locate(QLatin1String( "sounds/" ) + soundFile);
  }
  else
  {
    fileToLoad = soundFile;
  }

  // Change language effectively
  QAction *action = actionCollection()->action(fileToLoad);
  if (action && soundFactory->loadLanguage(fileToLoad))
  {
    action->setChecked(true);

    // Change language in the remembered options
    writeOptions();
  }
  else
  {
    KMessageBox::error(this, i18n("Error while loading the sound file."));
    soundOff();
  }
}

// Play a sound
void TopLevel::playSound(const QString &ref)
{
  soundFactory->playSound(ref);
}

// Read options from preferences file
void TopLevel::readOptions(QString &board, QString &language)
{
  KConfigGroup config(KSharedConfig::openConfig(), "General");

  QString option = config.readEntry("Sound",  "on" );
  bool soundEnabled = option.indexOf(QLatin1String( "on" )) == 0;
  board = config.readEntry("Gameboard", DEFAULT_THEME);
  language = config.readEntry("Language", "" );
  bool keepAspectRatio = config.readEntry("KeepAspectRatio", false);

  if (soundEnabled)
  {
    if (language.isEmpty())
    {
      language = sounds.value(KLocale::global()->language(), QStringLiteral( "en.soundtheme" ));
    }
  }
  else
  {
    soundOff();
    language = QString();
  }

  lockAspectRatio(keepAspectRatio);
}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfigGroup config(KSharedConfig::openConfig(), "General");
  config.writeEntry("Sound", actionCollection()->action(QStringLiteral( "speech_no_sound" ))->isChecked() ? "off": "on");

  config.writeEntry("Gameboard", playGround->currentGameboard());

  config.writeEntry("Language", soundFactory->currentSoundFile());

  config.writeEntry("KeepAspectRatio", playGround->isAspectRatioLocked());
}

// KAction initialization (aka menubar + toolbar init)
void TopLevel::setupKAction()
{
  QAction *action;

  //Game
  KStandardGameAction::gameNew(this, SLOT(fileNew()), actionCollection());
  KStandardGameAction::load(this, SLOT(fileOpen()), actionCollection());
  KStandardGameAction::save(this, SLOT(fileSave()), actionCollection());
  KStandardGameAction::print(this, SLOT(filePrint()), actionCollection());
  KStandardGameAction::quit(qApp, SLOT(quit()), actionCollection());

  action = actionCollection()->addAction( QStringLiteral( "game_save_picture" ));
  action->setText(i18n("Save &as Picture..."));
  connect(action, &QAction::triggered, this, &TopLevel::filePicture);

  //Edit
  action = KStandardAction::copy(this, SLOT(editCopy()), actionCollection());
  actionCollection()->addAction(action->objectName(), action);

  action = KStandardAction::undo(0, 0, actionCollection());
  playGround->connectUndoAction(action);
  action = KStandardAction::redo(0, 0, actionCollection());
  playGround->connectRedoAction(action);

  //Speech
  KToggleAction *t = new KToggleAction(i18n("&No Sound"), this);
  actionCollection()->addAction( QStringLiteral( "speech_no_sound" ), t);
  connect(t, &QAction::triggered, this, &TopLevel::soundOff);
  languagesGroup->addAction(t);

  KStandardAction::fullScreen(this, SLOT(toggleFullScreen()), this, actionCollection());

  t = new KToggleAction(i18n("&Lock Aspect Ratio"), this);
  actionCollection()->addAction( QStringLiteral( "lock_aspect_ratio" ), t);
  connect(t, &QAction::triggered, this, &TopLevel::lockAspectRatio);

  playgroundCombo = new KComboBox(this);
  playgroundCombo->setMinimumWidth(200);
  playgroundCombo->view()->setMinimumHeight(100);
  playgroundCombo->view()->setMinimumWidth(200);
  playgroundCombo->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  PlaygroundDelegate *playgroundDelegate = new PlaygroundDelegate(playgroundCombo->view());
  playgroundCombo->setItemDelegate(playgroundDelegate);

  connect(playgroundCombo, SIGNAL(currentIndexChanged(int)),this,SLOT(changeGameboardFromCombo(int)));

  QWidgetAction *widgetAction = new QWidgetAction(this);
  widgetAction->setDefaultWidget(playgroundCombo);
  actionCollection()->addAction( QStringLiteral( "playgroundSelection" ),widgetAction);

  setupGUI(ToolBar | Keys | Save | Create);
}

void TopLevel::saveNewToolbarConfig()
{
  // this destroys our actions lists ...
  KXmlGuiWindow::saveNewToolbarConfig();
  // ... so plug them again
  plugActionList( QStringLiteral( "playgroundList" ), playgroundsGroup->actions() );
  plugActionList( QStringLiteral( "languagesList" ), languagesGroup->actions() );
}

// Reset gameboard
void TopLevel::fileNew()
{
  playGround->reset();
}

// Load gameboard
void TopLevel::fileOpen()
{
  QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Load file"), QUrl(),
                                     i18n("KTuberling files (%1)", QStringLiteral("*.tuberling")));

  open(url);
}

void TopLevel::open(const QUrl &url)
{
  if (url.isEmpty())
    return;

  QString name;
  if (url.isLocalFile()) {
    // file protocol. We do not need the network
    name = url.toLocalFile();
  } else {
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(false);
    tmpFile.open();
    name = tmpFile.fileName();
    const QUrl dest = QUrl::fromLocalFile(name);
    KIO::Job *job = KIO::file_copy(url, dest, -1, KIO::Overwrite | KIO::HideProgressInfo);
    QEventLoop eventLoop;
    connect(job, &KIO::Job::result, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
  }

  switch(playGround->loadFrom(name))
  {
    case PlayGround::NoError:
     // good
    break;

    case PlayGround::OldFileVersionError:
      KMessageBox::error(this, i18n("The saved file is from an old version of KTuberling and unfortunately cannot be opened with this version."));
    break;

    case PlayGround::OtherError:
      KMessageBox::error(this, i18n("Could not load file."));
    break;
  }

  if (!url.isLocalFile()) {
    QFile::remove(name);
  }
}

// Save gameboard
void TopLevel::fileSave()
{
  QUrl url = QFileDialog::getSaveFileUrl( this, QString(), QUrl(), i18n("KTuberling files (%1)", QStringLiteral("*.tuberling")) );

  if (url.isEmpty())
    return;

  QTemporaryFile tempFile; // for network saving
  QString name;
  if( !url.isLocalFile() )
  {
    if (tempFile.open()) name = tempFile.fileName();
    else
    {
      KMessageBox::error(this, i18n("Could not save file."));
      return;
    }
  }
  else
  {
    name = url.path();
  }

  if( !playGround->saveAs( name ) )
  {
    KMessageBox::error(this, i18n("Could not save file."));
    return;
  }

  if( !url.isLocalFile() )
  {
    if (!upload(name, url))
      KMessageBox::error(this, i18n("Could not save file."));
  }
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  const QMimeDatabase mimedb;
  const QList<QByteArray> imageWriterMimetypes = QImageWriter::supportedMimeTypes();
  QStringList patterns;
  for(const auto &mimeName : imageWriterMimetypes)
  {
    const QMimeType mime = mimedb.mimeTypeForName(mimeName);
    if (mime.isValid())
    {
      QStringList suffixes;
      for(const QString &suffix : mime.suffixes())
      {
          suffixes << QStringLiteral("*.%1").arg(suffix);
      }

      // Favor png
      const QString pattern = i18nc("%1 is mimetype and %2 is the file extensions", "%1 (%2)", mime.comment(), suffixes.join(' '));
      if (mimeName == "image/png")
      {
        patterns.prepend(pattern);
      }
      else
      {
        patterns << pattern;
      }
    }
  }
  const QUrl url = QFileDialog::getSaveFileUrl( this, QString(), QUrl(), patterns.join(QStringLiteral(";;")) );

  if( url.isEmpty() )
    return;

  QTemporaryFile tempFile; // for network saving
  QString name;
  if( !url.isLocalFile() )
  {
    tempFile.open();
    name = tempFile.fileName();
  }
  else
  {
    name = url.path();
  }

  QPixmap picture(playGround->getPicture());

  if (!picture.save(name))
  {
    KMessageBox::error
      (this, i18n("Could not save file."));
    return;
  }

  if( !url.isLocalFile() )
  {
    if (!upload(name, url))
      KMessageBox::error(this, i18n("Could not save file."));
  }

}

// Save gameboard as picture
void TopLevel::filePrint()
{
  QPrinter printer;
  bool ok;

  QPrintDialog *printDialog = new QPrintDialog(&printer, this);
  printDialog->setWindowTitle(i18n("Print %1", actionCollection()->action(playGround->currentGameboard())->iconText()));
  ok = printDialog->exec();
  delete printDialog;
  if (!ok) return;
  playGround->repaint();
  if (!playGround->printPicture(printer))
    KMessageBox::error(this,
                         i18n("Could not print picture."));
  else
    KMessageBox::information(this,
                             i18n("Picture successfully printed."));
}

// Copy modified area to clipboard
void TopLevel::editCopy()
{
  QClipboard *clipboard = QApplication::clipboard();
  QPixmap picture(playGround->getPicture());

  clipboard->setPixmap(picture);
}

// Toggle sound off
void TopLevel::soundOff()
{
  actionCollection()->action(QStringLiteral( "speech_no_sound" ))->setChecked(true);
  writeOptions();
}

bool TopLevel::isSoundEnabled() const
{
  return !actionCollection()->action(QStringLiteral( "speech_no_sound" ))->isChecked();
}

void TopLevel::toggleFullScreen()
{
  KToggleFullScreenAction::setFullScreen( this, actionCollection()->action(QStringLiteral( "fullscreen" ))->isChecked());
}

void TopLevel::lockAspectRatio(bool lock)
{
  actionCollection()->action(QStringLiteral( "lock_aspect_ratio" ))->setChecked(lock);
  playGround->lockAspectRatio(lock);
  writeOptions();
}

bool TopLevel::upload(const QString &src, const QUrl &target)
{
  bool success = true;
  const QUrl srcUrl = QUrl::fromLocalFile(src);
  KIO::Job *job = KIO::file_copy(srcUrl, target, -1, KIO::Overwrite | KIO::HideProgressInfo);
  QEventLoop eventLoop;
  connect(job, &KIO::Job::result, this, [job, &eventLoop, &success] { success = !job->error(); eventLoop.quit(); } );
  eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
  return success;
}
