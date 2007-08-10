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

#ifndef _SOUNDFACTORY_H_
#define _SOUNDFACTORY_H_

#include <QStringList>

class TopLevel;

namespace Phonon
{
      class MediaObject;
}

class SoundFactory
{
public:

  SoundFactory(TopLevel *parent);
  ~SoundFactory();

  bool loadLanguage(const QString &selectedLanguageFile);
  void playSound(const QString &soundRef) const;

  QString currentSoundFile() const;

  void registerLanguages();

private:
  QString currentSndFile;		// The current language

  int sounds;				// Number of sounds
  QStringList namesList,		// List of sound names
              filesList;           // List of sound files associated with each sound name

  TopLevel *topLevel;		// Top-level window
  Phonon::MediaObject *player;  // Sound player
};

#endif
