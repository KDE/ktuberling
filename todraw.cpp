/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Object to draw on the game board */

#include "todraw.h"

#include <QDataStream>
#include <QPainter>
#include <QSvgRenderer>

static QImage toImage(const QString &element, int width, int height, QSvgRenderer *renderer)
{
  QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
  img.fill(Qt::transparent);
  QPainter p2(&img);
  // don't need quality here
  p2.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform, false);
  renderer->render(&p2, element);
  p2.end();
  return img;
}


ToDraw::ToDraw()
 : m_beingDragged(false)
{
}

// Load an object from a file
bool ToDraw::load(QDataStream &stream)
{
  // NOTE: read error checking?
  QPointF pos;
  QString element;
  qreal zOrder;

  stream >> pos;
  stream >> element;
  stream >> zOrder;

  setPos(pos);
  setElementId(element);
  setZValue(zOrder);

  return true;
}

// Save an object to a file
void ToDraw::save(QDataStream &stream) const
{
  stream << pos();
  stream << elementId();
  stream << zValue();
}

QRectF ToDraw::unclippedRect() const
{
  return QGraphicsSvgItem::boundingRect();
}

QRectF ToDraw::clippedRectAt(const QPointF &somePos) const
{
  if (m_beingDragged)
    return unclippedRect();

  QRectF backgroundRect = renderer()->boundsOnElement(QStringLiteral( "background" ));
  backgroundRect.translate(-somePos);
  backgroundRect = transform().inverted().map(backgroundRect).boundingRect();

  return unclippedRect().intersected(backgroundRect);
}

void ToDraw::setBeingDragged(bool dragged)
{
    prepareGeometryChange();
    m_beingDragged = dragged;
}

QRectF ToDraw::boundingRect() const
{
  return clippedRectAt(pos());
}

QVariant ToDraw::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange) {
    if (boundingRect() != clippedRectAt(value.toPointF()))
      prepareGeometryChange();
  }
  return QGraphicsSvgItem::itemChange(change, value);
}

bool ToDraw::contains(const QPointF &point) const
{
	bool result = QGraphicsSvgItem::contains(point);
	if (result)
	{
		QRectF bounds = transform().mapRect(unclippedRect());
		const QImage &img = toImage(elementId(), qRound(bounds.width()), qRound(bounds.height()), renderer());
		QPointF transformedPoint = transform().map(point);
		result = qAlpha(img.pixel(transformedPoint.toPoint())) != 0;
	}
	return result;
}

int ToDraw::type() const
{
  return Type;
}
