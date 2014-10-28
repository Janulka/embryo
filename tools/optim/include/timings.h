#ifndef CMAES_TIMINGS_H
#define CMAES_TIMINGS_H

#include <ctime>



namespace cmaes {
  /* timings_t
   * time measurement, used to time eigendecomposition 
   */
  struct timings {
    /* for outside use */
    double totaltime;
    double tictoctime;
    double lasttictoctime;

    /* local fields */
    clock_t lastclock;
    time_t lasttime;
    clock_t ticclock;
    time_t tictime;
    short istic;
    short isstarted;

    double lastdiff;
    double tictoczwischensumme;



    void start();

    double update();

    void tic();

    double toc();
  }
  ; // struct timings
} // namespace cmaes



#endif /* CMAES_TIMINGS_H */
