#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "exception.h"
#include "readpara.h"

using namespace std;
using namespace cmaes;



void
cmaes::readpara_init(readpara_t *t,
               int dim,
               int inseed,
               const double * inxstart,
               const double * inrgsigma,
               int lambda,
               const char * filename) {
  int i, N;

  /*
  t->rgsformat = (char **) new_void(45, sizeof(char *));
  t->rgpadr = (void **) new_void(45, sizeof(void *));
  t->rgskeyar = (char **) new_void(11, sizeof(char *));
  t->rgp2adr = (double ***) new_void(11, sizeof(double **));
  t->weigkey = (char *)new_void(7, sizeof(char));
  */

  t->rgsformat = new char*[45];
  t->rgpadr = new void*[45];
  t->rgskeyar = new char*[11];
  t->rgp2adr = new double**[11];
  t->weigkey = new char[7];
  
  /* All scalars:  */
  i = 0;
  t->rgsformat[i] = " N %d";
  t->rgpadr[i++] = (void *) &t->N;
  t->rgsformat[i] = " seed %d";
  t->rgpadr[i++] = (void *) &t->seed;
  t->rgsformat[i] = " stopMaxFunEvals %lg";
  t->rgpadr[i++] = (void *) &t->stopMaxFunEvals;
  t->rgsformat[i] = " stopMaxIter %lg";
  t->rgpadr[i++] = (void *) &t->stopMaxIter;
  t->rgsformat[i] = " stopFitness %lg";
  t->rgpadr[i++]=(void *) &t->stStopFitness.val;
  t->rgsformat[i] = " stopTolFun %lg";
  t->rgpadr[i++]=(void *) &t->stopTolFun;
  t->rgsformat[i] = " stopTolFunHist %lg";
  t->rgpadr[i++]=(void *) &t->stopTolFunHist;
  t->rgsformat[i] = " stopTolX %lg";
  t->rgpadr[i++]=(void *) &t->stopTolX;
  t->rgsformat[i] = " stopTolUpXFactor %lg";
  t->rgpadr[i++]=(void *) &t->stopTolUpXFactor;
  t->rgsformat[i] = " lambda %d";
  t->rgpadr[i++] = (void *) &t->lambda;
  t->rgsformat[i] = " mu %d";
  t->rgpadr[i++] = (void *) &t->mu;
  t->rgsformat[i] = " weights %5s";
  t->rgpadr[i++] = (void *) t->weigkey;
  t->rgsformat[i] = " fac*cs %lg";
  t->rgpadr[i++] = (void *) &t->cs;
  t->rgsformat[i] = " fac*damps %lg";
  t->rgpadr[i++] = (void *) &t->damps;
  t->rgsformat[i] = " ccumcov %lg";
  t->rgpadr[i++] = (void *) &t->ccumcov;
  t->rgsformat[i] = " mucov %lg";
  t->rgpadr[i++] = (void *) &t->mucov;
  t->rgsformat[i] = " fac*ccov %lg";
  t->rgpadr[i++]=(void *) &t->ccov;
  t->rgsformat[i] = " updatecov %lg";
  t->rgpadr[i++]=(void *) &t->updateCmode.modulo;
  t->rgsformat[i] = " maxTimeFractionForEigendecompostion %lg";
  t->rgpadr[i++]=(void *) &t->updateCmode.maxtime;
  t->rgsformat[i] = " resume %59s";
  t->rgpadr[i++] = (void *) t->resumefile;
  t->rgsformat[i] = " fac*maxFunEvals %lg";
  t->rgpadr[i++] = (void *) &t->facmaxeval;
  t->rgsformat[i] = " fac*updatecov %lg";
  t->rgpadr[i++]=(void *) &t->facupdateCmode;
  t->n1para = i;
  t->n1outpara = i-2; /* disregard last parameters in WriteToFile() */

  /* arrays */
  i = 0;
  t->rgskeyar[i]  = " typicalX %d";
  t->rgp2adr[i++] = &t->typicalX;
  t->rgskeyar[i]  = " initialX %d";
  t->rgp2adr[i++] = &t->xstart;
  t->rgskeyar[i]  = " initialStandardDeviations %d";
  t->rgp2adr[i++] = &t->rgInitialStds;
  t->rgskeyar[i]  = " diffMinChange %d";
  t->rgp2adr[i++] = &t->rgDiffMinChange;
  t->n2para = i;

  t->N = dim;
  t->seed = inseed;
  t->xstart = NULL;
  t->typicalX = NULL;
  t->typicalXcase = 0;
  t->rgInitialStds = NULL;
  t->rgDiffMinChange = NULL;
  t->stopMaxFunEvals = -1;
  t->stopMaxIter = -1;
  t->facmaxeval = 1;
  t->stStopFitness.flg = -1;
  t->stopTolFun = 1e-12;
  t->stopTolFunHist = 1e-13;
  t->stopTolX = 0; /* 1e-11*insigma would also be reasonable */
  t->stopTolUpXFactor = 1e3;

  t->lambda = lambda;
  t->mu = -1;
  t->mucov = -1;
  t->weights = NULL;
  strcpy(t->weigkey, "log");

  t->cs = -1;
  t->ccumcov = -1;
  t->damps = -1;
  t->ccov = -1;

  t->updateCmode.modulo = -1;
  t->updateCmode.maxtime = -1;
  t->updateCmode.flgalways = 0;
  t->facupdateCmode = 1;
  strcpy(t->resumefile, "_no_");

  if (strcmp(filename, "non") != 0 && strcmp(filename, "writeonly") != 0)
    readpara_ReadFromFile(t, filename);

  if (t->N <= 0)
    t->N = dim;

  N = t->N;
  if (N == 0) {
    ostringstream lBuffer;
    lBuffer 
    << "readpara_readpara_t(): problem dimension N undefined.\n"
    << "  (no default value available).";
    throw Exception(lBuffer.str());
  }

  if (t->xstart == NULL && inxstart == NULL && t->typicalX == NULL)
    cerr << "Warning: initialX undefined. typicalX = 0.5...0.5 used.";

  if (t->rgInitialStds == NULL && inrgsigma == NULL)
    cerr << "Warning: initialStandardDeviations undefined. 0.3...0.3 used.";

  if (t->xstart == 0) {
    t->xstart = new double[N];

    /* put inxstart into xstart */
    if (inxstart != NULL) {
      for (i=0; i<N; ++i)
        t->xstart[i] = inxstart[i];
    }
    /* otherwise use typicalX or default */
    else {
      t->typicalXcase = 1;
      for (i=0; i<N; ++i)
        t->xstart[i] = (t->typicalX == NULL) ? 0.5 : t->typicalX[i];
    }
  } /* xstart == NULL */

  if (t->rgInitialStds == NULL) {
    t->rgInitialStds = new double[N];
    for (i=0; i<N; ++i)
      t->rgInitialStds[i] = (inrgsigma == NULL) ? 0.3 : inrgsigma[i];
  }

  readpara_SupplementDefaults(t);
  
  /*
   * Alexandre Devert : Do not want that file to be generated
   */
  /*
  if (strcmp(filename, "non") != 0)
    readpara_WriteToFile(t, "actparcmaes.par", filename);
  */
} /* readpara_init */



