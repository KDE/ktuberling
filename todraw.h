/* -------------------------------------------------------------
   KDE Tuberling
   Object to draw on the game board
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */


#ifndef _TODRAW_H_
#define _TODRAW_H_

#include <qrect.h>

#include <stdio.h>

class ToDraw
{
  public:
    ToDraw();
    ToDraw(const ToDraw &);
    ToDraw(int, const QRect &);
    void draw(QPainter &, const QRect &, const QRect *, const QRect *, const QBitmap *, const QBitmap *) const;
    bool save(FILE *) const;
    bool load(FILE *, int, bool &);

    inline int getNumber() const { return number; }
    inline const QRect &getPosition() const { return position; }

  private:
    int number;
    QRect position;
};

#endif
