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

#include <kapplication.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>
#include <kaction.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kstandardgameaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>
#include <kimageio.h>
#include <kmimetype.h>
#include <kconfiggroup.h>
#include <ktemporaryfile.h>
#include <kdeprintdialog.h>
#include <kcombobox.h>

#include <QClipboard>
#include <QPrintDialog>
#include <QPrinter>

#include "playground.h"
#include "soundfactory.h"
#include "playgrounddelegate.h"

static const char *DEFAULT_THEME = "default_theme.theme";

// Constructor
TopLevel::TopLevel()
  : KXmlGuiWindow(0)
{
  QString board, language;

  playGround = new PlayGround(this);
  playGround->setObjectName( QLatin1String( "playGround" ) );

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
  unplugActionList( QLatin1String( "playgroundList" ) );
  plugActionList( QLatin1String( "playgroundList" ), actionList );

  playgroundCombo->addItem(menuText,QVariant(pixmap));
  playgroundCombo->setItemData(playgroundCombo->count()-1,QVariant(board),BOARD_THEME);
}

// Register an available language
void TopLevel::registerLanguage(const QString &code, const QString &soundFile, bool enabled)
{
  KToggleAction *t = new KToggleAction(KGlobal::locale()->languageCodeToName(code), this);
  t->setEnabled(enabled);
  actionCollection()->addAction(soundFile, t);
  t->setData(soundFile);
  sounds.insert(code, soundFile);
  connect(t, SIGNAL(toggled(bool)), SLOT(changeLanguage()));
  languagesGroup->addAction(t);
  QList<QAction*> actionList = languagesGroup->actions();
  actionList.removeAll(actionCollection()->action(QLatin1String( "speech_no_sound" )));
  qSort(actionList.begin(), actionList.end(), actionSorterByName);
  unplugActionList( QLatin1String( "languagesList" ) );
  plugActionList( QLatin1String( "languagesList" ), actionList );
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
    QStringList list = KGlobal::dirs()->findAllResources("appdata", QLatin1String( "pics/" ) + newGameBoard);
    if (!list.isEmpty()) fileToLoad = list.first();
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
    const QStringList list = KGlobal::dirs()->findAllResources("appdata", QLatin1String( "sounds/" ) + soundFile);
    if (!list.isEmpty()) fileToLoad = list.first();
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
void TopLevel::playSound(const QString &ref) const
{
  soundFactory->playSound(ref);
}

// Read options from preferences file
void TopLevel::readOptions(QString &board, QString &language)
{
  KConfigGroup config(KGlobal::config(), "General");

  QString option = config.readEntry("Sound",  "on" );
  bool soundEnabled = option.indexOf(QLatin1String( "on" )) == 0;
  board = config.readEntry("Gameboard", DEFAULT_THEME);
  language = config.readEntry("Language", "" );
  bool keepAspectRatio = config.readEntry("KeepAspectRatio", false);

  if (soundEnabled)
  {
    if (language.isEmpty())
    {
      language = sounds.value(KGlobal::locale()->language(), QLatin1String( "en.soundtheme" ));
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
  KConfigGroup config(KGlobal::config(), "General");
  config.writeEntry("Sound", actionCollection()->action(QLatin1String( "speech_no_sound" ))->isChecked() ? "off": "on");

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
  KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());

  action = actionCollection()->addAction( QLatin1String( "game_save_picture" ));
  action->setText(i18n("Save &as Picture..."));
  connect(action, SIGNAL(triggered(bool)), SLOT(filePicture()));

  //Edit
  action = KStandardAction::copy(this, SLOT(editCopy()), actionCollection());
  actionCollection()->addAction(action->objectName(), action);

  action = KStandardAction::undo(0, 0, actionCollection());
  playGround->connectUndoAction(action);
  action = KStandardAction::redo(0, 0, actionCollection());
  playGround->connectRedoAction(action);

  //Speech
  KToggleAction *t = new KToggleAction(i18n("&No Sound"), this);
  actionCollection()->addAction( QLatin1String( "speech_no_sound" ), t);
  connect(t, SIGNAL(triggered(bool)), SLOT(soundOff()));
  languagesGroup->addAction(t);

  KStandardAction::fullScreen(this, SLOT(toggleFullScreen()), this, actionCollection());

  t = new KToggleAction(i18n("&Lock Aspect Ratio"), this);
  actionCollection()->addAction( QLatin1String( "lock_aspect_ratio" ), t);
  connect(t, SIGNAL(triggered(bool)), this, SLOT(lockAspectRatio(bool)));

  playgroundCombo = new KComboBox(this);
  playgroundCombo->setMinimumWidth(200);
  playgroundCombo->view()->setMinimumHeight(100);
  playgroundCombo->view()->setMinimumWidth(200);
  playgroundCombo->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  PlaygroundDelegate *playgroundDelegate = new PlaygroundDelegate(playgroundCombo->view());
  playgroundCombo->setItemDelegate(playgroundDelegate);

  connect(playgroundCombo, SIGNAL(currentIndexChanged(int)),this,SLOT(changeGameboardFromCombo(int)));

  KAction *widgetAction = new KAction(this);
  widgetAction->setDefaultWidget(playgroundCombo);
  actionCollection()->addAction( QLatin1String( "playgroundSelection" ),widgetAction);

  setupGUI(ToolBar | Keys | Save | Create);
}

void TopLevel::saveNewToolbarConfig()
{
  // this destroys our actions lists ...
  KXmlGuiWindow::saveNewToolbarConfig();
  // ... so plug them again
  plugActionList( QLatin1String( "playgroundList" ), playgroundsGroup->actions() );
  plugActionList( QLatin1String( "languagesList" ), languagesGroup->actions() );
}

// Reset gameboard
void TopLevel::fileNew()
{
  playGround->reset();
}

// Load gameboard
void TopLevel::fileOpen()
{
  KUrl url = KFileDialog::getOpenUrl(KUrl("kfiledialog:///<ktuberling>"),
                                     QString::fromLatin1("*.tuberling|%1\n*|%2").arg(i18n("KTuberling files"), i18n("All files")));

  open(url);
}

void TopLevel::open(const KUrl &url)
{
  if (url.isEmpty())
    return;

  QString name;

  KIO::NetAccess::download(url, name, this);

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


  KIO::NetAccess::removeTempFile( name );
}

// Save gameboard
void TopLevel::fileSave()
{
  KUrl url = KFileDialog::getSaveUrl
                ( KUrl("kfiledialog:///<ktuberling>"),
                  QString::fromLatin1("*.tuberling|%1").arg(i18n("KTuberling files")), this, QString(), KFileDialog::ConfirmOverwrite);

  if (url.isEmpty())
    return;

  KTemporaryFile tempFile; // for network saving
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
    if (!KIO::NetAccess::upload(name, url, this))
      KMessageBox::error(this, i18n("Could not save file."));
  }
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  const QString patternsString = KImageIO::pattern(KImageIO::Writing);
  QStringList patterns = patternsString.split("\n");
  // Favor png
  if (!patterns.isEmpty()) {
      QString firstLine = patterns[0];
      patterns.removeAt(0);
      if (firstLine.contains(" *.png")) {
          firstLine.remove(" *.png");
          firstLine.prepend("*.png ");
      }
      patterns.prepend(firstLine);
  }
  const KUrl url = KFileDialog::getSaveUrl(KUrl(QString("kfiledialog:///<ktuberling>")), patterns.join("\n"), this, QString(), KFileDialog::ConfirmOverwrite);

  if( url.isEmpty() )
    return;

  KTemporaryFile tempFile; // for network saving
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

  KMimeType::Ptr mime = KMimeType::findByUrl(url, 0, true, true);
  if (!KImageIO::isSupported(mime->name(), KImageIO::Writing))
  {
    KMessageBox::error(this, i18n("Unknown picture format."));
    return;
  };

  QStringList types = KImageIO::typeForMime(mime->name());
  if (types.isEmpty()) return; // TODO error dialog?

  QPixmap picture(playGround->getPicture());

  if (!picture.save(name, types.at(0).toLatin1()))
  {
    KMessageBox::error
      (this, i18n("Could not save file."));
    return;
  }

  if( !url.isLocalFile() )
  {
    if (! KIO::NetAccess::upload(name, url, this))
      KMessageBox::error(this, i18n("Could not save file."));
  }

}

// Save gameboard as picture
void TopLevel::filePrint()
{
  QPrinter printer;
  bool ok;

  QPrintDialog *printDialog = KdePrint::createPrintDialog(&printer, this);
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
  actionCollection()->action(QLatin1String( "speech_no_sound" ))->setChecked(true);
  writeOptions();
}

bool TopLevel::isSoundEnabled() const
{
  return !actionCollection()->action(QLatin1String( "speech_no_sound" ))->isChecked();
}

void TopLevel::toggleFullScreen()
{
  KToggleFullScreenAction::setFullScreen( this, actionCollection()->action(QLatin1String( "fullscreen" ))->isChecked());
}

void TopLevel::lockAspectRatio(bool lock)
{
  actionCollection()->action(QLatin1String( "lock_aspect_ratio" ))->setChecked(lock);
  playGround->lockAspectRatio(lock);
  writeOptions();
}
