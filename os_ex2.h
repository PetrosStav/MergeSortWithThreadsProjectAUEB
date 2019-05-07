// Include everything needed for the program

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

// sort_data
// Used in order to store the thread's data so
// that they can be passed as arguments in the
// sorting functions
typedef struct sort_data{
    int thid;
    int* arr;
    int start;
    int end;
} SORT_DATA;

// isNumber
// Gets a char* s as input and checks if every character is an number
// including negatives (with '-' symbol at front)
// Returns values:
// 0 - s is not a number
// 1 - s is a number 
int isNumber(char* s);

// selectionSortMode1
// Sorts the array using selection sort with mode 1, which means that
// only one thread can access or write into the array at a given time 
void* selectionSortMode1(void* args);

// selectionSortMode2
// Sorts the array using selection sort with mode 2, which means that
// only one thread can write into the array at a given time, but all
// can access the array at the same time
void* selectionSortMode2(void* args);

// selectionSortMode3
// Sorts the array using selection sort with mode 3, which means that
// the threads can access and write into the array all the same time
// simultaneously
void* selectionSortMode3(void* args);

// merge
// Merges two subarrays/subspaces of the array (because we use the same array there aren't
// two arrays to merge, but the same array in two different spaces
void merge(int* arr,int left, int mid, int right);
