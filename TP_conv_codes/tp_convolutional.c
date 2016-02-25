// This file contains the functions for encoding and decoding convolutional codes
#define INFINITY     1.e10    // +Inf

// Prototypes of the functions

int Initialize_convolutional_code(void);                                       // Initialize the encoder
int Encode(unsigned char *b, unsigned char *x, unsigned int n);                // Encode a frame
int Viterbi(double **w, unsigned char *b_est,unsigned int n);                  // Viterbi decoder
int BCJR(double **w, unsigned char *b_est, unsigned int n);                    // BCJR    decoder
double LogSum(double a,double b);                                              // LogSum  function

int Initialize_convolutional_code(void)
{
  // This function initializes the encoder: first of all it converts the generators in binary format
  //                                        then creates the Forward,Backward matrices

  int i,j,k,s,s1,gen;                    // counters
  
  unsigned short *octal_gen_pointer;     // pointer to an octal generator
  unsigned short temp;                   // temp variable
  unsigned short mask;                   // mask
  unsigned char  sum;                    // variable used to compute the encoder output

  // converts the octal generators in binary format 
  for(gen=0;gen<N;gen++)                        // for each generator
  {
    octal_gen_pointer = &octal_gen[gen][M/3];   // octal_gen_pointer points to the last octal digit of the i-th generator
    temp = *octal_gen_pointer--;                // temp now contains this digit
    for(k=0,j=0;j<M+1;k++,j++)                  // we extract the binary digits from the octal digits
    {
                                         // every 3 binary digits (one octal digit) it loads in temp the new octal digit
      if(k==3)
      {
        k=0;
        temp = *octal_gen_pointer--;
      }
      generators[gen][M-j] = temp&1;
      temp >>=1;
    }
  }
 
  for(s=0;s<N_STATES;s++)                   // For each state
  {
                // the next state s' is given by shifting the state by 1 position and adding the new bit in the first position 
    Forward[s][0][0] = s>>1;               // next state when input is 0 and state is 'i'  we shift and we add nothing (0)
    Forward[s][1][0] = (s>>1)+(1<<(M-1));  // next state when input is 1 and state is 'i'  we shift and we add 2^(M-1) (1)

    Forward[s][0][1] = 0;
    Forward[s][1][1] = 0;

    for(gen=0;gen<N;gen++)
    {
      sum = 0;
      for(k=0,i=M;i>0;i--,k++)
        sum+= generators[gen][i]*((s>>k)&1);

      Forward[s][0][1] += (sum&1)<<gen;

      sum+= generators[gen][0];      
      Forward[s][1][1] += (sum&1)<<gen;
    }
  }

  mask = (1<<M)-1;

  for(s1=0;s1<N_STATES;s1++)                // For each state
  {
                                            // given the state ' s1 ' and the transition index 'i'  then
                                            // Backward[s1][i][0] = s    is the old state
                                            // Backward[s1][i][1] = out  is the index of the transition output
                                            //                           e.g. for N=2 (rate 1/2) 
                                            //                           0 -> 00;  1 -> 01; 2 -> 10; 3 = 11;
                                            //                           the convention is  xN,...,x1;
                                            // Backward[s1][i][2] = b    is the input bit that generated the transition
    
    Backward[s1][0][0] = (s1<<1)&mask;          // old state s for the first  possible transitions
    Backward[s1][1][0] = Backward[s1][0][0] +1; // old state s for the second possible transitions 

    Backward[s1][0][2] = (s1>>(M-1))&1;     // the 2 transitions have the same input bit
    Backward[s1][1][2] = (s1>>(M-1))&1;     // the 2 transitions have the same input bit

    // Here we use the Forward matrix to compute the Backward
    Backward[s1][0][1] = Forward[Backward[s1][0][0]][Backward[s1][0][2]][1];
    Backward[s1][1][1] = Forward[Backward[s1][1][0]][Backward[s1][1][2]][1];
  }
  
  
  // Write convolutional code parameters to stdout
  printf("*************************************************\n");
  printf("Convolutional code  K=%d, N=%d, M=%d, States=%d\n",K,N,M,1<<M);
  printf("  Generators (octal  format): \n");
  for(gen=0;gen<N;gen++)
  {
     printf("    ");
     for(j=0;j<(M/3+1);j++)
       printf("%d ",octal_gen[gen][j]);
     printf("\n");
  }
  printf("\n  Generators (binary format): \n");
  for(gen=0;gen<N;gen++)
  {
     printf("    ");
     for(j=0;j<(M+1);j++)
       printf("%d",generators[gen][j]);
     printf("\n");
  }
  printf("\n  Forward   matrix:\n");

  printf("     Next state\n"); 
  for(s=0;s<N_STATES;s++)
  { 
     printf("    ");
     for(j=0;j<(1<<K);j++)
       printf(" %3d ",Forward[s][j][0]);
     printf("\n");
  }

  printf("     Output index \n");     
  for(s=0;s<N_STATES;s++)
  {
     printf("    ");
     for(j=0;j<(1<<K);j++)
       printf(" %3d ",Forward[s][j][1]);
     printf("\n");
  }


  printf("\n  Backward  matrix: \n");

  printf("     Old state\n");  
  for(s=0;s<N_STATES;s++)
  {
     printf("    ");
     for(j=0;j<(1<<K);j++)
       printf(" %3d ",Backward[s][j][0]);
     printf("\n");
  }

  printf("     Old output\n");  
  for(s=0;s<N_STATES;s++)
  { 
    printf("    ");
    for(j=0;j<(1<<K);j++)
       printf(" %3d ",Backward[s][j][1]);
    printf("\n");
  }

  printf("     Old input\n");    
  for(s=0;s<N_STATES;s++)
  {   
    printf("    ");
    for(j=0;j<(1<<K);j++)
      printf(" %3d ",Backward[s][j][2]);
    printf("\n");
  }
  printf("\n*************************************************\n");
  fflush(stdout);

  return(0);
}

