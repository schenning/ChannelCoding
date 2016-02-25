// MISCELLANEA FUNCTIONS

// Random Generator
int Random;                   // the initial seed of the random generator

/* Random generators */
#define PIG          3.14159265
#define MODULE       2147483647
#define FACTOR       16807
#define LASTXN       127773
#define UPTOMOD     -2836

double Uniform(void);
int    GenBit(void);
void   Gaussian2 (double *x1, double *x2);

/* Uniform [0,1] */

double Uniform (void)
{
  static int  first_call=1;
  static int seed;
  static int times, rest, prod1, prod2;

  if(first_call) {
    if(Random<=0) {
      seed = time(NULL)%100000000;
    } else {
      seed = Random;
    }
    first_call = 0;
  }
  times    = seed / LASTXN;
  rest     = seed - times * LASTXN;
  prod1    = times * UPTOMOD;
  prod2    = rest * FACTOR;
  seed     = prod1 + prod2;
  if (seed < 0) seed = seed + MODULE;
  return (double) seed / MODULE;
}

/* Uniform on the integers {0,1} */

int GenBit(void)
{
  int a;

  do 
  {
    a = (int) (2 * Uniform());
  } while (a>=2);

  return(a);
}

/* Normal bivariate zero-mean with covariance diag(1/2,1/2) */

void Gaussian2 (double *x1, double *x2)
{
  double u1, u2, s;

  do 
  {
    u1 = Uniform ();
  } while (u1<=0);
  u2  = PIG*(2*Uniform () - 1);
  s   = sqrt(-log(u1));
  *x1 = s*cos(u2);
  *x2 = s*sin(u2);
}

