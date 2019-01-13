#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>

bool TestAndSet(bool*target){
	bool rv = *target;
	*target = true;
	return rv;

}
int fileExists(char*fn);
double*filescan(char*fn,int*j);
int getLength(double*arr);
void sortify(double*myarr);
int main(int argc, char *argv[]);
void historize(double*myarr, int*min, int*max, int*bin, int*N, int*w);
int newarr(int arr1[], int arr2[], int size1, int size2);
void fileWrite(double*arr,int*size);

double*histarr;
double*majorarr;

int main(int argc, char *argv[]){
	int process1, process2;
	bool key;
	bool lock; bool waiting[2];
	
		//ENTRY SECTION//
		waiting[process1] = false;
		key = false;
		while(waiting[process1]&&key)
			key = TestAndSet(&lock);
		
		waiting[process1] = true;
		
		//CRITICAL SECTION//
		
		//check argument count validity
		if(argc < 6) { 
			printf("either invalid entry or missing input\n"); 
			exit(0); 
			return 0; 
		}	
		//process id
		pid_t n; 
		//arguments
		double minvalue = atoi(argv[1]);
		double maxvalue = atoi(argv[2]);
		double bincount = atoi(argv[3]);
		int N = atoi(argv[4]);
		char* outfile = argv[N-1];
		//check input validity
		if(minvalue < 0 || maxvalue < minvalue || bincount <= 0 || N < 1) { 
			printf("either invalid entry or missing input\n"); 
			exit(0); 
			return 0; 
		}
		else{
			for(int c = 5; c < N; c++){
				//input is not available
				if(fileExists(argv[c]) == 0) { 
					printf("either invalid entry or missing input\n"); 
					exit(0); 
					return 0; 
				}
			}
		}
		//variables	
		double w = (maxvalue - minvalue)/bincount;	
		int binnumber = 1;
		//request file name input
		char* filename = argv[0];	
		//eliminate "./" from file name
		char* name = &filename[2];
		int myswitch = 0;
		//CODE SEGMENT
	
		if(minvalue > maxvalue) { 
			printf("minimum value should not be bigger than maximum value\n"); 
			exit(0); 
			return 0; 
		}
		int size1 = 0, size2 = 0, size3 = 0;
		int size = 0,oldsize = 0;
		int newsize = 0;
		int majorsizecounter=0;
		//total size
		for(int i=5; i<argc;i++){
			if(strstr(argv[i],".out")==NULL){
				filescan(argv[i],&size); 
				newsize += size; 
			}
		}
		double dump[newsize];
	
		///////// SHARED MULTI PROCESS  /////////////
	
		//child process
	
			newsize = 0;
			//search files
			for(int j = 0; j<argc ;j++)	{
				//skip non-file ones
				if(j<=4 || (strstr(argv[argc-1],".out"))!=NULL) continue;		
				//variables
				int minw = minvalue;
				int maxw = minw + w;
	
				size1 = size;
				majorarr = filescan(argv[j],&size);
				sortify(majorarr);
				size2 = size;
			
				//loop
				int i = 0;
				int counter = 0;
				char*line[size2];
				printf("[%i]* ",i);
				//bin interval 
				for( ; i< getLength(majorarr); i++) {
					double item = (double)majorarr[i];
					if(!(item>=minvalue && item<=maxvalue)) break; // invalid entry
	
					//bin condition
					if(item<minw) continue;
					else if(item>=minw && item<maxw){ 
						//printf("* ");
						line[i] = "* ";
					
					}
					else if(item>=maxw){
						putchar('\n');
						//printf("[%i]* ",++counter);
						line[i] = "\n* ";
			
						minw = minw + w;
						maxw = maxw + w;
	
	
					}
				
				}
				//write output to file
			//	fileWrite(majorarr,&size2);
				putchar('\n');
			}
		
		//parent process
	
			
			//combine histograms
			for(int i=0; i<argc;i++){
			
				if(i<=4 || (strstr(argv[argc-1],".out"))!=NULL) continue;
				double*input = filescan(argv[i],&size);
				putchar('\n');
				//printf("file%d: ",i-4);
				//combinational operation
				for(int j = 0; j<size; j++){			
					dump[majorsizecounter] = input[j];
				//	printf("%i ", (int)dump[majorsizecounter]);
					majorsizecounter++;		
			
				}
			}
			putchar('\n');
			
			
		//EXIT SECTION//
		
		process2 = (process1 + 1)%(atoi(argv[4]));
		while((process2!=process1)&&!waiting[process2])
			process2 = (process2 + 1)%(atoi(argv[4]));
		if(process2==process1)
			lock = false;
		else
			waiting[process2] = false;
			
		//REMAINDER SECTION
		
			

	return 0;
}

