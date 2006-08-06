/* -------------------------------------------------------------
   KDE Tuberling
   Object to draw on the game board
   mailto:ebischoff@nerim.net
 ------------------------------------------------------------- */

#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QTextStream>

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

// Affectation operator
ToDraw &ToDraw::operator=(const ToDraw &model)
{
  if (&model == this) return *this;

  position = model.position;
  number = model.number;

  return *this;
}

// Draw an object previously laid down on the game board
void ToDraw::draw(QPainter &artist, const QRect &area,
                  const QRect *objectsLayout,
                  const QPixmap *gameboard, const QBitmap *masks) const
{
  if (!position.intersects(area)) return;

  QPixmap objectPixmap(objectsLayout[number].size());
  QBitmap shapeBitmap(objectsLayout[number].size());
  shapeBitmap.clear();

  QPainter paint(&objectPixmap);
  paint.drawPixmap(QPoint(0, 0), *gameboard, objectsLayout[number]);
  paint.end();
  paint.begin(&shapeBitmap);
  paint.drawPixmap(QPoint(0, 0), *masks, objectsLayout[number]);
  paint.end();
  objectPixmap.setMask(shapeBitmap);
  artist.drawPixmap(position.topLeft(), objectPixmap);
}

// Load an object from a file
bool ToDraw::load(QTextStream &stream, int decorations)
{
  int pl, pt, pr, pb;

  stream >> number >> pl >> pt >> pr >> pb;
  // NOTE: read error checking?

  if (number < 0 || number >= decorations) return false;

  position.setCoords(pl, pt, pr, pb);

  return true;
}

// Save an object to a file
void ToDraw::save(QTextStream &stream) const
{
  stream << number << "\t" << position.left() <<" ";
  stream << position.top() << " " << position.right() << " ";
  stream << position.bottom() << "\n";
}

