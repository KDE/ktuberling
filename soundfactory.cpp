/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include <stdlib.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kaudioplayer.h>

#include <qdom.h>

#include "soundfactory.h"
#include "soundfactory.moc"
#include "toplevel.h"

// Constructor
SoundFactory::SoundFactory(TopLevel *parent, const char *name, uint selectedLanguage)
	: QObject(parent, name)
{
  topLevel = parent;

  namesList = filesList = 0;

  QDomDocument layoutsDocument;
  bool ok = topLevel->loadLayout(layoutsDocument);
  if (ok) ok = registerLanguages(layoutsDocument);
  if (ok) ok = loadLanguage(layoutsDocument, selectedLanguage);
  if (!ok) loadFailure();
}

// Destructor
SoundFactory::~SoundFactory()
{
  if (namesList) delete [] namesList;
  if (filesList) delete [] filesList;
}

// Change the language
void SoundFactory::change(uint selectedLanguage)
{
  QDomDocument layoutsDocument;
  bool ok = topLevel->loadLayout(layoutsDocument);
  if (ok) ok = loadLanguage(layoutsDocument, selectedLanguage);
  if (!ok) loadFailure();
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

  soundFile = locate("data", "ktuberling/sounds/" + filesList[sound]);
  if (soundFile == 0) return;

//printf("%s\n", (const char *) soundFile);
  KAudioPlayer::play(soundFile);
}

// Report a load failure
void SoundFactory::loadFailure()
{
	KMessageBox::error(topLevel, i18n("Error while loading the sound names."));
}

// Register the various languages
bool SoundFactory::registerLanguages(QDomDocument &layoutDocument)
{
  QDomNodeList languagesList, menuItemsList, labelsList;
  QDomElement languageElement, menuItemElement, labelElement;
  QDomAttr codeAttribute, actionAttribute;
  bool enabled;

  languagesList = layoutDocument.elementsByTagName("language");
  if (languagesList.count() < 1)
    return false;

  for (uint i = 0; i < languagesList.count(); i++)
  {
    languageElement = (const QDomElement &) languagesList.item(i).toElement();
    codeAttribute = languageElement.attributeNode("code");
    enabled = locate("data", "ktuberling/sounds/" + codeAttribute.value() + "/") != 0;

    menuItemsList = languageElement.elementsByTagName("menuitem");
    if (menuItemsList.count() != 1)
      return false;

    menuItemElement = (const QDomElement &) menuItemsList.item(0).toElement();

    labelsList = menuItemElement.elementsByTagName("label");
    if (labelsList.count() != 1)
      return false;

    labelElement = (const QDomElement &) labelsList.item(0).toElement();
    actionAttribute = menuItemElement.attributeNode("action");
    topLevel->registerLanguage(labelElement.text(), actionAttribute.value().latin1(), enabled); 
  }
 
  return true;
}

// Load the sounds of one given language
bool SoundFactory::loadLanguage(QDomDocument &layoutDocument, uint toLoad)
{
  QDomNodeList languagesList,
               soundNamesList;
  QDomElement languageElement,
              soundNameElement;
  QDomAttr nameAttribute, fileAttribute;

  languagesList = layoutDocument.elementsByTagName("language");
  if (toLoad >= languagesList.count())
    return false;

  languageElement = (const QDomElement &) languagesList.item(toLoad).toElement();

  soundNamesList = languageElement.elementsByTagName("sound");
  sounds = soundNamesList.count();
  if (sounds < 1)
    return false;

  if (!(namesList = new QString[sounds]))
    return false;
  if (!(filesList = new QString[sounds]))
    return false;

  for (uint sound = 0; sound < sounds; sound++)
  {
    soundNameElement = (const QDomElement &) soundNamesList.item(sound).toElement();

    nameAttribute = soundNameElement.attributeNode("name");
    namesList[sound] = nameAttribute.value();
    fileAttribute = soundNameElement.attributeNode("file");
    filesList[sound] = fileAttribute.value();
  }
  return true;
}

