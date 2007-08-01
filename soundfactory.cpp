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

#include <phonon/audioplayer.h>

#include <QDomDocument>
#include <QFile>

#include "toplevel.h"

// Constructor
SoundFactory::SoundFactory(TopLevel *parent)
{
  topLevel = parent;
  player = new Phonon::AudioPlayer(Phonon::GameCategory, parent);
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

  soundFile = KStandardDirs::locate("data", "ktuberling/sounds/" + filesList[sound]);
  if (soundFile.isEmpty()) return;

//printf("%s\n", (const char *) soundFile);
  player->play(KUrl::fromPath(soundFile));
}

// Register the various languages
void SoundFactory::registerLanguages()
{
  QStringList list = KGlobal::dirs()->findAllResources("appdata", "sounds/*.soundtheme");

  foreach(const QString &soundTheme, list)
  {
    QFile file(soundTheme);
    if (file.open(QIODevice::ReadOnly))
    {
      QDomDocument document;
      if (document.setContent(&file))
      {
        QString code = document.documentElement().attribute("code");
        bool enabled = !(KStandardDirs::locate("appdata", "sounds/" + code + '/').isEmpty());
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

  soundNamesList = languageElement.elementsByTagName("sound");
  sounds = soundNamesList.count();
  kDebug() << "C";
  if (sounds < 1)
    return false;


  for (int sound = 0; sound < sounds; sound++)
  {
    soundNameElement = (const QDomElement &) soundNamesList.item(sound).toElement();

    nameAttribute = soundNameElement.attributeNode("name");
    namesList << nameAttribute.value();
    fileAttribute = soundNameElement.attributeNode("file");
    filesList << fileAttribute.value();
  }

  currentSndFile = selectedLanguageFile;

  return true;
}

QString SoundFactory::currentSoundFile() const
{
  return currentSndFile;
}
