/* -------------------------------------------------------------
   KDE Tuberling
   Play ground widget
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */

#include "soundfactory.h"

#include <stdlib.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <phonon/audioplayer.h>

#include <QDomDocument>

#include "soundfactory.moc"
#include "toplevel.h"

// Constructor
SoundFactory::SoundFactory(TopLevel *parent)
	: QObject(parent)
{
  topLevel = parent;
  player = new Phonon::AudioPlayer(Phonon::GameCategory, this);

  namesList = filesList = 0;
}

// Destructor
SoundFactory::~SoundFactory()
{
  if (namesList) delete [] namesList;
  if (filesList) delete [] filesList;
}

// Change the language
void SoundFactory::change(const QString &selectedLanguage)
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

  soundFile = KStandardDirs::locate("data", "ktuberling/sounds/" + filesList[sound]);
  if (soundFile.isEmpty()) return;

//printf("%s\n", (const char *) soundFile);
  player->play(KUrl::fromPath(soundFile));
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

  for (int i = 0; i < languagesList.count(); i++)
  {
    languageElement = (const QDomElement &) languagesList.item(i).toElement();
    codeAttribute = languageElement.attributeNode("code");
    enabled = !(KStandardDirs::locate("data", "ktuberling/sounds/" + codeAttribute.value() + '/').isEmpty());

    menuItemsList = languageElement.elementsByTagName("menuitem");
    if (menuItemsList.count() != 1)
      return false;

    menuItemElement = (const QDomElement &) menuItemsList.item(0).toElement();

    labelsList = menuItemElement.elementsByTagName("label");
    if (labelsList.count() != 1)
      return false;

    labelElement = (const QDomElement &) labelsList.item(0).toElement();
    actionAttribute = menuItemElement.attributeNode("action");
    topLevel->registerLanguage(labelElement.text(), codeAttribute.value(), enabled);
  }

  return true;
}

// Load the sounds of one given language
bool SoundFactory::loadLanguage(QDomDocument &layoutDocument, const QString &selectedLanguage)
{
  QDomNodeList languagesList,
               soundNamesList;
  QDomElement languageElement,
              soundNameElement;
  QDomAttr nameAttribute, fileAttribute;

  languagesList = layoutDocument.elementsByTagName("language");

  bool found = false;
  for(int i = 0; !found && i < languagesList.count(); ++i)
  {
    languageElement = languagesList.item(i).toElement();
    if (languageElement.attribute("code") == selectedLanguage) found = true;
  }
  if (!found) return false;

  soundNamesList = languageElement.elementsByTagName("sound");
  sounds = soundNamesList.count();
  if (sounds < 1)
    return false;

  if (!(namesList = new QString[sounds]))
    return false;
  if (!(filesList = new QString[sounds]))
    return false;

  for (int sound = 0; sound < sounds; sound++)
  {
    soundNameElement = (const QDomElement &) soundNamesList.item(sound).toElement();

    nameAttribute = soundNameElement.attributeNode("name");
    namesList[sound] = nameAttribute.value();
    fileAttribute = soundNameElement.attributeNode("file");
    filesList[sound] = fileAttribute.value();
  }

  currentLang = selectedLanguage;

  return true;
}

QString SoundFactory::currentLanguage() const
{
  return currentLang;
}
