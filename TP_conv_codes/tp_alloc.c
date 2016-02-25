

/* Memory allocation functions for the TP



   ************** READ THIS FIRST *************


   The only function you have to call in this library is 

   void *Alloc_array (size_t elem_size, int dim, ...);

   This function allows you to allocate arrays (vector, matrix and arrays with more than 2 dimensions)
    - the first argument  given to the function is the size of the element you want to allocate
    - the second argument 'dim' is the number of dimensions of the array e.g. (1 for vectors, 2 for matrices)
    - the last 'dim' arguments are the size of the array for each dimension

    - the function returns the pointer to the allocated memory

   Example

   Suppose you want to allocate the memory for a matrix of integers with R rows and C columns.
   The matrix has 2 dimensions, the first one with size R and the second one with size C

   1) You declare somewhere the pointer to the matrix
      
      int **matrix;

   2) You allocate the memory using the function Alloc_array

      matrix = (int **)Alloc_array(sizeof(int), 2, R, C);

*/



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

int   chk_bytes = 0;                                     /* byte counter: counts allocated bytes */     
size_t Alloc_array_size[20];                             /* 20 is the maximum dimension of the array (max 20D) */

void CheckIt (int test, char *fmt, ...)
{
  va_list argpt;
  va_start (argpt, fmt);
  if (test) 
  {
    vfprintf (stderr, fmt, argpt);
    exit (1);
  }
  va_end (argpt);
}

void *Check_calloc (size_t nitems, size_t size)          /* alloc memory and update byte counter */
{
  void *tmp;
  tmp = calloc (nitems, size);
  if (tmp == NULL)
  {
    fprintf (stderr, "Memory allocation failure (%d items of size %d bytes)\n", nitems, size);
    fprintf (stderr, "Allocated memory before this call: %d bytes\n", chk_bytes);
    exit (0);
  }
  chk_bytes += nitems*size;
  return tmp;
}

void *Alloc_array1 (size_t elem_size, int dim)            /* allocate xD arrays with x form 1 to 20 */
{
  if (dim == 1)
  {
    return Check_calloc (Alloc_array_size[0], elem_size);
  } 
  else 
 {
    unsigned int i; 
    void **ret;
    assert (dim < sizeof (Alloc_array_size));
    dim--;
    ret = Check_calloc (Alloc_array_size[dim], sizeof (void **));
    for (i = 0; i < Alloc_array_size[dim]; i++) 
    {
      ret[i] = Alloc_array1 (elem_size, dim);
    }
    return ret;
  }
}

void *Alloc_array (size_t elem_size, int dim, ...)        /* This is the function to call to allocate memory. */
{
  int i;
  va_list dimpt;

  va_start (dimpt, dim);
  for (i = 0; i < dim; i++)
  {
    assert ((Alloc_array_size[dim-1-i] = va_arg (dimpt, size_t)) > 0);
  }
  va_end (dimpt);
  assert (dim > 0);
  return Alloc_array1 (elem_size, dim);
}
