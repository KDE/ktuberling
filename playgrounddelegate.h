/*
    SPDX-FileCopyrightText: 2010-2012 Alejandro Fiestas Olivares <afiestas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYGROUNDDELEGATE_H
#define PLAYGROUNDDELEGATE_H

#include <QAbstractItemDelegate>
#include <QAbstractItemView>

class PlaygroundDelegate : public QAbstractItemDelegate
{
  public:
    explicit PlaygroundDelegate(QObject *parent = nullptr);
  private:
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // PLAYGROUNDDELEGATE_H
