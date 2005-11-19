/* -------------------------------------------------------------
   KDE Tuberling
   Action stored in the undo buffer
   mailto:e.bischoff@noos.fr
 ------------------------------------------------------------- */

#include "action.h"

// Constructor
Action::Action
	(const ToDraw *drawn1, int zOrder1,
	 const ToDraw *drawn2, int zOrder2)
{
  if (drawn1) drawnBefore = *drawn1;
  zOrderBefore = zOrder1;

  if (drawn2) drawnAfter = *drawn2;
  zOrderAfter = zOrder2;
}

