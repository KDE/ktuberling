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
#include <kprinter.h>
#include <kimageio.h>
#include <kmimetype.h>
#include <kconfiggroup.h>

#include <QClipboard>
#include <QDomDocument>

#include "toplevel.moc"
#include "playground.h"
#include "soundfactory.h"

// Constructor
TopLevel::TopLevel()
  : KXmlGuiWindow(0)
{
  QString board, language;
  readOptions(board, language);

  playGround = new PlayGround(this);
  playGround->setObjectName( "playGround" );

  soundFactory = new SoundFactory(this);

  setCentralWidget(playGround);

  playgroundsGroup = new QActionGroup(this);
  playgroundsGroup->setExclusive(true);

  languagesGroup = new QActionGroup(this);
  languagesGroup->setExclusive(true);

  setupKAction();

  QDomDocument layoutsDocument;
  bool ok = loadLayout(layoutsDocument);
  ok &= playGround->registerPlayGrounds(layoutsDocument);
  ok &= soundFactory->registerLanguages(layoutsDocument);
  if (!ok) loadFailure();

  changeGameboard(board);
  changeLanguage(language);
}

// Destructor
TopLevel::~TopLevel()
{
}

// Report a load failure
void TopLevel::loadFailure()
{
  KMessageBox::error(this, i18n("Fatal error:\n"
				    "Unable to load the pictures, aborting."));
  exit(-1);
}


// Register an available gameboard
void TopLevel::registerGameboard(const QString &menuText, const QString &board)
{
  QList<QAction*> actionList;
  KToggleAction *t = new KToggleAction(i18n(menuText.toLatin1()), this);
  actionCollection()->addAction(board, t);
  t->setData(board);
  connect(t, SIGNAL(toggled(bool)), SLOT(changeGameboard()));
  actionList << t;
  playgroundsGroup->addAction(t);
  plugActionList( "playgroundList", actionList );
}

// Register an available language
void TopLevel::registerLanguage(const QString &code, bool enabled)
{
  QList<QAction*> actionList;
  KToggleAction *t = new KToggleAction(KGlobal::locale()->twoAlphaToLanguageName(code), this);
  t->setEnabled(enabled);
  actionCollection()->addAction(code, t);
  t->setData(code);
  connect(t, SIGNAL(toggled(bool)), SLOT(changeLanguage()));
  actionList << t;
  languagesGroup->addAction(t);
  plugActionList( "languagesList", actionList );
}

// Switch to another gameboard
void TopLevel::changeGameboard()
{
  QAction *action = qobject_cast<QAction*>(sender());
  QString newGameBoard = action->data().toString();
  changeGameboard(newGameBoard);
}

void TopLevel::changeGameboard(const QString &newGameBoard)
{
  if (newGameBoard == playGround->currentGameboard()) return;

  QDomDocument layoutsDocument;
  bool ok = loadLayout(layoutsDocument);
  ok &= playGround->loadPlayGround(layoutsDocument, newGameBoard);
  if (!ok) loadFailure();

  actionCollection()->action(newGameBoard)->setChecked(true);

  // Change gameboard in the remembered options
  writeOptions();
}

void TopLevel::changeLanguage()
{
  QAction *action = qobject_cast<QAction*>(sender());
  QString langCode = action->data().toString();
  changeLanguage(langCode);
}

// Switch to another language
void TopLevel::changeLanguage(const QString &langCode)
{
  if (langCode == soundFactory->currentLanguage()) return;

  // Change language effectively
  soundFactory->change(langCode);

  actionCollection()->action(langCode)->setChecked(true);

  // Change language in the remembered options
  soundEnabled = true;
  writeOptions();
}

// Load the layouts file
bool TopLevel::loadLayout(QDomDocument &layoutDocument)
{
  QFile layoutFile(QFile::encodeName(KStandardDirs::locate("data", "ktuberling/pics/layout.xml")));
  if (!layoutFile.open(QIODevice::ReadOnly))
     return false;

  if (!layoutDocument.setContent(&layoutFile))
  {
     layoutFile.close();
     return false;
  }
  layoutFile.close();

  return true;
}

