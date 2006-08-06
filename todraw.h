/* -------------------------------------------------------------
   KDE Tuberling
   Object to draw on the game board
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */


#ifndef _TODRAW_H_
#define _TODRAW_H_

#include <QRect>

class QTextStream;
class QPixmap;
class QPainter;
class QBitmap;

class ToDraw
{
  public:
    ToDraw();
    ToDraw(const ToDraw &);
    ToDraw(int, const QRect &);
    ToDraw &operator=(const ToDraw &);
    void draw(QPainter &, const QRect &, const QRect *, const QPixmap *, const QBitmap *) const;
    void save(QTextStream &) const;
    bool load(QTextStream &, int);

    inline int getNumber() const { return number; }
    inline void setNumber(int newValue) { number = newValue; }
    inline const QRect &getPosition() const { return position; }

  private:
    int number;
    QRect position;
};

#endif
