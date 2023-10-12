/*
    SPDX-FileCopyrightText: 2017 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filefactory.h"

#include <QStandardPaths>

bool FileFactory::folderExists(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    Q_UNUSED(relativePath);
    return true;
#else
    return !(QStandardPaths::locate(QStandardPaths::AppDataLocation, relativePath, QStandardPaths::LocateDirectory).isEmpty());
#endif
}

QString FileFactory::locate(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    return QLatin1String(":/") + relativePath;
#else
    return QStandardPaths::locate(QStandardPaths::AppDataLocation, relativePath);
#endif
}

QStringList FileFactory::locateAll(const QString &relativePath)
{
#if defined(Q_OS_ANDROID)
    return { QLatin1String(":/") + relativePath };
#else
    return QStandardPaths::locateAll(QStandardPaths::AppDataLocation, relativePath, QStandardPaths::LocateDirectory);
#endif
}

