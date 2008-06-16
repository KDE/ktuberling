/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/* Object to draw on the game board */

#ifndef _TODRAW_H_
#define _TODRAW_H_

#include <QGraphicsSvgItem>


QPixmap toPixmap(const QString &element, int width, int height, QSvgRenderer *renderer);

class ToDraw : public QGraphicsSvgItem
{
  public:
    ToDraw();
    
    void save(QDataStream &stream) const;
    bool load(QDataStream &stream);

    bool contains(const QPointF &point) const;

    enum { Type = UserType + 1 };
    int type() const;

    QRectF boundingRect() const;
    QRectF unclippedRect() const;

  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
  private:
    QRectF clippedRectAt(const QPointF &somePos) const;
};

#endif
