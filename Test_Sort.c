// gcc -o Test_Sort_demo Test_Sort.c & Test_Sort_demo.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char select_function[100];//function的代表號碼
int Chiawei_cnt = 0;

/******************************************************************************/

/*
 * Function Name        : swap
 * Function to perform the swap of two elements
 */
void swap(int *xp, int *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

/******************************************************************************/

/*
 * Function Name        : printArray
 * Function to print an array
 */
void printArray(int arr[], int size) {
	for (int i = 0; i < size; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

/******************************************************************************/

/*
 * Function Name        : merge
 * Merge two subarrays of arr[]
 * First subarray is arr[l..m]
 * Second subarray is arr[m+1..r]
 */
void merge(int arr[], int l, int m, int r) {
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;

	// Create temporary arrays
	int L[n1], R[n2];

	// Copy data to temporary arrays L[] and R[]
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	// Merge the temporary arrays back into arr[l..r]
	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2) {
		if (L[i] <= R[j]) {
			arr[k] = L[i];
			i++;
		} else {
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	// Copy the remaining elements of L[], if there are any
	while (i < n1) {
		arr[k] = L[i];
		i++;
		k++;
	}

	// Copy the remaining elements of R[], if there are any
	while (j < n2) {
		arr[k] = R[j];
		j++;
		k++;
	}

}

/******************************************************************************/

/*
 * Function Name        : partition
 * Function to partition the array and return the pivot index
 */
int partition(int arr[], int low, int high) {
	int pivot = arr[high]; // Pivot
	int i = (low - 1);     // Index of smaller element

	for (int j = low; j <= high - 1; j++) {
		// If the current element is smaller than or equal to the pivot
		if (arr[j] <= pivot) {
			i++; // Increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
		printf("Chiawei_cnt = %d_for, arr = ", Chiawei_cnt);
		printArray(arr, 7);
	}
	swap(&arr[i + 1], &arr[high]);
	printf("Chiawei_cnt = %d____, arr = ", Chiawei_cnt);
	printArray(arr, 7);
	return (i + 1);
}

/******************************************************************************/

/*
 * Function Name        : heapify
 * 對以root為根的子樹進行堆積的調整
 */
void heapify(int arr[], int n, int root) {
	int largest = root; // 初始化根為最大值
	int left = 2 * root + 1;
	int right = 2 * root + 2;

	// 如果左子節點大於根
	if (left < n && arr[left] > arr[largest])
		largest = left;

	// 如果右子節點大於根
	if (right < n && arr[right] > arr[largest])
		largest = right;

	// 如果最大值不是根，交換根和最大值
	if (largest != root) {
		swap(&arr[root], &arr[largest]);

		// 遞迴調整被交換的子樹
		heapify(arr, n, largest);
	}
}

/******************************************************************************/

/*
 * Function Name        : bubbleSort
 * Function to perform Bubble Sort
 */
void bubbleSort(int arr[], int n) {
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(&arr[j], &arr[j + 1]);
			}
			printArray(arr, 7);

		}
		printArray(arr, 7);
		printf("\n");
	}
}

/****************************************************************************

/*
 * Function Name        : insertionSort
 * Function to perform Insertion Sort
 */
void insertionSort(int arr[], int n) {
	int i, key, j;
	for (i = 1; i < n; i++) {
		key = arr[i];
		j = i - 1;

		// Move elements of arr[0..i-1] that are greater than key to one position ahead of their current position
		while (j >= 0 && arr[j] > key) {
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
		printArray(arr, 7);
	}
}

/******************************************************************************/

/*
 * Function Name        : selectionSort
 * Function to perform Selection Sort
 */
void selectionSort(int arr[], int n) {
	int i, j, min_idx;

	// One by one move the boundary of the unsorted subarray
	for (i = 0; i < n - 1; i++) {
		// Find the minimum element in the unsorted array
		min_idx = i;
		for (j = i + 1; j < n; j++) {
			if (arr[j] < arr[min_idx]) {
				min_idx = j;
			}
		}

		// Swap the found minimum element with the first element
		int temp = arr[min_idx];
		arr[min_idx] = arr[i];
		arr[i] = temp;
		printArray(arr, 7);
	}
}

/******************************************************************************/

/*
 * Function Name        : mergeSort
 * l is for left index and r is right index of the sub-array of arr to be sorted
 */
void mergeSort(int arr[], int l, int r) {
	if (l < r) {
		// Same as (l+r)/2, but avoids overflow for large l and r
		int m = l + (r - l) / 2;

		// Sort first and second halves
		mergeSort(arr, l, m);
		mergeSort(arr, m + 1, r);

		// Merge the sorted halves
		merge(arr, l, m, r);
		
		printf("merge(Arr, %d, %d, %d)  : ", l, m, r);
		printArray(arr, 7);
	}
}

/******************************************************************************/

/*
 * Function Name        : quickSort
 * Function to implement QuickSort
 */
void quickSort(int arr[], int low, int high) {
	if (low < high) {
		// Find pivot element such that elements smaller than pivot are on the left,
		// and elements greater than pivot are on the right
		int pi = partition(arr, low, high);
		Chiawei_cnt++;
		// Recursively sort the subarrays
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

/******************************************************************************/

/*
 * Function Name        : heapSort
 * 堆積排序
 */
void heapSort(int arr[], int n) {
	// 建構最大推
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);

	printf("Chiawei_cnt = %d____, arr = ", Chiawei_cnt);
	printArray(arr, 7);
	Chiawei_cnt++;

	// 從堆中提去元素，並逐步縮小堆
	for (int i = n - 1; i > 0; i--) { 
		// 將堆頂元素 (最大值) 與前堆的最後一個元素交換
		swap(&arr[0], &arr[i]);

		// 調整堆，使其重新成為最大堆
		heapify(arr, i, 0);

		printf("Chiawei_cnt = %d____, arr = ", Chiawei_cnt);
		printArray(arr, 7);
		Chiawei_cnt++;
	}
}

/******************************************************************************/

int main() {
	while(1){
		printf("\n//------------------------------------------------//\n");
		printf("//function List:\n");
		printf("//[O(n²)]------------------------------/\n");		
		printf("//( 1 ) bubbleSort\n");
		printf("//( 2 ) insertionSort\n");
		printf("//( 3 ) selectionSort\n");
		printf("//[O(nlogn)]------------------------------/\n");
		printf("//( 4 ) mergeSort\n");
		printf("//( 5 ) quickSort\n");
		printf("//( 6 ) heapSort\n");
		printf("//------------------------------------------------//\n\n");
		printf("[Test_Sort.exe]: select function num:");//提示請輸入function代碼
		scanf("%s",&select_function);//接收輸入的值存入select_function

		if(strncmp("1", select_function, 1) == 0){//function bubbleSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			bubbleSort(arr, n);

			printf("Sorted array: \n");
			printArray(arr, n);

		}else if(strncmp("2", select_function, 1) == 0){//function insertionSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			insertionSort(arr, n);

			printf("Sorted array: \n");
			printArray(arr, n);

		}else if(strncmp("3", select_function, 1) == 0){//function selectionSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			selectionSort(arr, n);

			printf("Sorted array: \n");
			printArray(arr, n);
					
		}else if(strncmp("4", select_function, 1) == 0){//function mergeSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			mergeSort(arr, 0, n - 1);

			printf("Sorted array: \n");
			printArray(arr, n);
							
		}else if(strncmp("5", select_function, 1) == 0){//function quickSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			quickSort(arr, 0, n - 1);

			printf("Sorted array: \n");
			printArray(arr, n);
			
		}else if(strncmp("6", select_function, 1) == 0){//function heapSort========================================================================================
			Chiawei_cnt = 0;
			// int arr[] = {64, 34, 25, 12, 22, 11, 90};
			int arr[] = {1,5,0,6,2,3,4};

			int n = sizeof(arr) / sizeof(arr[0]);

			printf("Unsorted array: \n");
			printArray(arr, n);

			heapSort(arr, n);

			printf("Sorted array: \n");
			printArray(arr, n);
			
		}else if(strncmp("test", select_function, 4) == 0){//test========================================================================================
		
		}else if(strncmp("exit", select_function, 4) == 0){//exit========================================================================================
			break;
		}else{
			printf("select function not found!\n\n");
			system("pause");//等待使用者
			printf("\n");
		}
	}

	return 0;
}





// int main() {

// 	Chiawei_cnt = 0;
// 	// int arr[] = {64, 34, 25, 12, 22, 11, 90};
// 	int arr[] = {1,5,0,6,2,3,4};

// 	int n = sizeof(arr) / sizeof(arr[0]);

// 	printf("Unsorted array: \n");
// 	printArray(arr, n);
	
// 	// [O(n²)]------------------------------/
// 	// bubbleSort(arr, n);
// 	// insertionSort(arr, n);
// 	// selectionSort(arr, n);

// 	// [O(nlogn)]------------------------------/
// 	// mergeSort(arr, 0, n - 1);
// 	// quickSort(arr, 0, n - 1);
// 	// heapSort(arr, n);

// 	printf("Sorted array: \n");
// 	printArray(arr, n);

// 	return 0;
// }