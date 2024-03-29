/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Action stored in the undo buffer */

#include "action.h"

#include <QGraphicsScene>

#include "todraw.h"

ActionAdd::ActionAdd(ToDraw *item, QGraphicsScene *scene)
 : m_item(item), m_scene(scene), m_done(false), m_shouldAdd(false)
{
	// First m_shouldAdd is false since it was already added
	// in the playground code
}

ActionAdd::~ActionAdd()
{
	if (!m_done) delete m_item;
}

void ActionAdd::redo()
{
	if (m_shouldAdd) {
		m_scene->addItem(m_item);
	}
	m_done = true;
	m_shouldAdd = true;
}

void ActionAdd::undo()
{
	m_scene->removeItem(m_item);
	m_done = false;
}



ActionRemove::ActionRemove(ToDraw *item, const QPointF &oldPos, QGraphicsScene *scene)
 : m_item(item), m_scene(scene), m_done(true)
{
	m_oldPos = QPointF(oldPos.x() / scene->width(), oldPos.y() / scene->height());
}

ActionRemove::~ActionRemove()
{
	if (m_done) delete m_item;
}

void ActionRemove::redo()
{
	m_scene->removeItem(m_item);
	m_done = true;
}

void ActionRemove::undo()
{
	m_item->setPos(m_oldPos.x() * m_scene->width(), m_oldPos.y() * m_scene->height());
	m_scene->addItem(m_item);
	m_done = false;
}



ActionMove::ActionMove(ToDraw *item, const QPointF &oldPos, int zValue, QGraphicsScene *scene)
 : m_item(item), m_zValue(zValue), m_scene(scene)
{
	m_oldPos = QPointF(oldPos.x() / scene->width(), oldPos.y() / scene->height());
	m_newPos = QPointF(m_item->pos().x() / scene->width(), m_item->pos().y() / scene->height());
}

void ActionMove::redo()
{
	qreal zValue = m_item->zValue();
	m_item->setPos(m_newPos.x() * m_scene->width(), m_newPos.y() * m_scene->height());
	m_item->setZValue(m_zValue);
	m_zValue = zValue;
}

void ActionMove::undo()
{
	qreal zValue = m_item->zValue();
	m_item->setPos(m_oldPos.x() * m_scene->width(), m_oldPos.y() * m_scene->height());
	m_item->setZValue(m_zValue);
	m_zValue = zValue;
}