// Play a sound
void TopLevel::playSound(const QString &ref) const
{
  soundFactory->playSound(ref);
}

// Read options from preferences file
void TopLevel::readOptions(QString &board, QString &language)
{
  QString option;
  KConfigGroup config(KGlobal::config(), "General");

  option = config.readEntry("Sound", "on");
  soundEnabled = option.indexOf("on") == 0;

  board = config.readEntry("Gameboard", "default_theme.svg");
  language = config.readEntry("Language", "en");
}

// Write options to preferences file
void TopLevel::writeOptions()
{
  KConfigGroup config(KGlobal::config(), "General");
  config.writeEntry("Sound", soundEnabled? "on": "off");

  config.writeEntry("Gameboard", playGround->currentGameboard());

  config.writeEntry("Language", soundFactory->currentLanguage());
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

  action = actionCollection()->addAction("game_save_picture");
  action->setText(i18n("Save &as Picture..."));
  connect(action, SIGNAL(triggered(bool) ), SLOT(filePicture()));

  //Edit
  action = KStandardAction::copy(this, SLOT(editCopy()), actionCollection());
  actionCollection()->addAction(action->objectName(), action);

  QAction *redoAction = playGround->getRedoAction();
  redoAction->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::Redo).primary());
  redoAction->setIcon( KIcon("edit-redo") );
  actionCollection()->addAction("edit_redo", redoAction);

  QAction *undoAction = playGround->getUndoAction();
  undoAction->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::Undo).primary());
  undoAction->setIcon( KIcon("edit-undo") );
  actionCollection()->addAction("edit_undo", undoAction);

  //Speech
  KToggleAction *t = new KToggleAction(i18n("&No Sound"), this);
  actionCollection()->addAction("speech_no_sound", t);
  connect(t, SIGNAL(triggered(bool) ), SLOT(soundOff()));
  languagesGroup->addAction(t);
  if (!soundEnabled) t->setChecked(true);

  setupGUI(ToolBar | Keys | Save | Create);
}

// Reset gameboard
void TopLevel::fileNew()
{
  playGround->reset();
}

// Load gameboard
void TopLevel::fileOpen()
{
  QString dir = KStandardDirs::locate("data", "ktuberling/museum/miss.tuberling");
  dir.truncate(dir.lastIndexOf('/') + 1);

  KUrl url = KFileDialog::getOpenUrl(dir, "*.tuberling");

  open(url);
}

void TopLevel::open(const KUrl &url)
{
  if (url.isEmpty())
    return;

  QString name;

  KIO::NetAccess::download(url, name, this);

  playGround->reset();

  switch(playGround->loadFrom(name))
  {
    case PlayGround::NoError:
     // good
    break;

    case PlayGround::OldFileVersionError:
      KMessageBox::error(this, i18n("The saved file is from an old version of KTuberling and unfortunately can not be open with this version."));
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
                ( KUrl(),
		 "*.tuberling");

  if (url.isEmpty())
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files is currently "
                            "supported."));
    return;
  }

  QString name = url.path();
  int suffix;

  suffix = name.lastIndexOf('.');
  if (suffix == -1)
  {
    name += ".tuberling";
  }

  if( !playGround->saveAs( name ) )
    KMessageBox::error(this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePicture()
{
  KUrl url = KFileDialog::getSaveUrl(KUrl(), KImageIO::pattern(KImageIO::Writing));

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry(this,
                       i18n("Only saving to local files is currently "
                            "supported."));
    return;
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

  QString name = url.path();

  if (!picture.save(name, types.at(0).toLatin1()))
    KMessageBox::error
      (this, i18n("Could not save file."));
}

// Save gameboard as picture
void TopLevel::filePrint()
{
  KPrinter printer;
  bool ok;

  ok = printer.setup(this, i18n("Print %1", actionCollection()->action(playGround->currentGameboard())->iconText()));
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
  if (!soundEnabled) return;

  soundEnabled = false;
  writeOptions();
}
