/* -------------------------------------------------------------
   KDE Tuberling
   Object to draw on the game board
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */


#ifndef _TODRAW_H_
#define _TODRAW_H_

#include <QGraphicsSvgItem>

class KSvgRenderer;

QImage toImage(const QString &element, int width, int height, QSvgRenderer *renderer);

class ToDraw : public QGraphicsSvgItem
{
  public:
    void save(QDataStream &stream) const;
    bool load(QDataStream &stream);

    bool contains(const QPointF &point) const;
};

#endif
