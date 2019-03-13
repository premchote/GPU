#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <string.h>


#define FAILURE 0
#define SUCCESS !FAILURE
#define P6 1
#define P3 0
#define USER_NAME "acr18pc"		//replace with your user name

void print_help();
int process_command_line(int argc, char *argv[]);

typedef enum MODE { CPU, OPENMP, CUDA, ALL } MODE;

unsigned int c = 0;
MODE execution_mode = CPU;
char *magic_number;
int magic;
FILE* fr;
int max_colour;
typedef struct {
     unsigned char r,g,b;
} pixel;




typedef struct {
     int x, y;
     pixel *data;
	 int rgb;
} Image;

Image* readImage(char *filename);
void write_P6(char *filename, Image *img);
void write_P3(char *filename, Image *img);
pixel average_CPU(Image *img, int start_row, int start_col,int end_row, int end_col);
pixel average_OPENMP(Image *img, int start_row, int start_col,int end_row, int end_col);
int pixel_indexing(int row,int col,int max_x);
char input_file[500];
Image* mosaic_CPU(Image *img,int c);
Image *input;

int main(int argc, char *argv[]) {
	
	if (process_command_line(argc, argv) == FAILURE)
		return 1;

	Image *img = readImage(input_file); //It checks automatically inside the function whether P3 or P6
	//TODO: read input image file (either binary or plain text PPM) 
	Image *out; //setting output
	pixel average;
	switch (execution_mode){
		case (CPU) : {
			clock_t begin, end;
			begin = clock();
			//TODO: starting timing here
			average = average_CPU(img,0,0,img -> x, img -> y);
			printf("\nAverage r = %d, g = %d and b=%d\n",average.r,average.g,average.b);
			average = average_OPENMP(img,0,0,img -> x, img -> y);
			printf("\nAverage r = %d, g = %d and b=%d\n",average.r,average.g,average.b);
			//average_CPU
			//TODO: calculate the average colour value
			out = mosaic_CPU(img,c);
			// Output the average colour value for the image
			printf("CPU Average image colour red = ???, green = ???, blue = ??? \n");
			write_P6("out.ppm",out); //writing an image
			write_P3("out_Plain_text.ppm",out); //writing the plain text
			//TODO: end timing here
			end = clock();
			double seconds = (end - begin) / (double)CLOCKS_PER_SEC;
			printf("CPU mode execution time took %f s and ???ms\n", seconds);
			break;
		}
		case (OPENMP) : {
			//TODO: starting timing here

			//TODO: calculate the average colour value

			// Output the average colour value for the image
			printf("OPENMP Average image colour red = ???, green = ???, blue = ??? \n");

			//TODO: end timing here
			printf("OPENMP mode execution time took ??? s and ?? ?ms\n");
			break;
		}
		case (CUDA) : {
			printf("CUDA Implementation not required for assignment part 1\n");
			break;
		}
		case (ALL) : {
			//TODO
			break;
		}
	}
	free(img -> data);
	free(img);
	free(out -> data);
	free(out);

	//save the output image file (from last executed mode)

	return 0;
}

void print_help(){
	printf("mosaic_%s C M -i input_file -o output_file [options]\n", USER_NAME);

	printf("where:\n");
	printf("\tC              Is the mosaic cell size which should be any positive\n"
		   "\t               power of 2 number \n");
	printf("\tM              Is the mode with a value of either CPU, OPENMP, CUDA or\n"
		   "\t               ALL. The mode specifies which version of the simulation\n"
		   "\t               code should execute. ALL should execute each mode in\n"
		   "\t               turn.\n");
	printf("\t-i input_file  Specifies an input image file\n");
	printf("\t-o output_file Specifies an output image file which will be used\n"
		   "\t               to write the mosaic image\n");
	printf("[options]:\n");
	printf("\t-f ppm_format  PPM image output format either PPM_BINARY (default) or \n"
		   "\t               PPM_PLAIN_TEXT\n ");
}

int process_command_line(int argc, char *argv[]){
	if (argc < 7){
		fprintf(stderr, "Error: Missing program arguments. Correct usage is...\n");
		print_help();
		return FAILURE;
	}
	
	//first argument is always the executable name
    
	//read in the non optional command line arguments
	c = (unsigned int)atoi(argv[1]);
	c = pow(2,c);
	
	//TODO: read in the mode
	execution_mode = atoi(argv[2]);
	//TODO: read in the input image name
	strcpy(input_file, argv[4]);
	printf("%s",input_file);
	//TODO: read in the output image name
	
	//TODO: read in any optional part 3 arguments
	return SUCCESS;
}

