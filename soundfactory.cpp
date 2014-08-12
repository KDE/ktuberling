/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/* Sound factory */

#include "soundfactory.h"

#include <stdlib.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <phonon/MediaObject>

#include <QDomDocument>
#include <QFile>

#include "toplevel.h"

// Constructor
SoundFactory::SoundFactory(TopLevel *parent)
{
  topLevel = parent;
  player = Phonon::createPlayer(Phonon::GameCategory);
  player->setParent(parent);
}

// Destructor
SoundFactory::~SoundFactory()
{
}

// Play some sound
void SoundFactory::playSound(const QString &soundRef) const
{
  int sound;
  QString soundFile;

  if (!topLevel->isSoundEnabled()) return;

  for (sound = 0; sound < sounds; sound++)
	  if (!namesList[sound].compare(soundRef)) break;
  if (sound == sounds) return;

  soundFile = KStandardDirs::locate("appdata", QLatin1String( "sounds/" ) + filesList[sound]);
  if (soundFile.isEmpty()) return;

//printf("%s\n", (const char *) soundFile);
  player->setCurrentSource(QUrl::fromLocalFile(soundFile));
  player->play();
}

// Register the various languages
void SoundFactory::registerLanguages()
{
  const QStringList list = KGlobal::dirs()->findAllResources("appdata", QLatin1String( "sounds/*.soundtheme" ));

  foreach(const QString &soundTheme, list)
  {
    QFile file(soundTheme);
    if (file.open(QIODevice::ReadOnly))
    {
      QDomDocument document;
      if (document.setContent(&file))
      {
        QString code = document.documentElement().attribute(QLatin1String( "code" ));
        bool enabled = !(KStandardDirs::locate("appdata", QLatin1String( "sounds/" ) + code + QLatin1Char( '/' )).isEmpty());
        topLevel->registerLanguage(code, soundTheme, enabled);
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

  soundNamesList = languageElement.elementsByTagName(QLatin1String( "sound" ));
  sounds = soundNamesList.count();
  if (sounds < 1)
    return false;


  namesList.clear();
  filesList.clear();
  for (int sound = 0; sound < sounds; sound++)
  {
    soundNameElement = (const QDomElement &) soundNamesList.item(sound).toElement();

    nameAttribute = soundNameElement.attributeNode(QLatin1String( "name" ));
    namesList << nameAttribute.value();
    fileAttribute = soundNameElement.attributeNode(QLatin1String( "file" ));
    filesList << fileAttribute.value();
  }

  currentSndFile = selectedLanguageFile;

  return true;
}

QString SoundFactory::currentSoundFile() const
{
  return currentSndFile;
}
