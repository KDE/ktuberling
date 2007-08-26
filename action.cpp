/***************************************************************************
 *   Copyright (C) 1999-2006 by Éric Bischoff <ebischoff@nerim.net>        *
 *   Copyright (C) 2007 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/* Action stored in the undo buffer */

#include "action.h"

#include <QGraphicsScene>

#include "todraw.h"

ActionAdd::ActionAdd(ToDraw *item, QGraphicsScene *scene)
 : m_item(item), m_scene(scene), m_done(false)
{
}

ActionAdd::~ActionAdd()
{
	if (!m_done) delete m_item;
}

void ActionAdd::redo()
{
	m_scene->addItem(m_item);
	m_done = true;
}

void ActionAdd::undo()
{
	m_scene->removeItem(m_item);
	m_done = false;
}



ActionRemove::ActionRemove(ToDraw *item, QGraphicsScene *scene)
 : m_item(item), m_scene(scene), m_done(true)
{
}

ActionRemove::~ActionRemove()
{
	if (m_done) delete m_item;
}

void ActionRemove::redo()
{
	if (!m_done)
	{
		m_scene->removeItem(m_item);
		m_done = true;
	}
}

void ActionRemove::undo()
{
	m_scene->addItem(m_item);
	m_done = false;
}



ActionMove::ActionMove(ToDraw *item, const QPointF &pos, int zValue, QGraphicsScene *scene)
 : m_item(item), m_zValue(zValue), m_scene(scene)
{
	m_pos = QPointF(pos.x() / scene->width(), pos.y() / scene->height());
}

void ActionMove::redo()
{
	QPointF pos = m_item->pos();
	qreal zValue = m_item->zValue();
	m_item->setPos(m_pos.x() * m_scene->width(), m_pos.y() * m_scene->height());
	m_item->setZValue(m_zValue);
	m_pos = QPointF(pos.x() / m_scene->width(), pos.y() / m_scene->height());
	m_zValue = zValue;
}

void ActionMove::undo()
{
	redo();
}
