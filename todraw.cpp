/* -------------------------------------------------------------
   KDE Tuberling
   Object to draw on the game board
   mailto:ebisch@cybercable.tm.fr
 ------------------------------------------------------------- */

#include <qbitmap.h>
#include <qpainter.h>

#include "todraw.h"

// Constructor with no arguments
ToDraw::ToDraw()
  : position()
{
  number = -1;
}

// Copy constructor
ToDraw::ToDraw(const ToDraw &model)
  : position(model.position)
{
  number = model.number;
}

// Constructor with arguments
ToDraw::ToDraw(int declaredNumber, const QRect &declaredPosition)
  : position(declaredPosition)
{
  number = declaredNumber;
}

// Draw an object previously laid down on the game board
void ToDraw::draw(QPainter &artist, const QRect &area,
                  const QRect *objectsLayout, const QRect *shapesLayout,
                  const QBitmap *gameboard, const QBitmap *masks) const
{
  if (!position.intersects(area)) return;

  QBitmap objectBitmap(objectsLayout[number].size()),
          shapeBitmap(shapesLayout[number].size());

  bitBlt(&objectBitmap, QPoint(0, 0), gameboard, objectsLayout[number], CopyROP);
  bitBlt(&shapeBitmap, QPoint(0, 0), masks, shapesLayout[number], CopyROP);
  objectBitmap.setMask(shapeBitmap);
  artist.drawPixmap(position.topLeft(), objectBitmap);
}

// Load an object from a file
bool ToDraw::load(FILE *fp, int decorations, bool &eof)
{
  int nitems;
  int pl, pt, pr, pb;

  nitems = fscanf(fp, "%d\t%d %d %d %d\n", &number, &pl, &pt, &pr, &pb);

  if (nitems == EOF)
  {
    eof = true;
    return true;
  }
  eof = false;
  if (nitems != 5) return false;

  if (number < 0 || number >= decorations) return false;

  position.setCoords(pl, pt, pr, pb);

  return true;
}

// Save an object to a file
bool ToDraw::save(FILE *fp) const
{
  return fprintf(fp, "%d\t%d %d %d %d\n",
                 number,
                 position.left(), position.top(), position.right(), position.bottom());
}

