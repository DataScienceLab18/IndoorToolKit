#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <math.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctime>   
#include <cstdlib>  



#define dim_books 20
#define dim_shelves 5

FILE * fact;
FILE * predict;
FILE * result;

int fact_matrix[dim_books][dim_shelves];
float predict_matrix[dim_books][dim_shelves];



int main(int argc, char *argv[]) {


int i=0;
int j=0;
double sum=0;
double kl[dim_books];
//double mean;


//initialing output files
fact=fopen("factual_dataset.txt","r");
predict=fopen("inversed.txt","r");
result=fopen("klresult.txt","a+");



// loading fact_matrix


while (fscanf (fact,"%d  ", (int *)&fact_matrix[0][0]+i)!=EOF)
{
  
i++;

}


i=0;

while (fscanf (predict,"%f  ", (float *)&predict_matrix[0][0]+i)!=EOF)
{
  
i++;

}


for(i=0;i<dim_books;i++){

   for(j=0;j<dim_shelves;j++){
         
    if (predict_matrix[i][j]==0) continue;
	
	else  {

    if (fact_matrix[i][j]==0) ;                //  0*log0=0

	else 

	sum=sum+ fact_matrix[i][j]* log(fact_matrix[i][j]/predict_matrix[i][j]);

	}
	
   }

   kl[i]=sum;
   sum=0;
   
}

sum=0;


for (i=0;i<dim_books;i++)    {                  

    //fprintf (result, "The KL divergence is ");

    //fprintf (result, "%f", kl[i]);

	//fprintf (result, "\n");

	 sum=sum+kl[i];	
        
    

}

//printf (" the KL divergence outputted:\n");

fprintf (result, "%f\n",sum/dim_books);


return 0;
}
