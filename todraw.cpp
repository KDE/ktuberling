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

#include "todraw.h"

#include <QPainter>
#include <QDataStream>

#include <QSvgRenderer>

QImage toImage(const QString &element, int width, int height, QSvgRenderer *renderer)
{
  QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
  QPainter p2(&img);
  p2.setCompositionMode(QPainter::CompositionMode_Clear);
  p2.setBrush(Qt::SolidPattern);
  p2.drawRect(0, 0, width, height);
  p2.setCompositionMode(QPainter::CompositionMode_SourceOver);
  renderer->render(&p2, element);
  return img;
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

#include <kdebug.h>
void ToDraw::paint(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	const QRectF &bounds = transform().mapRect(boundingRect());
	const QRectF &backgroundRect = renderer()->boundsOnElement("background");
	double xMaxEdge = transform().inverted().map(pos()).x() - backgroundRect.x() + boundingRect().width();
	double yMaxEdge = transform().inverted().map(pos()).y() - backgroundRect.y() + boundingRect().height();

	QImage img(bounds.width(), bounds.height(), QImage::Format_ARGB32_Premultiplied);
	QPainter p2(&img);
	p2.setCompositionMode(QPainter::CompositionMode_Clear);
	p2.setBrush(Qt::SolidPattern);
	p2.drawRect(0, 0, bounds.width(), bounds.height());
	p2.setCompositionMode(QPainter::CompositionMode_SourceOver);
	renderer()->render(&p2, elementId());
	painter->setWorldMatrix(QMatrix());
	double widthCut = 0;
	double heightCut = 0;
	double mappedWidthCut, mappedHeightCut;
	if (xMaxEdge > backgroundRect.width()) widthCut = xMaxEdge - backgroundRect.width();
	if (yMaxEdge > backgroundRect.height()) heightCut = yMaxEdge - backgroundRect.height();
	transform().map(widthCut, heightCut, &mappedWidthCut, &mappedHeightCut);
	painter->drawImage(pos(), img, QRectF(0, 0, bounds.width() - mappedWidthCut, bounds.height() - mappedHeightCut));
}

bool ToDraw::contains(const QPointF &point) const
{
	bool result = QGraphicsSvgItem::contains(point);
	if (result)
	{
		QRectF bounds = transform().mapRect(boundingRect());
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
