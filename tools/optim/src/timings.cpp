#include "timings.h"
#include "exception.h"

using namespace cmaes;



/* --------------------------------------------------------- */
/* --------------- Functions: timings_t -------------------- */
/* --------------------------------------------------------- */
/* timings_t measures overall time and times between calls
 * of tic and toc. For small time spans (up to 1000 seconds)
 * CPU time via clock() is used. For large time spans the
 * fall-back to elapsed time from time() is used.
 * timings_update() must be called often enough to prevent
 * the fallback. */
/* --------------------------------------------------------- */



void
timings::start() {
  totaltime = 0;
  tictoctime = 0;
  lasttictoctime = 0;
  istic = 0;
  lastclock = clock();
  lasttime = time(NULL);
  lastdiff = 0;
  tictoczwischensumme = 0;
  isstarted = 1;
}



double
timings::update() {
  /* returns time between last call of timings_*() and now,
   *    should better return totaltime or tictoctime? 
   */
  double diffc, difft;
  clock_t lc = lastclock; /* measure CPU in 1e-6s */
  time_t lt = lasttime;   /* measure time in s */

  if (isstarted != 1)
    throw Exception("timings_started() must be called before using timings... functions");

  lastclock = clock(); /* measures at most 2147 seconds, where 1s = 1e6 CLOCKS_PER_SEC */
  lasttime = time(NULL);

  diffc = (double)(lastclock - lc) / CLOCKS_PER_SEC; /* is presumably in [-21??, 21??] */
  difft = difftime(lasttime, lt);                    /* is presumably an integer */

  lastdiff = difft; /* on the "save" side */

  /* use diffc clock measurement if appropriate */
  if (diffc > 0 && difft < 1000)
    lastdiff = diffc;

  if (lastdiff < 0)
    throw Exception("Bug in time measurement");

  totaltime += lastdiff;
  if (istic) {
    tictoczwischensumme += lastdiff;
    tictoctime += lastdiff;
  }

  return lastdiff;
}



void
timings::tic() {
  if (istic)
    throw Exception("timings_tic called twice without toc");

  update();
  istic = 1;
}



double
timings::toc() {
  if (!istic)
    throw Exception("timings_toc called without tic");

  update();
  lasttictoctime = tictoczwischensumme;
  tictoczwischensumme = 0;
  istic = 0;

  return lasttictoctime;
}
