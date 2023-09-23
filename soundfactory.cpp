/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Sound factory */

#include "soundfactory.h"

#include <stdlib.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMediaPlayer>
#include <QSet>
#include <QUrl>

#include "filefactory.h"

// Constructor
SoundFactory::SoundFactory(SoundFactoryCallbacks *callbacks)
 : m_callbacks(callbacks)
{
  player = new QMediaPlayer();
}

// Destructor
SoundFactory::~SoundFactory()
{
  delete player;
}

// Play some sound
void SoundFactory::playSound(const QString &soundRef) const
{
  if (!m_callbacks->isSoundEnabled()) return;

  int sound;
  for (sound = 0; sound < sounds; sound++)
	  if (!namesList[sound].compare(soundRef)) break;
  if (sound == sounds) return;

  const QString soundFile = FileFactory::locate(QLatin1String( "sounds/" ) + filesList[sound]);
  if (soundFile.isEmpty()) return;

  if (soundFile.startsWith(':')) {
    player->setSource(QUrl("qrc" + soundFile));
  } else {
    player->setSource(QUrl::fromLocalFile(soundFile));
  }
  player->play();
}

// Register the various languages
void SoundFactory::registerLanguages()
{
  QSet<QString> list;
  const QStringList dirs = FileFactory::locateAll(QStringLiteral("sounds"));
  for (const QString &dir : dirs)
  {
    const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.soundtheme"));
    for (const QString &file :  fileNames)
    {
        list <<dir + '/' + file;
    }
  }

  for(const QString &soundTheme : std::as_const(list))
  {
    QFile file(soundTheme);
    if (file.open(QIODevice::ReadOnly))
    {
      QDomDocument document;
      if (document.setContent(&file))
      {
        const QString code = document.documentElement().attribute(QStringLiteral( "code" ));
        const bool enabled = FileFactory::folderExists(QLatin1String( "sounds/" ) + code + QLatin1Char( '/' ));
        m_callbacks->registerLanguage(code, soundTheme, enabled);
      }
    }
  }
}

// Load the sounds of one given language
bool SoundFactory::loadLanguage(const QString &selectedLanguageFile)
{
  QDomNodeList languagesList,
               soundNamesList;
  QDomElement languageElement,
              soundNameElement;
  QDomAttr nameAttribute, fileAttribute;

  QFile file(selectedLanguageFile);
  if (!file.open(QIODevice::ReadOnly)) return false;

  QDomDocument document;
  if (!document.setContent(&file)) return false;

  languageElement = document.documentElement();

  soundNamesList = languageElement.elementsByTagName(QStringLiteral( "sound" ));
  sounds = soundNamesList.count();
  if (sounds < 1)
    return false;


  namesList.clear();
  filesList.clear();
  for (int sound = 0; sound < sounds; sound++)
  {
    soundNameElement = (const QDomElement &) soundNamesList.item(sound).toElement();

    nameAttribute = soundNameElement.attributeNode(QStringLiteral( "name" ));
    namesList << nameAttribute.value();
    fileAttribute = soundNameElement.attributeNode(QStringLiteral( "file" ));
    filesList << fileAttribute.value();
  }

  currentSndFile = selectedLanguageFile;

  return true;
}

QString SoundFactory::currentSoundFile() const
{
  return currentSndFile;
}