Image *readImage(char *filename){
	/*
	Reading file and save to Image variaable
	*/
	char str[20];
	Image *img;
	FILE *fp;
	int temp, rgb;
	
	fp = fopen(filename, "rb"); //Opening a file
	temp = getc(fp);
	while (temp == '#') {
		while (getc(fp) != '\n') ;
			temp = getc(fp);
    }
	ungetc(temp, fp); //Filtering comments
	fgets(str, sizeof(str), fp); //Obtaining magic number
    //check the image format
	if(str[1] == '6'){
		magic = 1; //P6
	}
	else 
	{
		magic = 0; //P3
	}
    temp = getc(fp);
    while (temp == '#') {
		while (getc(fp) != '\n') ;
			temp = getc(fp);
    } //Filtering comments
    ungetc(temp, fp);
	//Getting size of image
	img = (Image *)malloc(sizeof(Image)); //Initial Image
    fscanf(fp, "%d %d", &img->x, &img->y); //Getting size
	if(img->x < c || img->y < c){
		printf("Size error");
		exit(1);
	} //Checking size
	temp = getc(fp);
    while (temp == '#') {
		while (getc(fp) != '\n') ;
			temp = getc(fp);
    } 
    ungetc(temp, fp); //Filtering comments
    fscanf(fp, "%d", &rgb);
    if (rgb > 255) {
        printf("The program cannot support this file");
        exit(1);
    } //check rgb value
	img -> rgb = rgb;
    while (fgetc(fp) != '\n');
	if(magic == P6){
		img->data = (pixel*)malloc(img->x * img->y * sizeof(pixel)); //Initialising pixels in the variable
		if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
			printf("Read error");
			exit(1);
		}
	}
    else{
		img->data = (pixel*)malloc(img->x * img->y * sizeof(pixel)); //Initialising pixels in the variable
		int i;
		printf("something");
		for(i = 0; i < img->x*img->y;i++){
			fscanf(fp,"%hhd",&img->data[i].r);
			fscanf(fp,"%hhd",&img->data[i].g);
			fscanf(fp,"%hhd",&img->data[i].b);
		}
	}
    fclose(fp);
    return img;
}



Image* mosaic_CPU(Image* img,int c){
	/*
	Making mosaic using CPU
	*/
	Image *out; //initial output
	out = (Image *)malloc(sizeof(Image));
	
	
	int row,col,big_row,big_col,max_x = img->x,max_y = img->y,index;
	out->data = (pixel*)malloc(img->x * img->y * sizeof(pixel));
	out->x = max_x;
	out->y = max_y;
	out->rgb = img -> rgb;
	pixel avg; // For saving an average value
	for (big_row = 0; big_row < max_y;big_row += c){
		for (big_col = 0; big_col < max_x;big_col += c){
			avg = average_CPU(img,big_row,big_col,big_row+c,big_col+c);
			
			col = big_col;
			for(row = big_row;row < big_row+c;row++){
				for(col = big_col;col < big_col+c;col++){
					index = pixel_indexing(row,col,max_x);
					out -> data[index].r = avg.r;
					out -> data[index].g = avg.g;
					out -> data[index].b = avg.b;
				}
			}
		}
	}
	return out;
}

int pixel_indexing(int row,int col,int max_x){
	/*
	The function for helping indexing
	*/
	return row*(max_x)+col;
}

pixel average_OPENMP(Image *img, int start_row, int start_col,int end_row, int end_col){
	/*
	The function for calculate the average with CPU
	*/
	int row,col,r=0,g=0,b=0,count=0,index,max_x = img->x,red,green,blue;
	#pragma omp parallel for collapse(2) reduction(+:r,g,b)
	for(row = start_row; row < end_row; row++){
		for(col = start_col; col < end_col;col++){
			index = pixel_indexing(row,col,max_x);
			//Calling values
			#pragma omp section
			red = img -> data[index].r;
			#pragma omp section
			green = img -> data[index].g;
			#pragma omp section
			blue = img->data[index].b;
			//reduction part
			r += red;
			g += green;
			b += blue;
			//increasing count
			#pragma omp atomic
			count++;
		}
	}
	printf("\ncount = %d\n",count);
	pixel out;
	out.r = r/count;
	out.g = g/count;
	out.b = b/count;
	return out;
}

pixel average_CPU(Image *img, int start_row, int start_col,int end_row, int end_col){
	/*
	The function for calculate the average with CPU
	*/
	int row,col,r=0,g=0,b=0,count=0,index,max_x = img->x;
	for(row = start_row; row < end_row; row++){
		for(col = start_col; col < end_col;col++){
			index = pixel_indexing(row,col,max_x);
			r += img ->data[index].r;
			g += img ->data[index].g;
			b += img ->data[index].b;
			count++;
		}
	}
	//printf("\ncount = %d\n",count);
	pixel out;
	out.r = r/count;
	out.g = g/count;
	out.b = b/count;
	return out;
}


void write_P6(char *filename, Image *img)
{
	/*
	writting a P6 file
	*/
    FILE *fp;
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n"); //Magic number
    fprintf(fp, "%d %d\n",img->x,img->y); // write image sizes
    fprintf(fp, "%d\n",img -> rgb); //write rgb value
    fwrite(img->data, 3 * img->x, img->y, fp); //write pixels
    fclose(fp);
}


void write_P3(char *filename, Image *img)
{
	/*
	writting a P3 file
	*/
    FILE *fp;
    fp = fopen(filename, "wb");
    fprintf(fp, "P3\n"); //Magic number
    fprintf(fp, "%d %d\n",img->x,img->y); // write image sizes
    fprintf(fp, "%d\n",img -> rgb); //write rgb value
	int i;
	for(i=0;i < (img -> x) * (img -> y);i++){
		fprintf(fp, "%d %d %d ",img -> data[i].r,img -> data[i].g,img -> data[i].b);
		if(i % (img -> x) == 0){
			fprintf(fp,"\n");
		}
	}
    fclose(fp);
}