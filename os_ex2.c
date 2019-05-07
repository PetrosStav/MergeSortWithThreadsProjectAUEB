// Include the header file that contains all the other inlcudes as well as the
// declarations of all the functions used

#include "os_ex2.h"

// Everything needed to print the messages in order with mutex locks and conditions

// printThreadId : the current thread id to be printed - used for synchronization
// printCount : how many threads are left to be printed, used for thread started messages
int printThreadId, printCount;
// Global variables for the mutexes

// printMutex : mutex used for synchronizing threads to print sorted messages in descending order
// modeMutex : mutex used for synchronizing threads for the access/write on the array according to mode
// printLockMutex : mutex used for synchronizing threads to wait until all thread started messages are printed before startig sorting
pthread_mutex_t printMutex, modeMutex, printLockMutex;
// Global variables for the conditions

// printThreadCondition : condition used for synchronizing threads to print sorted messages in descending order
// printLockCondition : condition used for synchronizing threads to wait until all thread started messages are printed before startig sorting
pthread_cond_t printThreadCondition, printLockCondition;

// Main function of mergesort

int main(int argc, char * argv[]) {

    // Declare all needed variables

    // fp : file pointer needed to write to file results.dat
    // data_array : a struct pointer array used to pass the arguments to each thread's execution of the sorting algorithm
    // timeval start, end : used to calculate the running time of the mergesort
    // token : used in the start to check the validity of the arguments
    // numarray : the number array that will be sorted
    // argcheck : contains 0/1 if the specific argument has already been used with indexes : {0: numbers,1: threads, 2: seed, 3:mode}
    // nums : the amount of number of the user's input
    // threadNum : the number of threads of the user's input 
    // seed : the seed used for rand() function
    // mode : the access mode of the threads to the number array (1 - absolute sync , 2 - loose sync , 3 - no sync)
    // i : used as an index
    // retVal : used for the return value of pthread_create
    // rc : used for the return value of mutexes and conditions for error checking

    FILE * fp;
    SORT_DATA * data_array;
    pthread_t * threads;
    struct timeval start, end;
    char * token;
    int * numarray;
    int argcheck[4];
    int nums, threadNum, seed, mode;
    int i, retVal, rc;

    // Get start time
    gettimeofday( & start, NULL);

    // Make sure that 4 arguments (+1 the name of the program) were given from the user
    if (argc != 5) {
        // If not print error message
        fprintf(stderr, "Invalid input!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
        // Exit the program
        return -1;
    }

    // Set all argument checks to 0 - no argument has been used
    for (i = 0; i < 4; i++) {
        argcheck[i] = 0;
    }

    // Check each argument for correct input
    for (i = 1; i < argc; i++) {

        // Get the flag before the equal sign
        token = strtok(argv[i], "=");

        // Check what flag it is and assign to corresponding variable

        if (strcmp(token, "-numbers") == 0) {

            // Check if it has already been used
            if (argcheck[0] == 1) {
                // If it has print error
                fprintf(stderr, "Invalid input at flag: '-numbers' : Flag has been used twice!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

            // If it has not then set argcheck to 1
            argcheck[0] = 1;

            // Get what is after '=' symbol
            token = strtok(NULL, "=");
            // If it is a number then safely convert it with atoi
            if (isNumber(token) == 1) nums = atoi(token);
            else {
                // If it is not then print error
                fprintf(stderr, "Invalid input at flag: '-numbers'!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

        } else if (strcmp(token, "-threads") == 0) {

            // Check if it has already been used
            if (argcheck[1] == 1) {
                // If it has print error
                fprintf(stderr, "Invalid input at flag: '-threads' : Flag has been used twice!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

            // If it has not then set argcheck to 1
            argcheck[1] = 1;

            // Get what is after '=' symbol
            token = strtok(NULL, "=");
            // If it is a number then
            if (isNumber(token) == 1) {
                // Safely convert it with atoi
                threadNum = atoi(token);
                // Set printThreadId to the number of the threads (as it must print them in descending order)
                printThreadId = threadNum;
                // Set printCount to the number of the threads
                printCount = threadNum;
            } else {
                // If it is not then print error
                fprintf(stderr, "Invalid input at flag: '-threads'!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

        } else if (strcmp(token, "-seed") == 0) {

            // Check if it has already been used
            if (argcheck[2] == 1) {
                // If it has print error
                fprintf(stderr, "Invalid input at flag: '-seed' : Flag has been used twice!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

            // If it has not then set argcheck to 1
            argcheck[2] = 1;

            // Get what is after '=' symbol
            token = strtok(NULL, "=");
            // If it is a number then safely convert it with atoi
            if (isNumber(token) == 1) seed = atoi(token);
            else {
                // If it is not then print error
                fprintf(stderr, "Invalid input at flag: '-seed'!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

        } else if (strcmp(token, "-mode") == 0) {

            // Check if it has already been used
            if (argcheck[3] == 1) {
                // If it has print error
                fprintf(stderr, "Invalid input at flag: '-mode' : Flag has been used twice!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

            // If it has not then set argcheck to 1
            argcheck[3] = 1;

            // Get what is after '=' symbol
            token = strtok(NULL, "=");
            // If it is a number then safely convert it with atoi
            if (isNumber(token) == 1) mode = atoi(token);
            else {
                // If it is not then print error
                fprintf(stderr, "Invalid input at flag: '-mode'!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
                // Exit program
                return -1;
            }

        } else {

            // The flags are invalid
            // Print error message
            fprintf(stderr, "Invalid flags!\nUsage:\n./a.out -numbers=X -threads=Y -seed=Z -mode=I\n");
            // Exit the program
            return -1;

        }
    }
    // Check extra stuff about the arguments

    // If the user didnt give a positive amount of numbers to sort
    if (nums <= 0) {
        // Print error message
        fprintf(stderr, "Invalid input!\nNegative or zero numbers are not accepted.\n");
        // Exit the program
        return -1;
    }
    // If the thread number is not {1,2,4,8}
    if (threadNum != 1 && threadNum != 2 && threadNum != 4 && threadNum != 8) {
        // Print error message
        fprintf(stderr, "Invalid input!\nValid input for threads: 1,2,4,8.\n");
        // Exit the program
        return -1;
    }
    // If the seed is negative
    if (seed < 0) {
        // Print error message
        fprintf(stderr, "Invalid input!\nNegative number for seed is not accepted.\n");
        // Exit the program
        return -1;
    }
    // If mode is not {1,2,3}
    if (mode != 1 && mode != 2 && mode != 3) {
        // Print error message
        fprintf(stderr, "Invalid input!\nValid input for mode: 1,2,3.\n");
        // Exit the program
        return -1;
    }

    // ThreadNum must be less than nums so that the array can be split properly
    if (nums < threadNum) {
        // If it is not print error
        fprintf(stderr, "Invalid input!\nThreadNum must be less or equal than the numbers so that the array can be split properly.\n");
        // Exit program
        return -1;
    }

    // Print diagnostic messages
    printf("Mergesort started with %d threads and mode:%d\nSetting seed to %d\n", threadNum, mode, seed);

    // Set seed to rand() with srand();

    srand(seed);

    // Allocate the array for numbers

    numarray = (int * ) malloc(nums * sizeof(int));

    // If the allocation failed
    if (numarray == NULL) {
        // Print error
        fprintf(stderr, "Not enough memory!\n");
        // Exit program
        return -1;
    }

    // Set random numbers to array
    printf("Randomizing %d numbers\n", nums);
    for (i = 0; i < nums; i++) {
        // Set random numbers from 0 to nums*10 to numarray
        numarray[i] = rand() % (nums * 10);
    }

    // Open the file
    fp = fopen("./results.dat", "w+");
    // Write the initial array
    fprintf(fp, "Initial array={");
    for (i = 0; i < nums - 1; i++) {
        fprintf(fp, "%d, ", numarray[i]);
    }
    fprintf(fp, "%d}\n", numarray[nums - 1]);
    // Inform the user that the file was written
    printf("Written initial array to results.dat\n");

    // Allocate the data_array array for the threads' bounds

    data_array = (SORT_DATA * ) malloc(threadNum * sizeof(SORT_DATA));

    // If the allocation failed
    if (data_array == NULL) {
        // Print error
        fprintf(stderr, "Not enough memory!\n");
        // Free memory allocated for numarray
        free(numarray);
        // Close the open file
        fclose(fp);
        // Exit program
        return -1;
    }

    // Set the data for each thread
    // The array contains data for the sorting

    for (i = 0; i < threadNum - 1; i++) {
        // Set the thread id
        data_array[i].thid = i + 1;
        // Set the array to sort
        data_array[i].arr = numarray;
        // Set the starting point of the array to sort
        data_array[i].start = i * (nums / threadNum);
        // Set the ending point of the array to sort
        data_array[i].end = (i + 1) * (nums / threadNum) - 1;
    }

    // For the last thread put everything that remains (last thread because of imperfect division has usually more numbers to sort)
    // Set the thread id
    data_array[threadNum - 1].thid = threadNum;
    // Set the array to sort
    data_array[threadNum - 1].arr = numarray;
    // Set the starting point of the array to sort
    data_array[threadNum - 1].start = (threadNum - 1) * (nums / threadNum);
    // Set the ending point of the array to sort (the end of the array)
    data_array[threadNum - 1].end = nums - 1;

    // Allocate the thread array

    threads = (pthread_t * ) malloc(threadNum * sizeof(pthread_t));

    // If the allocation failed
    if (threads == NULL) {
        // Print error
        fprintf(stderr, "Not enough memory!\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    // Initialize the mutex locks and conditions
    rc = pthread_mutex_init( & printMutex, NULL);
    // If mutex init failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_init()\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    rc = pthread_mutex_init( & printLockMutex, NULL);
    // If mutex init failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_init()\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    rc = pthread_cond_init( & printThreadCondition, NULL);
    // If mutex init failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_init()\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    rc = pthread_cond_init( & printLockCondition, NULL);
    // If mutex init failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_init()\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    rc = pthread_mutex_init( & modeMutex, NULL);
    // If mutex init failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_init()\n");
        // Free the memory allocated for data_array and numarray
        free(data_array);
        free(numarray);
        // Close open file
        fclose(fp);
        // Exit program
        return -1;
    }

    // Create the threads

    for (i = 0; i < threadNum; i++) {
        printf("Creating thread %d\n", i + 1);
        // Depending on the mode specified use the according selectionSort function
        // passing the thread's data_array as arguments to the function
        if (mode == 1) {
            retVal = pthread_create( & threads[i], NULL, selectionSortMode1, & data_array[i]);
        } else if (mode == 2) {
            retVal = pthread_create( & threads[i], NULL, selectionSortMode2, & data_array[i]);
        } else {
            retVal = pthread_create( & threads[i], NULL, selectionSortMode3, & data_array[i]);
        }
        // If pthread_create failed
        if (retVal != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_create()!\n");
            // Free the memory allocated to data_array, numarray and threads
            free(data_array);
            free(numarray);
            free(threads);
            // Close the open file
            fclose(fp);
            // Destroy the mutex locks and conditions
            rc = pthread_mutex_destroy( & printMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_cond_destroy( & printThreadCondition);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_cond_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_mutex_destroy( & printLockMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_cond_destroy( & printLockCondition);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_cond_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_mutex_destroy( & modeMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            // Exit program
            return -1;
        }

    }

    // Join the threads

    for (i = 0; i < threadNum; i++) {
        retVal = pthread_join(threads[i], NULL);
        // If thread join failed
        if (retVal != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_join()!\n");
            // Free the memory allocated to data_array, numarray, threads
            free(data_array);
            free(numarray);
            free(threads);
            // Close the open file
            fclose(fp);
            // Destroy the mutex locks and conditions
            rc = pthread_mutex_destroy( & printMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_cond_destroy( & printThreadCondition);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_cond_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_mutex_destroy( & printLockMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_cond_destroy( & printLockCondition);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_cond_destroy()\n");
                // Exit program
                return -1;
            }
            rc = pthread_mutex_destroy( & modeMutex);
            // If mutex destroy failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_destroy()\n");
                // Exit program
                return -1;
            }
            // Exit program
            return -1;
        }
    }

    // Merge the arrays from the threads
    // Merging will be done in parts
    // Firstly if we have more than 1 thread we merge the arrays by each 2 (2's)
    // Then if we have more than 2 threads we merge again by each 2 (4's)
    // Lastly if we have 4 threads we merge again by each 2 (8's)

    // Merge the 2's -- if threadNum == 1 will bypass
    if (threadNum != 1) {
        for (i = 0; i < threadNum; i += 2) {
            merge(numarray, data_array[i].start, data_array[i].end, data_array[i + 1].end);
        }
    }

    // Merge the 4's -- if threadNum == 1 or 2 will bypass
    if (threadNum != 1 && threadNum != 2) {
        for (i = 0; i < threadNum; i += 4) {
            merge(numarray, data_array[i].start, data_array[i + 1].end, data_array[i + 3].end);
        }
    }

    // Merge the 8's -- if threadNum == 1 or 2 or 4 will bypass
    if (threadNum == 8) {
        merge(numarray, data_array[0].start, data_array[3].end, data_array[7].end);
    }

    // Write sorted array to file
    fprintf(fp, "Sorted array={");
    for (i = 0; i < nums - 1; i++) {
        fprintf(fp, "%d, ", numarray[i]);
    }
    fprintf(fp, "%d}\n", numarray[nums - 1]);
    // Inform the user that the file was written
    printf("Written sorted array to results.dat\n");

    // Free the memory allocated to data_array, numarray, threads
    free(data_array);
    free(numarray);
    free(threads);

    // Close the open file
    fclose(fp);

    // Destroy the mutex locks and conditions
    rc = pthread_mutex_destroy( & printMutex);
    // If mutex destroy failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_destroy()\n");
        // Exit program
        return -1;
    }
    rc = pthread_cond_destroy( & printThreadCondition);
    // If mutex destroy failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_destroy()\n");
        // Exit program
        return -1;
    }

    rc = pthread_mutex_destroy( & printLockMutex);
    // If mutex destroy failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_destroy()\n");
        // Exit program
        return -1;
    }

    rc = pthread_cond_destroy( & printLockCondition);
    // If mutex destroy failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_destroy()\n");
        // Exit program
        return -1;
    }

    rc = pthread_mutex_destroy( & modeMutex);
    // If mutex destroy failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_destroy()\n");
        // Exit program
        return -1;
    }

    // Get end time
    gettimeofday( & end, NULL);

    // Print the ending message informing how long the mergesort took in nanoseconds (using end-start time)
    printf("Mergesort completed after %ld nanoseconds\n", (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_usec - start.tv_usec) * 1000);

    // Exit program with no errors
    return 0;
}

// HELP FUNCTIONS

int isNumber(char * s) {

    // i : used as an index
    // len : used for the length of the input s

    int i, len;

    // If input is NULL then return -1 as an error
    if (s == NULL) return -1;
    // Find the length of the input
    len = strlen(s);
    // If the first character is '-', bypass it
    if (s[0] == '-') i = 1;
    else i = 0;
    // Loop through all the characters
    for (; i < len; i++) {
        // If we find a character which is not a digit return 0
        if (!isdigit(s[i])) return 0;
    }
    // All are digits return 1
    return 1;
}

void * selectionSortMode1(void * args) {

    // Declare variables used

    // data : the thread's data that has been passed as an argument to the sorting function
    // arr : the number array to be sorted
    // start : the starting point in the array for the sorting
    // end : the ending point in the array for the sorting
    // i,j : used as indexes
    // pos, temp : used for the actual sorting as index and temp value
    // rc : used for the return value of mutexes and conditions for error checking

    SORT_DATA * data;
    int * arr;
    int start, end, i, j, pos, temp, rc;

    // Convert the void* data into SORT_DATA
    data = (SORT_DATA * ) args;

    // Get the data 
    arr = data -> arr;
    start = data -> start;
    end = data -> end;

    // Lock for printing all the started messages before the threads begin sorting

    rc = pthread_mutex_lock( & printLockMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Print the started message
    printf("Started thread %d for sorting cells from %d to %d\n", data -> thid, data -> start, data -> end);
    // Decrease the number of printCount by 1 
    printCount--;
    // If printCount == 0 then all started messages have been printed
    if (printCount == 0) {
        // Broadcast to all threads to start sorting using condition printLockCondition to wake them
        rc = pthread_cond_broadcast( & printLockCondition);
        // If condition broadcast failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_boardcast()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // While there are more thread started messages to be printed
    while (printCount != 0) {
        // Wait for them to be printed using condition printLockCOndition and printLockMutex
        rc = pthread_cond_wait( & printLockCondition, & printLockMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // All thread started messages have been printed, so unlock the mutex
    rc = pthread_mutex_unlock( & printLockMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }

    // Start sorting -- selection sort

    for (i = start; i < end; i++) {

        // Initialize position to i

        pos = i;

        // Lock before accessing or writing into the array -- Mode 1

        rc = pthread_mutex_lock( & modeMutex);
        // If mutex lock failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_mutex_lock()\n");
            // Exit thread
            pthread_exit( & rc);
        }

        // Find the right position for the number being sorted

        for (j = i + 1; j <= end; j++) {
            if (arr[pos] > arr[j]) pos = j;
        }

        // If the position has changed then swap the numbers

        if (pos != i) {
            temp = arr[i];
            arr[i] = arr[pos];
            arr[pos] = temp;
        }

        // Unlock after accessing and writing into the array -- Mode 1

        rc = pthread_mutex_unlock( & modeMutex);
        // If mutex unlock failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_mutex_unlock()\n");
            // Exit thread
            pthread_exit( & rc);
        }

    }
    // Lock printMutex so that it can wait and be synchronized with the other threads
    // to print the sorted numbers in descending thread id order
    rc = pthread_mutex_lock( & printMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // While the thread's id isn't the id in the desceding printing order
    while (printThreadId != data -> thid) {
        // Thread waits for it's order denoted by printThreadId, condition printThreadCOnditio and mutex printMutex
        rc = pthread_cond_wait( & printThreadCondition, & printMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // The thread's order has come to print the numbers it sorted
    // Print the number the thread sorted
    printf("Thread %d sorted numbers: ", data -> thid);
    for (i = start; i <= end; i++) {
        printf("%d ", (data -> arr)[i]);
    }
    printf("\n");
    // Decrease the printThreadId for the next in order thread to print it's numbers
    printThreadId--;
    // Broadcast to all threads to check if it is their order to print their numbers
    rc = pthread_cond_broadcast( & printThreadCondition);
    // If condition broadcast failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_broadcast()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Unlock printMutex after printing the numbers and broadcasting for the other threads to print their numbers
    rc = pthread_mutex_unlock( & printMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_unlock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // function returns NULL -- no errors
    return NULL;
}

void * selectionSortMode2(void * args) {

    // Declare variables used

    // data : the thread's data that has been passed as an argument to the sorting function
    // arr : the number array to be sorted
    // start : the starting point in the array for the sorting
    // end : the ending point in the array for the sorting
    // i,j : used as indexes
    // pos, temp : used for the actual sorting as index and temp value
    // rc : used for the return value of mutexes and conditions for error checking

    SORT_DATA * data;
    int * arr;
    int start, end, i, j, pos, temp, rc;

    // Convert the void* data into SORT_DATA
    data = (SORT_DATA * ) args;

    // Get the data 
    arr = data -> arr;
    start = data -> start;
    end = data -> end;

    // Lock for printing all the started messages before the threads begin sorting

    rc = pthread_mutex_lock( & printLockMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Print the started message
    printf("Started thread %d for sorting cells from %d to %d\n", data -> thid, data -> start, data -> end);
    // Decrease the number of printCount by 1 
    printCount--;
    // If printCount == 0 then all started messages have been printed
    if (printCount == 0) {
        // Broadcast to all threads to start sorting using condition printLockCondition to wake them
        rc = pthread_cond_broadcast( & printLockCondition);
        // If condition broadcast failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_boardcast()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // While there are more thread started messages to be printed
    while (printCount != 0) {
        // Wait for them to be printed using condition printLockCondition and printLockMutex
        rc = pthread_cond_wait( & printLockCondition, & printLockMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // All thread started messages have been printed, so unlock the mutex
    rc = pthread_mutex_unlock( & printLockMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }

    // Start sorting -- selection sort

    for (i = start; i < end; i++) {

        // Initialize position to i

        pos = i;

        // Find the right position for the number being sorted

        for (j = i + 1; j <= end; j++) {
            if (arr[pos] > arr[j]) pos = j;
        }

        // If the position has changed then swap the numbers

        if (pos != i) {
            temp = arr[i];

            // Lock before writing to array -- Mode 2

            rc = pthread_mutex_lock( & modeMutex);
            // If mutex lock failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_lock()\n");
                // Exit thread
                pthread_exit( & rc);
            }

            arr[i] = arr[pos];
            arr[pos] = temp;

            // Unlock after writing to array -- Mode 2

            rc = pthread_mutex_unlock( & modeMutex);
            // If mutex unlock failed
            if (rc != 0) {
                // Print error
                fprintf(stderr, "Error with pthread_mutex_unlock()\n");
                // Exit thread
                pthread_exit( & rc);
            }

        }

    }
    // Lock printMutex so that it can wait and be synchronized with the other threads
    // to print the sorted numbers in descending thread id order
    rc = pthread_mutex_lock( & printMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // While the thread's id isn't the id in the desceding printing order
    while (printThreadId != data -> thid) {
        // Thread waits for it's order denoted by printThreadId, condition printThreadCOnditio and mutex printMutex
        rc = pthread_cond_wait( & printThreadCondition, & printMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // The thread's order has come to print the numbers it sorted
    // Print the number the thread sorted
    printf("Thread %d sorted numbers: ", data -> thid);
    for (i = start; i <= end; i++) {
        printf("%d ", (data -> arr)[i]);
    }
    printf("\n");
    // Decrease the printThreadId for the next in order thread to print it's numbers
    printThreadId--;
    // Broadcast to all threads to check if it is their order to print their numbers
    rc = pthread_cond_broadcast( & printThreadCondition);
    // If condition broadcast failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_broadcast()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Unlock printMutex after printing the numbers and broadcasting for the other threads to print their numbers
    rc = pthread_mutex_unlock( & printMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_unlock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // function returns NULL -- no errors
    return NULL;
}

void * selectionSortMode3(void * args) {

    // Declare variables used

    // data : the thread's data that has been passed as an argument to the sorting function
    // arr : the number array to be sorted
    // start : the starting point in the array for the sorting
    // end : the ending point in the array for the sorting
    // i,j : used as indexes
    // pos, temp : used for the actual sorting as index and temp value
    // rc : used for the return value of mutexes and conditions for error checking

    SORT_DATA * data;
    int * arr;
    int start, end, i, j, pos, temp, rc;

    // Convert the void* data into SORT_DATA
    data = (SORT_DATA * ) args;

    // Get the data 
    arr = data -> arr;
    start = data -> start;
    end = data -> end;

    // Lock for printing all the started messages before the threads begin sorting

    rc = pthread_mutex_lock( & printLockMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Print the started message
    printf("Started thread %d for sorting cells from %d to %d\n", data -> thid, data -> start, data -> end);
    // Decrease the number of printCount by 1 
    printCount--;
    // If printCount == 0 then all started messages have been printed
    if (printCount == 0) {
        // Broadcast to all threads to start sorting using condition printLockCondition to wake them
        rc = pthread_cond_broadcast( & printLockCondition);
        // If condition broadcast failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_boardcast()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // While there are more thread started messages to be printed
    while (printCount != 0) {
        // Wait for them to be printed using condition printLockCOndition and printLockMutex
        rc = pthread_cond_wait( & printLockCondition, & printLockMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // All thread started messages have been printed, so unlock the mutex
    rc = pthread_mutex_unlock( & printLockMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }

    // Start sorting -- selection sort
    // Array not locked for accessing or writing -- Mode 3

    for (i = start; i < end; i++) {

        // Initialize position to i

        pos = i;

        // Find the right position for the number being sorted

        for (j = i + 1; j <= end; j++) {
            if (arr[pos] > arr[j]) pos = j;
        }

        // If the position has changed then swap the numbers

        if (pos != i) {
            temp = arr[i];
            arr[i] = arr[pos];
            arr[pos] = temp;
        }

    }
    // Lock printMutex so that it can wait and be synchronized with the other threads
    // to print the sorted numbers in descending thread id order
    rc = pthread_mutex_lock( & printMutex);
    // If mutex lock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_lock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // While the thread's id isn't the id in the desceding printing order
    while (printThreadId != data -> thid) {
        // Thread waits for it's order denoted by printThreadId, condition printThreadCOnditio and mutex printMutex
        rc = pthread_cond_wait( & printThreadCondition, & printMutex);
        // If condition wait failed
        if (rc != 0) {
            // Print error
            fprintf(stderr, "Error with pthread_cond_wait()\n");
            // Exit thread
            pthread_exit( & rc);
        }
    }
    // The thread's order has come to print the numbers it sorted
    // Print the number the thread sorted
    printf("Thread %d sorted numbers: ", data -> thid);
    for (i = start; i <= end; i++) {
        printf("%d ", (data -> arr)[i]);
    }
    printf("\n");
    // Decrease the printThreadId for the next in order thread to print it's numbers
    printThreadId--;
    // Broadcast to all threads to check if it is their order to print their numbers
    rc = pthread_cond_broadcast( & printThreadCondition);
    // If condition broadcast failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_cond_broadcast()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // Unlock printMutex after printing the numbers and broadcasting for the other threads to print their numbers
    rc = pthread_mutex_unlock( & printMutex);
    // If mutex unlock failed
    if (rc != 0) {
        // Print error
        fprintf(stderr, "Error with pthread_mutex_unlock()\n");
        // Exit thread
        pthread_exit( & rc);
    }
    // function returns NULL -- no errors
    return NULL;
}

void merge(int * arr, int left, int mid, int right) {
    // a is the index for the first threaded space in the array being merged
    // b is the index for the second threaded space in the array being merged
    // i is for indexing the temp array
    // temp is used as a temporal array which will be copied after the merging to the original array
    int a, b, i, j;
    int temp[right - left];

    // For every number in the spaces of a and b check which is less and put it in the temp array
    // moving it's index accordingly until one of the threaded spaces reaches it's end
    for (a = left, b = mid + 1, i = 0; a <= mid && b <= right; i++) {
        if (arr[a] <= arr[b]) temp[i] = arr[a++];
        else temp[i] = arr[b++];
    }

    // While there are more numbers remaining in the first threaded space
    // insert everything into the temp array 
    while (a <= mid) {
        temp[i++] = arr[a++];
    }

    // While there are more numbers remaining in the second threaded space
    // insert everything into the temp array 
    while (b <= right) {
        temp[i++] = arr[b++];
    }

    // Set j to 0
    j = 0;

    // Copy the merged array which is in temp into the original array
    for (i = left; i <= right; i++) arr[i] = temp[j++];

}