void 
cmaes::readpara_exit(readpara_t *t) {
  if (t->xstart != 0) /* not really necessary */
    delete[] t->xstart;

  if (t->typicalX != 0)
    free( t->typicalX);

  if (t->rgInitialStds != 0)
    delete[] t->rgInitialStds;

  if (t->rgDiffMinChange != 0)
    free( t->rgDiffMinChange);

  if (t->weights != 0)
    delete[] t->weights;

  delete[] t->rgsformat;
  delete[] t->rgpadr;
  delete[] t->rgskeyar;
  delete[] t->rgp2adr;
  delete[] t->weigkey;
}



void
cmaes::readpara_ReadFromFile(readpara_t *t, const char * filename) {
  char s[1000], *ss = "initials.par";
  int ipara, i;
  int size;
  FILE *fp;
  if (filename == NULL)
    filename = ss;

  fp = fopen(filename, "r");
  if (fp == 0) {
    ostringstream lBuffer;
    lBuffer << "cmaes_ReadFromFile(): could not open '" << filename << "'";
    throw Exception(lBuffer.str());
  }

  for (ipara=0; ipara < t->n1para; ++ipara) {
    rewind(fp);
    while(fgets(s, sizeof(s), fp) != NULL) { /* skip comments  */
      if (s[0] == '#' || s[0] == '%')
        continue;
      if(sscanf(s, t->rgsformat[ipara], t->rgpadr[ipara]) == 1) {
        if (strncmp(t->rgsformat[ipara], " stopFitness ", 13) == 0)
          t->stStopFitness.flg = 1;
        break;
      }
    }
  } /* for */

  if (t->N <= 0)
    throw Exception("readpara_ReadFromFile(): No valid dimension N");
  
  for (ipara=0; ipara < t->n2para; ++ipara) {
    rewind(fp);
    while(fgets(s, sizeof(s), fp) != NULL) /* read one line */
    { /* skip comments  */
      if (s[0] == '#' || s[0] == '%')
        continue;
      if(sscanf(s, t->rgskeyar[ipara], &size) == 1) { /* size==number of values to be read */
        if (size > 0) {
          *t->rgp2adr[ipara] = new double[t->N];
          for (i=0;i<size&&i<t->N;++i) /* start reading next line */
            if (fscanf(fp, " %lf", &(*t->rgp2adr[ipara])[i]) != 1)
              break;
          if (i<size && i < t->N) {
            ostringstream lBuffer;
            lBuffer
            << "readpara_ReadFromFile '" << filename << "': "
            << "'" << t->rgskeyar[ipara]
            << "' not enough values found.\n"
            << "   Remove all comments between numbers.";
            throw Exception(lBuffer.str());
          }
          for (; i < t->N; ++i) /* recycle */
            (*t->rgp2adr[ipara])[i] = (*t->rgp2adr[ipara])[i%size];
        }
      }
    }
  } /* for */
  fclose(fp);
  return;
} /* readpara_ReadFromFile() */



