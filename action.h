/* -------------------------------------------------------------
   KDE Tuberling
   Action stored in the undo buffer
   mailto:aacid@kde.org
 ------------------------------------------------------------- */

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
		ActionMove(ToDraw *item, const QPointF &pos, QGraphicsScene *scene);
		
		void redo();
		void undo();
	
	private:
		ToDraw *m_item;
		QPointF m_pos;
		QGraphicsScene *m_scene;
};

#endif
