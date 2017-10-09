/***************************************************************************
 *   Copyright (C) 2017 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FILEFACTORY_H
#define FILEFACTORY_H

class QString;
class QStringList;

namespace FileFactory
{
    bool folderExists(const QString &relativePath);
    QString locate(const QString &relativePath);
    QStringList locateAll(const QString &relativePath);
};

#endif
