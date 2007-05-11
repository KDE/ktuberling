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

class QDomDocument;
class TopLevel;

namespace Phonon
{
      class AudioPlayer;
}

class SoundFactory
{
public:

  SoundFactory(TopLevel *parent);
  ~SoundFactory();

  void change(const QString &selectedLanguage);
  void playSound(const QString &soundRef) const;

  QString currentLanguage() const;

  bool registerLanguages(QDomDocument &layoutDocument);

protected:
  bool loadLanguage(QDomDocument &layoutDocument, const QString &selectedLanguage);

private:

  void loadFailure();

private:
  QString currentLang;		// The current language

  int sounds;				// Number of sounds
  QStringList namesList,		// List of sound names
              filesList;           // List of sound files associated with each sound name

  TopLevel *topLevel;		// Top-level window
  Phonon::AudioPlayer *player;  // Sound player
};

#endif
