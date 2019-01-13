#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>


int fileExists(char*fn);
int*filescan(char*fn,int*j);
int getLength(int*arr);
void sortify(int*myarr);
int main(int argc, char *argv[]);
void historize(int*myarr, int*min, int*max, int*bin, int*N, int*w);
int newarr(int arr1[], int arr2[], int size1, int size2);
void fileWrite(int*arr,int*size);
void*threadworker(int newsize, int j, double*minvalue, double*maxvalue, double*w, int*size, int*size1, int*size2, char*argv[]);

int*histarr;
int*majorarr;	show size of an array

//shared data struct
struct thread_data_t{
    pid_t tid;
    double sharedthread;
};
	
struct thread_data_t *thr_data;
double shared_x;
pthread_mutex_t lock_x;

int main(int argc, char *argv[]){
	
	

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
	
	thr_data = malloc( N * sizeof(struct thread_data_t));
	
	//argument array

		
	char* outfile = argv[N-1];
	//check input validity
	if(minvalue < 0 || maxvalue < minvalue || bincount <= 0 || N < 1) { 
		printf("either invalid entry or missing input\n"); 
		exit(0); 
		return 0; 
	}
	else{
		for(int c = 5; c < N; c++){
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
		printf("minimum val,ue should not be bigger than maximum value\n"); 
		exit(0); 
		return 0; 
	}
	int size1 = 0, size2 = 0, size3 = 0;
	int size = 0,oldsize = 0;
	int newsize = 0;
	int majorsizecounter=0;
	//total size
	for(int i=5; i<argc;i++){ filescan(argv[i],&size); newsize += size; }
	int dump[newsize];
	
	/////////  MULTITHREAD  /////////////
	
	//thread id and counters
	pthread_t tid[argc];
	int j, rc;
	
	
	
	//shared data
	shared_x = 0;
	
	//protected mutex
	pthread_mutex_init(&lock_x, NULL);
	
	//histogram shared multithreading
	for(j = 0; j<argc ;++j)	{
		thr_data[j].tid = j;
    thr_data[j].sharedthread = (j + 1) * N;
    if ((rc = pthread_create(&tid[j], NULL, threadworker(0,j,&minvalue,&maxvalue,&w,&size,&size1,&size2,argv), &thr_data[j]))) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      return 0;
    }
		pthread_create(&tid[j], NULL, threadworker(0,j,&minvalue,&maxvalue,&w,&size,&size1,&size2,argv), (void*)&tid);
		
	}
	
	//combine histograms
	for(j = 0; j<argc;j++){
		pthread_join(tid[j], NULL);
		putchar('\n');
	}
	
	//jobe is done with multithread
	pthread_exit(NULL);

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
int*filescan(char*fn,int*j){
	FILE* file = fopen (fn, "r");
	int i = 0;
	*j = 0;
	int*arr = malloc(sizeof(int));
	fscanf (file, "%d", &i);    
	while (!feof (file))		{
		arr = realloc(arr,sizeof(arr)*100);
		//printf ("%d ", i);
		arr[*j] = i;
		//printf ("%d ", arr[j]);
		
		fscanf (file, "%d", &i); 
		(*j)++;
	}
	
	fclose (file);
	return arr;
}
//turn contents stored in an array into a histogram
void historize(int*myarr, int*min, int*max, int*bin, int*N, int*w){
	//initial variables for counters
	int index, outdex, yandex, wmin = 0, wmax = *w;
	//size of input array
	int arrsize = getLength(myarr);
	//a sorted temporary array within interval
	int*dump = malloc(sizeof(int));
	for(int count = 0; myarr[count]>=*min && myarr[count]<=*max; count++){
		if(count == getLength(dump))
			dump = realloc(dump, sizeof(dump) + sizeof(int));
		dump[count] = myarr[count];
	}	
	for(int c=0;c<*N;c++)		
		//printf("%i ",dump[c]);
	
	myarr = dump;
	arrsize = getLength(myarr);
			
	//get contents of input array
	for(index=0;index<arrsize;++index){
		
		if(myarr[index]>=wmin && myarr[index]<*w) {
			//printf(" *");
		}
		else{
			//printf("\n *");
			wmin = wmin + *w;			
			(*w)++;
			
		}
	}		
	
	printf("\n");
}

//merge arrays
void merge(int arr1[], int arr2[], int size1, int size2){
	memcpy(arr1 + size1, arr2, size2);

}

//write to a file
void fileWrite(int*arr,int*size){
	//variables
	FILE* file;
	int num;
	static int i = 0;
	char fn[16];
	
	//file name with numerical order
	//sprintf(fn,"a%d.out", i++);
	
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
int getLength(int*arr){
	int count = 0;
	while(arr[count]!='\0'){ (count)++; }
	
	return count; // exclude tail flag operator 'null' (last element->next = null)


}
//sort arrays
void sortify(int*myarr){
	if(getLength(myarr)<=0) return;
	//variables
	int loopext, loopint, dump;
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

void*threadworker(int newsize, int j, double*minvalue, double*maxvalue, double*w, int*size, int*size1, int*size2, char*argv[]){
	
		//skip non-file ones
		if(j<=4) return NULL;				
		
		//variables
		int minw = *minvalue;
		int maxw = minw + *w;

		*size1 = *size;
		majorarr = filescan(argv[j],size);
		sortify(majorarr);
		*size2 = *size;
		
		//lock and unlock process for shared data
		struct thread_data_t *data = (struct thread_data_t*)majorarr;
		
		pthread_mutex_lock(&lock_x);
		shared_x += data->sharedthread;
		pthread_mutex_unlock(&lock_x);
	
		
		
		//loop
		int i = 0;
		int counter = 0;
		char*line[*size2];
		//printf("[%i]* ",i);
		//bin interval 
		for( ; i< getLength(majorarr); i++) {
			double item = (double)majorarr[i];
			if(!(item>=*minvalue && item<=*maxvalue)) break; // invalid entry

			//bin condition
			if(item<minw) continue;
			else if(item>=minw && item<maxw){ 
				printf("* ");
				line[i] = "* ";
				//printf("item: %i ",majorarr[i]);
			}
			else if(item>=maxw){
				putchar('\n');
				//printf("[%i]* ",++counter);
				line[i] = "\n* ";
				//printf("item: %i ",majorarr[i]);			
				minw = minw + *w;
				maxw = maxw + *w;
			}
			
		}
		//write output to file
		fileWrite(majorarr,size2);
		putchar('\n');
		
		pthread_exit(NULL);
}

