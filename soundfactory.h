/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Sound factory */

#ifndef _SOUNDFACTORY_H_
#define _SOUNDFACTORY_H_

#include <QStringList>

class QMediaPlayer;

class SoundFactoryCallbacks
{
public:
  virtual ~SoundFactoryCallbacks() {};
  virtual bool isSoundEnabled() const = 0;
  virtual void registerLanguage(const QString &code, const QString &soundFile, bool enabled) = 0;
};

class SoundFactory
{
public:
  explicit SoundFactory(SoundFactoryCallbacks *callbacks);
  ~SoundFactory();
  SoundFactory(const SoundFactory &) = delete;
  SoundFactory &operator=(const SoundFactory &) = delete;

  bool loadLanguage(const QString &selectedLanguageFile);
  void playSound(const QString &soundRef) const;

  QString currentSoundFile() const;

  void registerLanguages();

private:
  SoundFactoryCallbacks *m_callbacks;

  QString currentSndFile;		// The current language

  int sounds;				// Number of sounds
  QStringList namesList,		// List of sound names
              filesList;           // List of sound files associated with each sound name

  QMediaPlayer *player;
};

#endif
