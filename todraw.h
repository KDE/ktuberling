/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Object to draw on the game board */

#ifndef _TODRAW_H_
#define _TODRAW_H_

#include <QGraphicsSvgItem>

class ToDraw : public QGraphicsSvgItem
{
  public:
    ToDraw();
    
    void save(QDataStream &stream) const;
    bool load(QDataStream &stream);

    bool contains(const QPointF &point) const override;

    enum { Type = UserType + 1 };
    int type() const override;

    QRectF boundingRect() const override;
    QRectF unclippedRect() const;

    void setBeingDragged(bool dragged);

  protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
  
  private:
    QRectF clippedRectAt(const QPointF &somePos) const;

    bool m_beingDragged;
};

#endif
