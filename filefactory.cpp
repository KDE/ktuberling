/***************************************************************************
 *   Copyright (C) 2017 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "filefactory.h"

#include <QFileInfo>
#include <QStandardPaths>

bool FileFactory::folderExists(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    QFileInfo fi("/data/data/org.kde.ktuberling/qt-reserved-files/share/ktuberling/" + relativePath);
    return fi.isDir();
#else
    return !(QStandardPaths::locate(QStandardPaths::AppDataLocation, relativePath, QStandardPaths::LocateDirectory).isEmpty());
#endif
}

QString FileFactory::locate(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    return "/data/data/org.kde.ktuberling/qt-reserved-files/share/ktuberling/" + relativePath;
#else
    return QStandardPaths::locate(QStandardPaths::AppDataLocation, relativePath);
#endif
}

QStringList FileFactory::locateAll(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    return { "/data/data/org.kde.ktuberling/qt-reserved-files/share/ktuberling/" + relativePath };
#else
    return QStandardPaths::locateAll(QStandardPaths::AppDataLocation, relativePath, QStandardPaths::LocateDirectory);
#endif
}