void
cmaes::readpara_WriteToFile(readpara_t *t, const char *filenamedest,
                     const char *filenamesource) {
  int ipara, i;
  size_t len;
  time_t ti = time(NULL);

  FILE *fp = fopen(filenamedest, "a");
  if (fp == 0) {
    ostringstream lBuffer;
    lBuffer << "cmaes_WriteToFile(): could not open '" << filenamedest << "'";
    throw Exception(lBuffer.str());
  }

  fprintf(fp, "\n# Read from %s at %s\n", filenamesource,
          asctime(localtime(&ti))); /* == ctime() */
  for (ipara=0; ipara < 1; ++ipara) {
    fprintf(fp, t->rgsformat[ipara], *(int *)t->rgpadr[ipara]);
    fprintf(fp, "\n");
  }
  for (ipara=0; ipara < t->n2para; ++ipara) {
    if(*t->rgp2adr[ipara] == NULL)
      continue;
    fprintf(fp, t->rgskeyar[ipara], t->N);
    fprintf(fp, "\n");
    for (i=0; i<t->N; ++i)
      fprintf(fp, "%7.3g%c", (*t->rgp2adr[ipara])[i], (i%5==4)?'\n':' ');
    fprintf(fp, "\n");
  }
  for (ipara=1; ipara < t->n1outpara; ++ipara) {
    if (strncmp(t->rgsformat[ipara], " stopFitness ", 13) == 0)
      if(t->stStopFitness.flg == 0) {
        fprintf(fp, " stopFitness\n");
        continue;
      }
    len = strlen(t->rgsformat[ipara]);
    if (t->rgsformat[ipara][len-1] == 'd') /* read integer */
      fprintf(fp, t->rgsformat[ipara], *(int *)t->rgpadr[ipara]);
    else if (t->rgsformat[ipara][len-1] == 's') /* read string */
      fprintf(fp, t->rgsformat[ipara], (char *)t->rgpadr[ipara]);
    else {
      if (strncmp(" fac*", t->rgsformat[ipara], 5) == 0) {
        fprintf(fp, " ");
        fprintf(fp, t->rgsformat[ipara]+5, *(double *)t->rgpadr[ipara]);
      } else
        fprintf(fp, t->rgsformat[ipara], *(double *)t->rgpadr[ipara]);
    }
    fprintf(fp, "\n");
  } /* for */
  fprintf(fp, "\n");
  fclose(fp);
} /* readpara_WriteToFile() */



