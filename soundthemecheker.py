#!/usr/bin/python
# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2008-2019 Albert Astals Cid <aacid@kde.org>

# SPDX-License-Identifier: GPL-2.0-or-later

import sys
from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtXml

app = QtGui.QGuiApplication(sys.argv)

if len(sys.argv) != 5:
	print("Error: You have to specify the sound theme file to check, the folder containing the graphical themes, the folder where to look for the sound files and the path to ignore in the file tag of the sound object")
	print(" E.g: For lt it would be something like 'python soundthemecheker.py /home/kdeunstable/l10n-kde4/lt/data/kdegames/ktuberling/lt.soundtheme /home/kdeunstable/kdegames/ktuberling/pics /home/kdeunstable/l10n-kde4/lt/data/kdegames/ktuberling/ lt'")
	sys.exit(1)

soundThemePath = sys.argv[1]
graphicalThemesPath = sys.argv[2]
soundsPath = sys.argv[3]
ignorePath = sys.argv[4]
print("Processing " + soundThemePath + " " + graphicalThemesPath)
soundThemeFile = QtCore.QFile(soundThemePath)

if soundThemeFile.exists():
	if (soundThemeFile.open(QtCore.QIODevice.ReadOnly)):
		doc = QtXml.QDomDocument()
		doc.setContent(soundThemeFile.readAll())
		root = doc.documentElement()
		if (root.tagName() == "language"):
			soundList = [];
			soundTag = root.firstChildElement("sound");
			while (not soundTag.isNull()):
				name = soundTag.attribute("name")
				contains = soundList.count(name)
				if (contains == 0):
					soundList.append(name)
					fpath = soundTag.attribute("file")
					if (fpath.startswith(ignorePath)):
						fpath = fpath[len(ignorePath):]
					if (not QtCore.QFile.exists(soundsPath + fpath)):
						print("Error: Sound file for " + name + " not found")
				else:
					print("Error: The name {0} is used more than once in the sound theme file".format(name))
				
				soundTag = soundTag.nextSiblingElement("sound");
			
			graphicalThemesDir = QtCore.QDir(graphicalThemesPath)
			allSoundsList = []
			for graphicalThemePath in graphicalThemesDir.entryList(["*.theme"]):
				graphicalThemeFile = QtCore.QFile(graphicalThemesPath + "/" + graphicalThemePath)
				if graphicalThemeFile.exists():
					if (graphicalThemeFile.open(QtCore.QIODevice.ReadOnly)):
						doc = QtXml.QDomDocument()
						doc.setContent(graphicalThemeFile.readAll())
						root = doc.documentElement()
						if (root.tagName() == "playground"):
							objectTag = root.firstChildElement("object");
							while (not objectTag.isNull()):
								sound = objectTag.attribute("sound")
								contains = allSoundsList.count(sound)
								if (sound == ""):
									print("The sound of " + objectTag.attribute("name") + " in " + graphicalThemeFile.fileName() + " is empty")
								if (contains == 0):
									allSoundsList.append(sound)
								
								objectTag = objectTag.nextSiblingElement("object");
						else:
							print("Error: The graphical theme file should begin with the playground tag " + graphicalThemeFile.fileName())
					else:
						print("Error: Could not open {0} for reading".format(graphicalThemePath))
				else:
					print("Error: File {0} does not exist".format(graphicalThemePath))
			for sound in soundList:
				if (allSoundsList.count(sound) == 1):
					allSoundsList.remove(sound)
				else:
					print("Error: The sound theme defines " + sound + " that is not used in any graphical theme")
			if (len(allSoundsList) > 0):
				print("The following sounds used in the graphical themes are not defined in the sound theme:")
				for sound in allSoundsList:
					print("\t" + sound)
		else:
			print("Error: The sound theme file should begin with the language tag")
		soundThemeFile.close();
	else:
		print("Error: Could not open {0} for reading".format(path))
else:
	print("Error: File {0} does not exist".format(soundThemePath))

sys.exit(0)
