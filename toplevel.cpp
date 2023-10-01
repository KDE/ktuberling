/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Top level window */

#include "toplevel.h"

#include <KMessageBox>
#include <KLocalizedString>
#include <KLanguageName>
#include <KStandardAction>
#include <KStandardShortcut>
#include <KStandardGameAction>
#include <KActionCollection>
#include <KToggleAction>
#include <KToggleFullScreenAction>
#include <KConfigGroup>
#include <KSharedConfig>
#include <kio/job.h>

#include <QActionGroup>
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
#include "playgrounddelegate.h"


static const char *DEFAULT_THEME = "default_theme.theme";

// Constructor
TopLevel::TopLevel()
  : KXmlGuiWindow(nullptr)
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
  connect(t, &KToggleAction::toggled, this, [this, t, board] {
    if (t->isChecked())
    {
      changeGameboard(board);
    }
  });
  playgroundsGroup->addAction(t);
  QList<QAction*> actionList = playgroundsGroup->actions();
  std::sort(actionList.begin(), actionList.end(), actionSorterByName);
  unplugActionList( QStringLiteral( "playgroundList" ) );
  plugActionList( QStringLiteral( "playgroundList" ), actionList );

  playgroundCombo->addItem(menuText,QVariant(pixmap));
  playgroundCombo->setItemData(playgroundCombo->count()-1,QVariant(board),BOARD_THEME);
}

// Register an available language
void TopLevel::registerLanguage(const QString &code, const QString &soundFile, bool enabled)
{
  KToggleAction *t = new KToggleAction(KLanguageName::nameForCode(code), this);
  t->setEnabled(enabled);
  actionCollection()->addAction(soundFile, t);
  sounds.insert(code, soundFile);
  connect(t, &KToggleAction::toggled, this, [this, soundFile] {
    changeLanguage(soundFile);
  });
  languagesGroup->addAction(t);
  QList<QAction*> actionList = languagesGroup->actions();
  actionList.removeAll(actionCollection()->action(QStringLiteral( "speech_no_sound" )));
  std::sort(actionList.begin(), actionList.end(), actionSorterByName);
  unplugActionList( QStringLiteral( "languagesList" ) );
  plugActionList( QStringLiteral( "languagesList" ), actionList );
}

// Switch to another gameboard
void TopLevel::changeGameboardFromCombo(int index)
{
  QString newBoard = playgroundCombo->itemData(index,BOARD_THEME).toString();
  changeGameboard(newBoard);
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

  if (soundEnabled && language.isEmpty())
  {
    const QStringList &systemLanguages = KLocalizedString::languages();
    for (const QString &systemLanguage : systemLanguages)
    {
      QString sound = sounds.value(systemLanguage);
      if (!sound.isEmpty())
      {
        language = sound;
        break;
      }
    }
    if (language.isEmpty())
    {
      language = QStringLiteral("en.soundtheme");
    }
  }

  if (!soundEnabled)
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
  KStandardGameAction::gameNew(this, &TopLevel::fileNew, actionCollection());
  KStandardGameAction::load(this, &TopLevel::fileOpen, actionCollection());
  KStandardGameAction::save(this, &TopLevel::fileSave, actionCollection());
  KStandardGameAction::print(this, &TopLevel::filePrint, actionCollection());
  KStandardGameAction::quit(qApp, &QApplication::quit, actionCollection());

  action = actionCollection()->addAction( QStringLiteral( "game_save_picture" ));
  action->setText(i18n("Save &as Picture..."));
  connect(action, &QAction::triggered, this, &TopLevel::filePicture);

  //Edit
  action = KStandardAction::copy(this, &TopLevel::editCopy, actionCollection());
  actionCollection()->addAction(action->objectName(), action);

  action = KStandardAction::undo(nullptr, nullptr, actionCollection());
  playGround->connectUndoAction(action);
  action = KStandardAction::redo(nullptr, nullptr, actionCollection());
  playGround->connectRedoAction(action);

  //Speech
  KToggleAction *t = new KToggleAction(i18n("&No Sound"), this);
  actionCollection()->addAction( QStringLiteral( "speech_no_sound" ), t);
  connect(t, &QAction::triggered, this, &TopLevel::soundOff);
  languagesGroup->addAction(t);

  KStandardAction::fullScreen(this, &TopLevel::toggleFullScreen, this, actionCollection());

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
  connect(playgroundCombo, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &TopLevel::changeGameboardFromCombo);
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
  QUrl url = QFileDialog::getOpenFileUrl(this, i18nc("@title:window", "Load file"), QUrl(),
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

static QStringList extractSuffixesFromQtPattern(const QString &qtPattern)
{
  static const QRegularExpression regexp(".*\\((.*)\\)");
  const QRegularExpressionMatch match = regexp.match(qtPattern);
  if (match.hasMatch()) {
    QStringList suffixes = match.captured(1).split(" ");
    if (!suffixes.isEmpty()) {
      for (QString &suffix : suffixes) {
        suffix = suffix.mid(1); // Remove the * from the start, we want the actual suffix
      }
      return suffixes;
    }
    qWarning() << "extractSuffixesFromQtPattern suffix split failed" << qtPattern;
  } else {
    qWarning() << "extractSuffixesFromQtPattern regexp match failed" << qtPattern;
  }
  return { ".report_bug_please" };
}

static QUrl getSaveFileUrl(QWidget *w, const QString &patterns)
{
  QString selectedPattern;
  QUrl url = QFileDialog::getSaveFileUrl( w, QString(), QUrl(), patterns, &selectedPattern );

  if( url.isEmpty() )
    return {};

  // make sure the url ends in one of the extensions of selectedPattern
  const QStringList selectedSuffixes = extractSuffixesFromQtPattern(selectedPattern);
  bool validSuffix = false;
  for (const QString &suffix : selectedSuffixes) {
    if (url.path().endsWith(suffix)) {
      validSuffix = true;
      break;
    }
  }
  // and if it does not add it
  if (!validSuffix) {
    url.setPath(url.path() + selectedSuffixes[0]);
  }

  return url;
}

// Save gameboard
void TopLevel::fileSave()
{
  const QUrl url = getSaveFileUrl( this, i18n("KTuberling files (%1)", QStringLiteral("*.tuberling")) );

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
    name = url.toLocalFile();
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
  const QUrl url = getSaveFileUrl( this, patterns.join(QStringLiteral(";;")) );

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
    name = url.toLocalFile();
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
  printDialog->setWindowTitle(i18nc("@title:window", "Print %1", actionCollection()->action(playGround->currentGameboard())->iconText()));
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

#include "moc_toplevel.cpp"