int Viterbi(double **w, unsigned char *b_est, unsigned int n)
{
  // Viterbi decoder
  //
  //  w       - pointer to the metric matrix
  //  b_est   - pointer to the decoded bits vector
  //  n       - number of bits to decode

  double        a0[N_STATES];           // accumulated metric
  double        a1[N_STATES];           // accumulated metric
  static unsigned char **Z0;            // array used to store the old state
  static unsigned char **Z1;            // array used to store the old input bit
  static int first_call=0;

  // *******  ADD HERE THE EXTRA VARIABLES YOU NEED *************  


  // ************************************************************

  if(first_call==0)
  {
    // This part of the routine is executed only when the function is called the fist time
    // the successive call do not need to allocate mempry for the internal structures
    // because the pointers are declared as 'static' 
    // allocate memory
    Z0 = (unsigned char **)Alloc_array(sizeof(unsigned char),2,N_STATES,n); // array used to store the old state
    Z1 = (unsigned char **)Alloc_array(sizeof(unsigned char),2,N_STATES,n); // array used to store the old input bit

    first_call=1;
  }

  // *******  ADD THE REQUIRED CODE IN THIS SECTION *************
  // Initialize path metrics

  // Viterbi forward recursion

  // Trace back 
    
  // ***********************************************************

  return(0);
}

int Encode(unsigned char *b, unsigned char *x, unsigned int n)
{
  // This function encodes the source bit stream pointed by b
  //
  // b    - pointer to the source bit stream
  // x    - pointer to the encoded bits
  // n    - length of the source bit stream

  unsigned int i,j;            // counters
  unsigned short state = 0;    // state  initialized to 0
  unsigned short input_bit,output;

  for(i=0;i<n;i++)
  {
    input_bit = b[i];
    output = Forward[state][input_bit][1];
    state  = Forward[state][input_bit][0];    

    for(j=0;j<N;j++)
      *x++ = (output>>j)&1;
  }

  input_bit = 0;                              // for the trellis termination input_bit=0
  for(i=0;i<M;i++)
  {
    output = Forward[state][input_bit][1]; 
    state  = Forward[state][input_bit][0];
    for(j=0;j<N;j++)
      *x++ = (output>>j)&1;
  }  

  return(0);
}

int BCJR(double **w, unsigned char *b_est, unsigned int n)
{
  // BCJR decoder
  //
  //  w       - pointer to the symbol metric matrix
  //  b_est   - pointer to the decoded bits
  //  n       - number of bits to decode

  double        Beta0[N_STATES];            // Vector used in the backward recursion
  double        Beta1[N_STATES];            // Vector used in the backward recursion
  double        APPin[2];                   // A posteriori probability vector for an input bit
  static double **Alpha;                    // Matrix for forward recursion
  static int first_call=0;                  // static variable that tells if it's the 1st time you call the function

  // *******  ADD HERE THE EXTRA VARIABLES YOU NEED *************  


  // ************************************************************

  if(first_call==0)
  {
    Alpha  = (double **)Alloc_array(sizeof(double),2,N_STATES,n+1);
    first_call=1;
  }

  // *******  ADD THE REQUIRED CODE IN THIS SECTION *************
    // initialize the array (Alpha) for the forward recursion

    // BCJR Forward recursion

    // BCJR Backward recursion
    
  // ************************************************************
  
  return(0);
}

double LogSum(double a,double b)
{
  double d;

  d = b - a;

  if(d<=0)
    return(a + log(1 + exp(d)));
  else     
    return(b + log(1 + exp(-d)));
}