void
cmaes::readpara_SupplementDefaults(readpara_t *t) {
  double t1, t2;
  int N = t->N;
  clock_t cloc = clock();

  if (t->seed < 1) {
    while ((int) (cloc - clock()) == 0)
      ; /* TODO: remove this for time critical applications!? */
    t->seed = (unsigned int)abs(100*time(0)+clock());
  }

  if (t->stStopFitness.flg == -1)
    t->stStopFitness.flg = 0;

  if (t->lambda < 2)
    t->lambda = 4+(int)(3*log((double)N));
  if (t->mu == -1) {
    t->mu = t->lambda/2;
    readpara_SetWeights(t, t->weigkey);
  }
  if (t->weights == NULL)
    readpara_SetWeights(t, t->weigkey);

  if (t->cs > 0) /* factor was read */
    t->cs *= (t->mueff + 2.) / (N + t->mueff + 3.);
  if (t->cs <= 0 || t->cs >= 1)
    t->cs = (t->mueff + 2.) / (N + t->mueff + 3.);

  if (t->ccumcov <= 0 || t->ccumcov > 1)
    t->ccumcov = 4. / (N + 4);

  if (t->mucov < 1) {
    t->mucov = t->mueff;
  }
  t1 = 2. / ((N+1.4142)*(N+1.4142));
  t2 = (2.*t->mueff-1.) / ((N+2.)*(N+2.)+t->mueff);
  t2 = (t2 > 1) ? 1 : t2;
  t2 = (1./t->mucov) * t1 + (1.-1./t->mucov) * t2;
  if (t->ccov >= 0) /* ccov holds the read factor */
    t->ccov *= t2;
  if (t->ccov < 0 || t->ccov > 1) /* set default in case */
    t->ccov = t2;

  if (t->stopMaxFunEvals == -1)  /* may depend on ccov in near future */
    t->stopMaxFunEvals = t->facmaxeval*900*(N+3)*(N+3);
  else
    t->stopMaxFunEvals *= t->facmaxeval;

  if (t->stopMaxIter == -1)
    t->stopMaxIter = ceil((double)(t->stopMaxFunEvals / t->lambda));

  if (t->damps < 0)
    t->damps = 1; /* otherwise a factor was read */
  t->damps = t->damps
             * (1 + 2*max(0., sqrt((t->mueff-1.)/(N+1.)) - 1))     /* basic factor */
             * max(0.3, 1. -                                       /* modify for short runs */
                      (double)N / (1e-6+min(t->stopMaxIter, t->stopMaxFunEvals/t->lambda)))
             + t->cs;                                                 /* minor increment */

  if (t->updateCmode.modulo < 1)
    t->updateCmode.modulo = 1./t->ccov/(double)(N)/10.;
  t->updateCmode.modulo *= t->facupdateCmode;
  if (t->updateCmode.maxtime < 0)
    t->updateCmode.maxtime = 0.20; /* maximal 20% of CPU-time */

} /* readpara_SupplementDefaults() */



void
cmaes::readpara_SetWeights(readpara_t *t, const char* mode) {
  double s1, s2;
  int i;

  if (t->weights != 0)
    delete[] t->weights;

  t->weights = new double[t->mu];

  if (strcmp(mode, "lin") == 0)
    for (i=0; i<t->mu; ++i)
      t->weights[i] = t->mu - i;
  else if (strncmp(mode, "equal", 3) == 0)
    for (i=0; i<t->mu; ++i)
      t->weights[i] = 1;
  else if (strcmp(mode, "log") == 0)
    for (i=0; i<t->mu; ++i)
      t->weights[i] = log(t->mu+1.)-log(i+1.);
  else
    for (i=0; i<t->mu; ++i)
      t->weights[i] = log(t->mu+1.)-log(i+1.);

  /* normalize weights vector and set mueff */
  for (i=0, s1=0, s2=0; i<t->mu; ++i) {
    s1 += t->weights[i];
    s2 += t->weights[i]*t->weights[i];
  }
  t->mueff = s1*s1/s2;
  for (i=0; i<t->mu; ++i)
    t->weights[i] /= s1;

  if(t->mu < 1 || t->mu > t->lambda ||
          (t->mu==t->lambda && t->weights[0]==t->weights[t->mu-1]))
    throw Exception("readpara_SetWeights(): invalid setting of mu or lambda");
}
