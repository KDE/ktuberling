/* -------------------------------------------------------------
   KDE Tuberling
   Action stored in the undo buffer
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */


#ifndef _ACTION_H_
#define _ACTION_H_

#include "todraw.h"

class Action
{
  public:
    Action(const ToDraw *, int,
           const ToDraw *, int);
    inline const ToDraw &DrawnBefore() const {return drawnBefore;}
    inline int ZOrderBefore() const {return zOrderBefore;}
    inline const ToDraw &DrawnAfter() const {return drawnAfter;}
    inline int ZOrderAfter() const {return zOrderAfter;}

  private:
    ToDraw drawnBefore, drawnAfter;
    int zOrderBefore, zOrderAfter;
};

#endif
