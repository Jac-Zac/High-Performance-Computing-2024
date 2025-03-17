
//
// defines the type for the testing function
//

typedef int (*test_funct)(int *, int, int); 


//
// Compute time used by the test function
//
double ftime(test_funct, int *, int, int,
	     double *, int, int, double );


//
// returns an "error" estimate
//
double err( double *,  int, int );