//find file
int fileExists(char*fn){
	FILE * file;
	file = fopen(fn, "r");
	//file exits
	if (file){ 
	  fclose(file);
		return 1;
	}
	//file not found or not allowed
	return 0;
}
//check whether file exits
double*filescan(char*fn,int*j){
	
	FILE* file = fopen (fn, "r");
	int i = 0;
	*j = 0;
	double*arr = NULL;
	fscanf (file, "%d", &i);    
	while (!feof (file))		{
		arr = realloc(arr,sizeof(double)*(*j + 1));
		
		arr[*j] = i;
		
		fscanf (file, "%d", &i); 
		(*j)++;
	}
	
	fclose (file);
	return arr;
}
//turn contents stored in an array into a histogram
void historize(double*myarr, int*min, int*max, int*bin, int*N, int*w){
	//initial variables for counters
	int index, outdex, yandex, wmin = 0, wmax = *w;
	//size of input array
	int arrsize = getLength(myarr);
	//a sorted temporary array within interval
	double*dump = NULL;
	for(int count = 0; myarr[count]>=*min && myarr[count]<=*max; count++){
		if(count == getLength(dump))
			dump = realloc(dump, sizeof(dump) + sizeof(double)*100);
		dump[count] = myarr[count];
	}	
	for(int c=0;c<*N;c++)		
		//printf("%i ",dump[c]);
	
	myarr = dump;
	arrsize = getLength(myarr);
			
	//get contents of input array
	for(index=0;index<arrsize;++index){
		
		if(myarr[index]>=wmin && myarr[index]<*w) ;
		else{
			
			wmin = wmin + *w;			
			(*w)++;
			
		}
	}		
	
	printf("\n");
}

//merge arrays
void merge(double arr1[], double arr2[], int size1, int size2){
	memcpy(arr1 + size1, arr2, size2);

}

//write to a file
void fileWrite(double*arr,int*size){
	//variables
	FILE* file;
	int num;
	static int i = 0;
	char fn[16];
	
	//file name with numerical order
	sprintf(fn,"a%d.out", i++);
	
	//create the file
	file = fopen (fn, "w");
	//failed to create the file
	if(file == NULL) {
		printf("OS failed to create new file");
		exit(-1);
	}
	int numcount = 0;
	while(1){
		//write numbers received from the array
		if(num<0 & numcount<*size) break;
		putw(arr[numcount++],file);
	
	
	}
	fclose (file);
}

//get length of an array
int getLength(double*arr){
	int count = 0;
	while(arr[count]!='\0'){ (count)++; }
	
	return count; // exclude tail flag operator 'null' (last element->next = null)


}
//sort arrays
void sortify(double*myarr){
	if(getLength(myarr)<=0) return;
	//variables
	int loopext, loopint;
	double dump;
	int range = getLength(myarr);
	//external loop
	for(loopext = 0; loopext < range; loopext++){
		//internal loop
		for(loopint = loopext + 1; loopint < range; loopint++){
			//check ascension
			if(myarr[loopext] > myarr[loopint]){
				//switch groups
				dump = myarr[loopext];
				myarr[loopext] = myarr[loopint];
				myarr[loopint] = dump;
			}
		}
	}
}
