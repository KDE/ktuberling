/*************************************************************************************
 *  Copyright (C) 2010-2012 by Alejandro Fiestas Olivares <afiestas@kde.org>         *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

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
