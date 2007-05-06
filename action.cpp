/* -------------------------------------------------------------
   KDE Tuberling
   Action stored in the undo buffer
   mailto:aacid@kde.org
 ------------------------------------------------------------- */

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



ActionMove::ActionMove(ToDraw *item, const QPointF &pos, QGraphicsScene *scene)
 : m_item(item), m_pos(pos), m_scene(scene)
{
}

void ActionMove::redo()
{
	QPointF pos = m_item->pos();
	m_item->setPos(m_pos);
	m_pos = pos;
}

void ActionMove::undo()
{
	redo();
}
