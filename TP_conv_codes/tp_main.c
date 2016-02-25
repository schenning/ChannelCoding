// Main file for TP2_2001
//
// Use the following command line to compile
//
// gcc tp_main.c -I. -lm -O -o tp_main

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tp_alloc.c"         // includes some functions for memory allocation   (YOU DON'T NEED TO MODIFY IT)
#include "tp_random.c"        // includes some useful random generators          (YOU DON'T NEED TO MODIFY IT)

// **** MAIN DEFINITIONS: MODIFY IT ACCORDING TO THE PARAMETERS GIVEN TO YOUR GROUP ***************************

#define K 1                   // DO NOT MODIFY! K=1 for this TP
#define N 2                   // output size [bit]  this depends on the code that has been given to you
#define M 2                   // code memory [bit]  this depends on the code that has been given to you
#define N_STATES (1<<M)       // number of states 2^M

// **** END MAIN DEFINITIONS **********************************************************************************

// Global variables 

// ******** INITIALIZE THE OCTAL GENERETORS ARRAY HERE BELOW ACCORDING TO THE GIVEN GENERATORS ****************
unsigned short octal_gen[N][M/3 +1] = {{5},{7}};   // convolutional code generators in octal  format
                                                   // Read the generators that have been assigned to you and set
                                                   // this array.

unsigned char  generators[N][M+1];                 // convolutional code generators in binary format

unsigned short Forward[N_STATES][2][2];            // forward trellis matrix. 
                                                   // given the state 's' and the input bit 'b' then
                                                   // Forward[s][b][0] = s'     is the next state
                                                   // Forward[s][b][1] = out    is the index of the transition output
                                                   //                           e.g. for N=2 (rate 1/2) 
                                                   //                           0 -> 00;  1 -> 01; 2 -> 10; 3 = 11;
                                                   //                           the convention is  xN,...,x2,x1;

unsigned char  Backward[N_STATES][2][3];           // backward trellis matrix
                                                   // given the state ' s' ' and the transition index 'i'  then
                                                   // Backward[s'][i][0] = s    is the old state
                                                   // Backward[s'][i][1] = out  is the index of the transition output
                                                   //                           e.g. for N=2 (rate 1/2) 
                                                   //                           0 -> 00;  1 -> 01; 2 -> 10; 3 = 11;
                                                   //                           the convention is  xN,...,x1;
                                                   // Backward[s'][i][2] = b    is the input bit that generated the transition

// ******* THE MAIN GOAL OF THE TP CONSISTS IN MODIFYING THE FILE INCLUDED HERE BELOW **************************
#include "tp_convolutional.c" // Includes the functions that handle convolutional codes:
                              // e.g. the encoder, and the decoders (Viterbi and BCJR)

//  main simulator functions
int Generate_source_bits(unsigned char *b, unsigned int n);
int AWGN(unsigned char *x, double *y, unsigned int n, double esn0);      // Does BPSK modulation and adds AWGN noise
int Compute_metric(double *y, double **w, unsigned int n, double amp);   // Computes the trellis metrics
int Error_count(unsigned char *x1, unsigned char *x2, unsigned int n);   // Counts the errors

// *********************  MAIN *********************************************************************************

