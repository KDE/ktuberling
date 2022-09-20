/*
    SPDX-FileCopyrightText: 1999-2006 Éric Bischoff <ebischoff@nerim.net>
    SPDX-FileCopyrightText: 2007 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* Action stored in the undo buffer */

#ifndef _ACTION_H_
#define _ACTION_H_

#include <QUndoCommand>
#include <QPointF>

class ToDraw;

class QGraphicsScene;

class ActionAdd : public QUndoCommand
{
	public:
		ActionAdd(ToDraw *item, QGraphicsScene *scene);
		~ActionAdd() override;
		
		void redo() override;
		void undo() override;
	
	private:
		ToDraw *m_item;
		QGraphicsScene *m_scene;
		bool m_done;
		bool m_shouldAdd;
};


class ActionRemove : public QUndoCommand
{
	public:
		ActionRemove(ToDraw *item, const QPointF &oldPos, QGraphicsScene *scene);
		~ActionRemove() override;
		
		void redo() override;
		void undo() override;
	
	private:
		ToDraw *m_item;
		QPointF m_oldPos;
		QGraphicsScene *m_scene;
		bool m_done;
};

class ActionMove : public QUndoCommand
{
	public:
		ActionMove(ToDraw *item, const QPointF &oldPos, int zValue, QGraphicsScene *scene);
		
		void redo() override;
		void undo() override;
	
	private:
		ToDraw *m_item;
		QPointF m_oldPos;
		QPointF m_newPos;
		qreal m_zValue;
		QGraphicsScene *m_scene;
};

#endif
