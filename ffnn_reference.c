#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include<assert.h>
#include <sys/mman.h>
#include <sys/time.h>

#define N 784
#define H_NODES 2
#define O_NODES 2
#define WIDTH  2

double round_f(float var) 
{ 
    float value = (int)(var * 1000000 + .5); 
    return (double)value / 1000000; 
} 
int main() 
{
        	FILE *myfile,*file,*fp, *outfile, *SIGM;
       		double myvariable0, myvariable1, outvariable;
                double myvar;
		double secs = 0;
		double w0[784], w1[784];
		double w00[2],w01[2];
       		double w_temp[2];
       		double w00_temp[2];
		double exp_val_hidden[2], sigmoid[2];
		double exp_val_out[2], sum_exp_val, softmax[2];
		double mul_result0[784], mul_result1[784];
                double mul_result00[2],mul_result01[2];
		double acc_temp[2];
		double out_temp[2];
                double out0[2115], out1[2115];
		double temp[2];
		
		int i,j;
		int ii =0;
		int jj =0;
		float x_temp[2115][784];
		struct timeval stop, start;
//==================== reading the weights of the hidden layar =====================//
    myfile=fopen("hidden_wieghts_float.txt", "r");
    for(i = 0; i < N; i++)
    {
        for (j = 0 ; j < WIDTH; j++)
        {
            fscanf(myfile,"%lf",&myvariable0);
            w_temp[j] = myvariable0 ;
        }
        w0[i] = w_temp[0];
        w1[i] = w_temp[1];
      
 //printf("%.9f \n", w0[i]);
    }
//==================================================================================// 

//==================== reading the weights of the output layar =====================//

  fp=fopen("layer2_wieghts_float.txt", "r");
  for(i = 0; i < 2; i++)
  {
    for (j = 0 ; j < 2; j++)
    {
      fscanf(fp,"%lf",&myvariable1);
      w00_temp[j] = myvariable1 ;
    }
   w00[i] = w00_temp[0];
   w01[i] = w00_temp[1];
 // printf("%.9f \n", w01[i]);
}
//==================================================================================// 
//==================== reading the inputs of the hidden layar =====================//   
 
    file = fopen("inputs.txt", "r");
    if(file == NULL) {
        perror("Error opening file");
        return(-1);
    }

for(i = 0; i < 2115; i++)
    {
        for (j = 0 ; j < N; j++)
        {
            fscanf(file,"%lf",&myvar);
            x_temp[i][j] = myvar ;
 //printf("%f ", x_temp[i][j]);
        }
        
//printf("\n\n");
    }
   
//==================================================================================// 
    outfile=fopen("outputs.txt", "r");
    SIGM=fopen("SIGM.txt", "w");
    for(ii=0;ii<2115;ii++) {
        
        acc_temp[0] = 0;
        acc_temp[1] = 0;
        acc_temp[2] = 0;
        acc_temp[3] = 0;
	out_temp[0] = 0;
        out_temp[1] = 0;
        gettimeofday(&start, NULL);

	for (int i = 0; i < N; i++){
		mul_result0[i] = w0[i] *  x_temp[ii][i];
		acc_temp[0] = acc_temp[0] +  mul_result0[i];
		mul_result1[i] = w1[i] * x_temp[ii][i];
		acc_temp[1] = acc_temp[1] + mul_result1[i];
		
		//printf(" w = %.9f ---  x = %d ---  w * x =%.9f  \n",w3[i], matrix[ii][i],mul_result3[i]);
	}

                fprintf(SIGM,"%f		%f\n", acc_temp[0], acc_temp[1]);
        
	for (j =0; j<H_NODES; j++){
		exp_val_hidden[j] = exp(-acc_temp[j]);
		sigmoid[j] = 1 / (1 + exp_val_hidden[j]);
		//printf("Y_node%d = %f \n\n",j, sigmoid[j]);
       		}
              //  fprintf(SIGM,"%f		%f\n", sigmoid[0], sigmoid[1]);
	for ( i = 0; i < 2; i++){
		mul_result00[i] = w00[i] * sigmoid[i];
		out_temp[0] = out_temp[0] + mul_result00[i];
		mul_result01[i] = w01[i] * sigmoid[i];
		out_temp[1] = out_temp[1] + mul_result01[i];
		//printf(" %f	  %f",out_temp[0], out_temp[1] );
	}
	
      //printf(" %f	  %f",out_temp[0], out_temp[1] );	printf("\n");

	sum_exp_val =0;
	for (j =0; j<O_NODES; j++){
		exp_val_out[j] = exp(out_temp[j]);
		sum_exp_val = sum_exp_val + exp_val_out[j];
		//printf("input%d: Y_node%d = %f \n",ii, j, exp_val_out[j]);
       		}
       // printf("input%d: %f \n",ii, sum_exp_val);
	//softmax[0] = round_f(exp_val_out[0] / sum_exp_val);
	//softmax[1] = round_f(exp_val_out[1] / sum_exp_val);
         softmax[0] = exp_val_out[0] / sum_exp_val;
	 softmax[1] = exp_val_out[1] / sum_exp_val;
         	gettimeofday(&stop, NULL);
     // printf("Output %d: %.6f		%.6f \n",ii, softmax[0], softmax[1]);

	for (j = 0 ; j < 2; j++){
	      fscanf(outfile,"%lf",&outvariable);
	      temp[j] = outvariable ;
	    }
		out0[ii] = temp[0];
		out1[ii] = temp[1];
if (ii ==1){
                printf("Before Sigmoid %d: %f		%f\n",ii, acc_temp[0], acc_temp[1]);
                printf("Sigmoid Output %d: %f		%f\n",ii, sigmoid[0], sigmoid[1]);
                printf("Before softmax %d: %f		%f\n",ii, out_temp[0], out_temp[1] );	
                printf("during softmax %d: %f		%f \n",ii, exp_val_out[0],exp_val_out[1]);
                printf("Softmax Output %d: %f		%f \n",ii, softmax[0], softmax[1]);
		printf("Python  Output %d: %f		%f \n",ii, out0[ii], out1[ii]);

}
    }

	//gettimeofday(&stop, NULL);
	//printf("took %lu\n", stop.tv_usec - start.tv_usec);

	secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

printf("=================================================================================\
\n Time Taken(sec) for Software-based MNIST: %f\n====================================================================================\n\
\n", secs);

      
       fclose(file);
       fclose(myfile);
       fclose(fp);
       fclose(outfile);
       fclose(SIGM);
        
	return 0;
}
