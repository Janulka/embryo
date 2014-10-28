#ifndef CMAES_READ_PARA_H
#define CMAES_READ_PARA_H



namespace cmaes {
  /* readpara_t
   * collects all parameters, in particular those that are read from 
   * a file before to start. This should split in future? 
   */

  struct readpara_t {
    /* input parameter */
    int N; /* problem dimension, must stay constant */
    unsigned int seed;
    double * xstart;
    double * typicalX;
    int typicalXcase;
    double * rgInitialStds;
    double * rgDiffMinChange;

    /* termination parameters */
    double stopMaxFunEvals;
    double facmaxeval;
    double stopMaxIter;
    struct {
      int flg;
      double val;
    }
    stStopFitness;
    double stopTolFun;
    double stopTolFunHist;
    double stopTolX;
    double stopTolUpXFactor;

    /* internal evolution strategy parameters */
    int lambda;          /* -> mu, <- N */
    int mu;              /* -> weights, (lambda) */
    double mucov, mueff; /* <- weights */
    double *weights;     /* <- mu, -> mueff, mucov, ccov */
    double damps;        /* <- cs, maxeval, lambda */
    double cs;           /* -> damps, <- N */
    double ccumcov;      /* <- N */
    double ccov;         /* <- mucov, <- N */
    struct {
      int flgalways;
      double modulo;
      double maxtime;
    }
    updateCmode;
    double facupdateCmode;

    /* supplementary variables */

    char *weigkey;
    char resumefile[99];
    char **rgsformat;
    void **rgpadr;
    char **rgskeyar;
    double ***rgp2adr;
    int n1para, n1outpara;
    int n2para;
  };



  void readpara_init(readpara_t *, int dim, int seed,  const double * xstart,
                     const double * sigma, int lambda, const char * filename);

  void readpara_exit(readpara_t *);

  void readpara_ReadFromFile(readpara_t *, const char *szFileName);

  void readpara_SupplementDefaults(readpara_t *);

  void readpara_SetWeights(readpara_t *, const char * mode);

  void readpara_WriteToFile(readpara_t *, const char *filenamedest,
                            const char *parafilesource);

} // namespace cmaes



#endif /* CMAES_READ_PARA_H */
