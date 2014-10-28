#ifndef CMAES_RANDOM_H
#define CMAES_RANDOM_H



namespace cmaes {
  class randomizer {
  private:
    /* Variables for Uniform() */
    long int startseed;
    long int aktseed;
    long int aktrand;
    long int rgrand[32];

    /* Variables for Gauss() */
    short flgstored;
    double hold;



  public:
    long init(long unsigned inSeed);

    long start(long unsigned inSeed);

    double gauss();

    double uniform();
  }
  ; // class randomizer
} // namespace cmaes



#endif /* CMAES_RANDOM_H */
