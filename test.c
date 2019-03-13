#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
int fact(int n){
  //printf("%d\n",n);
  if (n == 0)
    return 1;
  else
    return(n * fact(n-1));
}

int main(){
    int n;
    double result = 1.0;
    double x = 0;

    #pragma omp parallel for
    for (n = 0; n < 10;n++){
        int i = 2*n;
        printf("%d test = %d \n", n,fact(i));
        double r = pow(-1,n-1)*pow(x,2*n-1)/fact(2*n);
        printf("%d and %f\n",n,r);
        #pragma omp critical
        {
            result -= r;
        }
    }

    #pragma omp parallel 
    {
      int thread = omp_get_thread_num();
      switch (thread)
      {
        case 1:
          printf("something %d\n", 1);
          break;
      
        case 2:
          printf("success %d\n", 2);
          break;
        
      }
    }
    printf("\nit is %f", result);
}