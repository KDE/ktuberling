/*
    SPDX-FileCopyrightText: 2017 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FILEFACTORY_H
#define FILEFACTORY_H

class QString;

#include <QStringList>

namespace FileFactory
{
    bool folderExists(const QString &relativePath);
    QString locate(const QString &relativePath);
    QStringList locateAll(const QString &relativePath);
};

#endif
