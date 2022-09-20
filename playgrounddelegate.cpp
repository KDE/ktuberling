/*
    SPDX-FileCopyrightText: 2010-2012 Alejandro Fiestas Olivares <afiestas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "playgrounddelegate.h"
#include <QPainter>

PlaygroundDelegate::PlaygroundDelegate(QObject *parent): QAbstractItemDelegate(parent)
{ }

QSize PlaygroundDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_UNUSED(option);
  Q_UNUSED(index);
  return QSize(200,100);
}

void PlaygroundDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QString title = index.model()->data(index, Qt::DisplayRole).toString();
  QPixmap pixmap = index.model()->data(index,Qt::UserRole).value<QPixmap>();

  //Paint background with highlight
  painter->save();
  if (option.state & QStyle::State_Selected) {
      painter->setBrush(option.palette.color(QPalette::Highlight));
  } else {
      painter->setBrush(Qt::gray);
  }
  painter->drawRect(option.rect);
  painter->restore();

  //Any way of do this more beatifuly?
  painter->drawPixmap(option.rect.x()+4,option.rect.y()+4,option.rect.width()-8,option.rect.height()-8,pixmap);
  QFont font = painter->font();
  font.setWeight(QFont::Bold);

  painter->setFont(font);
  painter->drawText(option.rect, Qt::AlignCenter | Qt::TextWordWrap, title);
  painter->save();
  painter->restore();
}
