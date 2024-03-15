#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
	//Check for size 1, invalid, or null
	if (r-l<1||pData==NULL) {
		return;
	}
	if (r-l==1) {
		//If theres only 2, just compare and swap
		if (pData[l]>pData[r]) {
			int temp = pData[l];
			pData[l] = pData[r];
			pData[r] = temp;
		}
		return;
	}
	int mid = (l+r)/2;

	//Recursively call mergeSort
	mergeSort(pData, l, mid);
	//Don't need to recurse on size 1 half
	if (r-l>2) {
		mergeSort(pData, mid+1, r);
	}

	//Check if already sorted (largest element in first half less than smallest in second half)
	if (pData[mid]<pData[mid+1]){
		return;
	}

	//Merge
	int* tempArr = (int*)Alloc((1+r-l)*sizeof(int));
	//Current indicies in first and second half
	int c1 = l, c2 = mid+1;

	for (int i = 0; i<=r-l; i++) {
		if (c1>mid) {
			if (c2>r) {
				//This shouldn't happen, but if it does I don't want garbage data
				tempArr[i] = 0;
				printf("Couldn't find data to merge, added a zero.\n");
				continue;
			}
			tempArr[i] = pData[c2];
			c2++;
			continue;
		}
		if (c2>r) {
			tempArr[i] = pData[c1];
			c1++;
			continue;
		}

		if (pData[c1]<=pData[c2]) {
			tempArr[i] = pData[c1];
			c1++;
		} else {
			tempArr[i] = pData[c2];
			c2++;
		}
	}

	//Reuse c1 as current index when copying tempArr to pData
	c1 = l;
	for (int i = 0; i<=r-l; i++) {
		if (c1>r) {
			break;
		}
		pData[c1] = tempArr[i];
		c1++;
	}

	//Dealloc
	DeAlloc(tempArr);
}

// parses input file to an integer array
int parseData(char *inputFileName, int **ppData)
{
	FILE* inFile = fopen(inputFileName,"r");
	int dataSz = 0;
	int i, n, *data;
	*ppData = NULL;
	
	if (inFile)
	{
		fscanf(inFile,"%d\n",&dataSz);
		*ppData = (int *)malloc(sizeof(int) * dataSz);
		// Implement parse data block
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i=0;i<dataSz;++i)
		{
			fscanf(inFile, "%d ",&n);
			data = *ppData + i;
			*data = n;
		}

		fclose(inFile);
	}
	
	return dataSz;
}

// prints first and last 100 items in the data array
//Updated according to TA instructions
void printArray(int pData[], int dataSz)
{
	int i, sz = (dataSz > 100 ? dataSz - 100 : 0);
	int firstHundred = (dataSz < 100 ? dataSz : 100);
	printf("\tData:\n\t");
	for (i=0;i<firstHundred;++i)
	{
		printf("%d ",pData[i]);
	}
	printf("\n\t");
	if (dataSz > 100)
	for (i=sz;i<dataSz;++i)
	{
		printf("%d ",pData[i]);
	}
	printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
    double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };
	
	for (i=0;i<4;++i)
	{
		int *pDataSrc, *pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);
		
		if (dataSz <= 0)
			continue;
		
		pDataCopy = (int *)malloc(sizeof(int)*dataSz);
	
		printf("---------------------------\n");
		printf("Dataset Size : %d\n",dataSz);
		printf("---------------------------\n");
		
		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz*sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n",cpu_time_used);
		printf("\textra memory allocated\t: %d\n",extraMemoryAllocated);
		printArray(pDataCopy, dataSz);
		
		free(pDataCopy);
		free(pDataSrc);
	}
	
}