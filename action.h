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
		~ActionAdd();
		
		void redo();
		void undo();
	
	private:
		ToDraw *m_item;
		QGraphicsScene *m_scene;
		bool m_done;
		bool m_shouldAdd;
};


class ActionRemove : public QUndoCommand
{
	public:
		ActionRemove(ToDraw *item, QGraphicsScene *scene);
		~ActionRemove();
		
		void redo();
		void undo();
	
	private:
		ToDraw *m_item;
		QGraphicsScene *m_scene;
		bool m_done;
};

class ActionMove : public QUndoCommand
{
	public:
		ActionMove(ToDraw *item, const QPointF &oldPos, int zValue, QGraphicsScene *scene);
		
		void redo();
		void undo();
	
	private:
		ToDraw *m_item;
		QPointF m_oldPos;
		QPointF m_newPos;
		qreal m_zValue;
		QGraphicsScene *m_scene;
};

#endif