int main(void)
{
  unsigned int  Frame_length;                      // Frame length             :  length(b)
  unsigned int  Coded_frame_length;                // Length of the coded frame:  length(x)  
  unsigned char *b;                                // Pointer to the frame. The frame contains the source bits
  unsigned char *b_est;                            // Pointer to the decoded bits. It contains the estimation
                                                   //   of the source bits 
  unsigned char *x;                                // Pointer to the coded frame.
  double        *y;                                // Received signal. Each element is a BPSK symbol corrupted with AWGN noise
  double        **w;                               // Symbol Metric matrix

  unsigned int  errors_viterbi,errors_BCJR;        // error counters
  double        ebn0dB,ebn0,esn0;                  // Eb/N0 [dB], Eb/N0 (linear scale)  and Es/N0  (linear scale)
  double        code_rate;                         // Code rate

  // ********* IF YOU NEED TO DECLARE SOME EXTRA VARIABLES DO IT HERE *********************

  // **************************************************************************************

  // ***** SET THESE VARIABLES AS YOU PREFERE *******************
  Random       = ;                                 // Initialize the random seed (Random= 0 -> seed is chosen randomly) 
  Frame_length = ;                                 // Set the frame length
  ebn0dB       = ;                                 // Set the Eb/N0 [dB]
  // ***** END SETTING THESE VARIABLES AS YOU PREFERE ************

  // ***** COMPUTE THE VARIABLES LISTED BELOW *******************
  Coded_frame_length = ;                           // Compute the lengt of the coded frame
  code_rate          = ;                           // Compute the Code rate
  ebn0               = ;                           // Compute ebn0 (linear scale)
  esn0               = ;                           // Compute esn0 (linear scale)
  // ***** END COMPUTING VARIABLES  *****************************

  // allocate the memory required by the simulator
  b       = (unsigned char *)Alloc_array(sizeof(unsigned char),1,Frame_length);       // alloc memory for b
  b_est   = (unsigned char *)Alloc_array(sizeof(unsigned char),1,Frame_length+M);     // alloc memory for b_est
  x       = (unsigned char *)Alloc_array(sizeof(unsigned char),1,Coded_frame_length); // alloc memory for x
  y       = (double *)       Alloc_array(sizeof(double),1,Coded_frame_length);        // alloc memory for y
  w       = (double **)      Alloc_array(sizeof(double),2,(1<<N),Frame_length+M); // alloc memory for w

  Initialize_convolutional_code();                                           // Initialize Forward and Backward

  // ********************** Start simulations **********************
                                             
  Generate_source_bits(b, Frame_length);                   // Generate the frame (b)
  Encode(b, x, Frame_length);                              // Encode   the frame (b) in the coded frame (x)
  AWGN(x, y, Coded_frame_length, esn0);                    // Modulate (x) (BPSK) and add white gaussian noise, output is (y)

  Compute_metric( y, w, Frame_length+M, sqrt(esn0));   // Compute the symbol by symbol metric
  Viterbi(w, b_est, Frame_length+M);                       // Call Viterbi decoder
  errors_viterbi = Error_count(b, b_est, Frame_length);    // Count the errors (Viterbi decoder)

  BCJR(w, b_est, Frame_length+M);                          // Call BCJR decoder
  errors_BCJR    = Error_count(b, b_est, Frame_length);    // Count the errors (BCJR    decoder)

  // Print the output
  printf(" Eb/N0[dB]\t= %g dB\n",ebn0dB);
  printf(" Eb/N0\t\t= %g\n",ebn0);
  printf(" Es/N0\t\t= %g\n",esn0);
  printf(" Nominal rate \t= %f\n",(double)K/N);
  printf(" True    rate \t= %f\n",code_rate);
  printf(" Frame length\t= %d\n\n",Frame_length);
  printf("        \t Viterbi \t BCJR\n");
  printf(" Errors:\t %6d\t\t%6d\n",errors_viterbi,errors_BCJR);
  printf(" BER:   \t %6.2e\t%6.2e\n",(double)(errors_viterbi)/Frame_length,(double)(errors_BCJR)/Frame_length);

  printf("\n************* END SIMULATION ********************\n\n");
  return(0);
}

// ********************** Main simulator functions ****************************************************************************************

int Error_count(unsigned char *x1, unsigned char *x2, unsigned int n)
{
  // This function compares the contents of the vectors x1 and x2, both of length n
  // It returns the number of differences (errors)

  unsigned int i;
  unsigned int errors = 0;

  // count the differences between x1 and x2
  for(i=0;i<n;i++)
  {
    if(x1[i]!=x2[i])
      errors++;
  }
  return(errors);
}

int AWGN(unsigned char *x, double *y, unsigned int n, double esn0)
{
  // This function takes as input the bit stream x and produces the signal y.
  // Each bit of x is BPSK modulated and multiplied by the amplitude of the signal and eventually
  // white gaussian noise is addded
  //
  // x    - is the pointer to the coded stream        (length(x) = n )
  // y    - is the pointer to the received symbols of (length(y) = n )
  // n    - the length of the streams
  // esn0 - Es/N0 at the receiver (linear scale)
  //
  // REMARKS: 1) We use BPSK modulation 0 -> -1; 1 -> +1
  //          2) Noise generation: use the function 'Gaussian2(double *real,double *imag)' provided in "tp_random.c"
  //             This function generates a complex sample of white gaussian noise  noise = (real,imag) 
  //             where 'real' and 'imag' have both zero mean and variance 1/2

  unsigned int i;             // counter
  double amp;                 // amplitude of the signal
  double nr,ni;               // sample of gaussian noise (real,imag)

  // Compute the signal amplitude
  amp = sqrt(esn0);

  // modulate each symbol and add white gaussian noise
  for(i=0;i<n;i++)
  {
    Gaussian2(&nr, &ni);                    // produce the noise samples (re,im)
    y[i] = amp*(2*x[i]-1)+ nr;              // modulate and add noise
  }

  return(0);
}

int Compute_metric(double *y, double **w, unsigned int n, double amp)
{
  // This function computes the metric m for each symbol using the received signal y
  //   and the signal amplitude amp
  // y is the pointer to the received signal
  // w is the pointer to the metric matrix (2^N rows, n columns)
  // n is the length of y ( Frame_Length+M)
  // amp is the amplitude of the signal

  // ***** COMPUTE THE TRELLIS METRIC MATRIX *******
  // for each transition compute the corresponding metric


  // ***** END COMPUTING THE TRELLIS METRIC MATRIX ************
  return(0);
}

int Generate_source_bits(unsigned char *b, unsigned int n)
{
  // b   - the pointer to the source bit stream of length n
  // n   - the length of the stream (is also the number of bit generated by the function)
  // 
  // REMARK : to generate the bits we use the function 'int GenBit(void)' provided in "tp_random.c"
 
  unsigned int i;     // bit counter

  // generate bit
  for(i=0;i<n;i++)
    b[i] = GenBit();  
  return(0);
}

