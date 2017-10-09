/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "filefactory.h"
#include "soundfactory.h"
#include "playground.h"

static const char version[] = "1.0.0";

class KTuberlingMobile : public PlayGroundCallbacks, public SoundFactoryCallbacks
{
public:
  KTuberlingMobile()
   : m_soundEnabled(true)
  {
    m_soundFactory = new SoundFactory(this);
    m_soundFactory->registerLanguages();
    m_soundFactory->loadLanguage(FileFactory::locate("sounds/en.soundtheme"));

    QWidget *mainWidget = new QWidget();
    QHBoxLayout *lay = new QHBoxLayout(mainWidget);

    m_themesWidget = new QWidget();
    m_gameboardLayout = new QGridLayout(m_themesWidget);

    m_playground = new PlayGround(this, mainWidget);
    m_playground->registerPlayGrounds();
    m_playground->lockAspectRatio(true);
    m_playground->setAllowOnlyDrag(true);
    m_playground->loadPlayGround(FileFactory::locate("pics/default_theme.theme"));

    QVBoxLayout *sideLayout = new QVBoxLayout();

    // Not sure this is the best way but it works for now
    const int screenWidth = QDesktopWidget().screenGeometry().width();
    const int iconWidth = screenWidth / 15;

    QPushButton *themesButton = new QPushButton(mainWidget);
    themesButton->setIcon(QPixmap(":/games-config-theme.png"));
    themesButton->setIconSize(QSize(iconWidth, iconWidth));
    themesButton->setFocusPolicy(Qt::NoFocus);
    QObject::connect(themesButton, &QPushButton::clicked, [this, mainWidget] {
      m_themesWidget->showFullScreen();
    });

    QPushButton *soundsButton = new QPushButton(mainWidget);
    soundsButton->setIcon(QPixmap(":/audio-volume-high.png"));
    soundsButton->setIconSize(QSize(iconWidth, iconWidth));
    soundsButton->setFocusPolicy(Qt::NoFocus);
    QObject::connect(soundsButton, &QPushButton::clicked, [this, soundsButton] {
      m_soundEnabled = !m_soundEnabled;
      soundsButton->setIcon(QPixmap(m_soundEnabled ? ":/audio-volume-high.png" : ":/audio-volume-muted.png"));
    });

    sideLayout->addWidget(themesButton);
    sideLayout->addWidget(soundsButton);
    sideLayout->addStretch(1);

    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_playground);
    lay->addLayout(sideLayout);

    mainWidget->showFullScreen();
  }

  ~KTuberlingMobile()
  {
    delete m_soundFactory;
  }

  void playSound(const QString &ref) override
  {
    m_soundFactory->playSound(ref);
  }

  void changeGameboard(const QString &/*gameboard*/) override
  {
    // Only needed when loading a file so not needed for now
  }

  void registerGameboard(const QString& menuText, const QString& boardFile, const QPixmap &/*pixmap*/) override
  {
    // TODO this should be scrollable
    // TODO use the pixmap
    QPushButton *pb = new QPushButton(menuText);
    QObject::connect(pb, &QPushButton::clicked, [this, boardFile] {
      m_playground->loadPlayGround(boardFile);
      m_themesWidget->hide();
    });

    m_gameboardLayout->addWidget(pb, m_gameboardLayout->count() / 2, m_gameboardLayout->count() % 2);
  }

  bool isSoundEnabled() const override
  {
    return m_soundEnabled;
  }

  void registerLanguage(const QString &/*code*/, const QString &/*soundFile*/, bool /*enabled*/)
  {
    // TODO
  }

private:
  SoundFactory *m_soundFactory;
  PlayGround *m_playground;
  QWidget *m_themesWidget;
  QGridLayout *m_gameboardLayout;
  bool m_soundEnabled;
};

// Main function
Q_DECL_EXPORT int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QLocale::system().name(); // needed to workaround QTBUG-41385
  app.setApplicationName("ktuberling");

  KTuberlingMobile tuberling;

  return app.exec();
}
