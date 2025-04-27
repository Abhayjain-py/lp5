#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

// Function to merge two subarrays
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    vector<int> L(n1), R(n2);
    
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
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
    
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Sequential Merge Sort
void mergeSortSequential(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortSequential(arr, left, mid);
        mergeSortSequential(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Parallel Merge Sort
void mergeSortParallel(vector<int>& arr, int left, int right, int depth = 0) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        if (depth < 3) {  // Only use parallel sections for first few levels
            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    mergeSortParallel(arr, left, mid, depth + 1);
                }
                
                #pragma omp section
                {
                    mergeSortParallel(arr, mid + 1, right, depth + 1);
                }
            }
        } else {
            mergeSortSequential(arr, left, mid);
            mergeSortSequential(arr, mid + 1, right);
        }
        
        merge(arr, left, mid, right);
    }
}

// Parallel Bubble Sort
void bubbleSortParallel(vector<int>& arr) {
    int n = arr.size();
    bool swapped = true;
    
    while (swapped) {
        swapped = false;
        
        // Odd phase
        #pragma omp parallel for shared(arr, swapped)
        for (int j = 1; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        
        // Even phase
        #pragma omp parallel for shared(arr, swapped)
        for (int j = 0; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
    }
}

// Sequential Bubble Sort
void bubbleSortSequential(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Generate random array
vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 10000;
    }
    return arr;
}

// Print first 10 elements of array
void printFirst10(const vector<int>& arr, const string& label) {
    cout << label << " (first 10): ";
    for (int i = 0; i < 10 && i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main() {
    srand(time(nullptr));
    int numThreads = omp_get_max_threads();
    cout << "Running with " << numThreads << " threads" << endl;
    
    // Test only with sizes 10000 and 20000
    int sizes[] = {10000, 20000};
    
    for (int size : sizes) {
        cout << "\n======= Array Size: " << size << " =======" << endl;
        
        // Generate random array
        vector<int> originalArray = generateRandomArray(size);
        vector<int> arr1 = originalArray;  // For sequential bubble sort
        vector<int> arr2 = originalArray;  // For parallel bubble sort
        vector<int> arr3 = originalArray;  // For sequential merge sort
        vector<int> arr4 = originalArray;  // For parallel merge sort
        
        printFirst10(originalArray, "Original array");
        
        double start, end;
        
        // Sequential Bubble Sort
        cout << "\nBubble Sort:" << endl;
        start = omp_get_wtime();
        bubbleSortSequential(arr1);
        end = omp_get_wtime();
        cout << "  Sequential: " << (end - start) << " seconds" << endl;
        printFirst10(arr1, "  Sorted");
        
        // Parallel Bubble Sort
        start = omp_get_wtime();
        bubbleSortParallel(arr2);
        end = omp_get_wtime();
        cout << "  Parallel:   " << (end - start) << " seconds" << endl;
        printFirst10(arr2, "  Sorted");
        
        // Sequential Merge Sort
        cout << "\nMerge Sort:" << endl;
        start = omp_get_wtime();
        mergeSortSequential(arr3, 0, arr3.size() - 1);
        end = omp_get_wtime();
        cout << "  Sequential: " << (end - start) << " seconds" << endl;
        printFirst10(arr3, "  Sorted");
        
        // Parallel Merge Sort
        start = omp_get_wtime();
        mergeSortParallel(arr4, 0, arr4.size() - 1);
        end = omp_get_wtime();
        cout << "  Parallel:   " << (end - start) << " seconds" << endl;
        printFirst10(arr4, "  Sorted");
    }
    
    return 0;
}

// PS D:\DEGREE\8th Sem\practicals\HPC> g++ -fopenmp pr2.cpp -o pr2
// PS D:\DEGREE\8th Sem\practicals\HPC> ./pr2
// Running with 4 threads

// ======= Array Size: 10000 =======
// Original array (first 10): 1234 6050 5078 3636 4764 7242 1488 7213 3735 4522 

// Bubble Sort:
//   Sequential: 1.046 seconds
//   Sorted (first 10): 0 0 1 1 1 3 4 4 5 5
//   Parallel:   1.652 seconds
//   Sorted (first 10): 0 0 1 1 1 3 4 4 5 5

// Merge Sort:
//   Sequential: 0.00800014 seconds
//   Sorted (first 10): 0 0 1 1 1 3 4 4 5 5 
//   Parallel:   0.0109999 seconds
//   Sorted (first 10): 0 0 1 1 1 3 4 4 5 5

// ======= Array Size: 20000 =======
// Original array (first 10): 10 823 3474 1453 981 8430 5524 8182 774 7139

// Bubble Sort:
//   Sequential: 3.81 seconds
//   Sorted (first 10): 1 1 1 1 2 2 2 2 3 3
//   Parallel:   4.605 seconds
//   Sorted (first 10): 1 1 1 1 2 2 2 2 3 3

// Merge Sort:
//   Sequential: 0.0220001 seconds
//   Sorted (first 10): 1 1 1 1 2 2 2 2 3 3 
//   Parallel:   0.0189998 seconds
//   Sorted (first 10): 1 1 1 1 2 2 2 2 3 3
// PS D:\DEGREE\8th Sem\practicals\HPC> 