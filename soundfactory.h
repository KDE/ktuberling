/* -------------------------------------------------------------
   KDE Tuberling
   Sound factory
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */


#ifndef _SOUNDFACTORY_H_
#define _SOUNDFACTORY_H_

#include <QObject>

class QDomDocument;
class TopLevel;

namespace Phonon
{
      class AudioPlayer;
}

class SoundFactory : public QObject
{
  Q_OBJECT

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
  QString *namesList,		// List of sound names
  	  *filesList;           // List of sound files associated with each sound name

  TopLevel *topLevel;		// Top-level window
  Phonon::AudioPlayer *player;  // Sound player
};

#endif
