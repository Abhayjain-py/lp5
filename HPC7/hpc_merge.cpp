// parallel_merge_sort.cpp
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

// Merge two subarrays
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
        
        if (depth < 3) {  // Only parallelize first few levels
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

// Generate random array
vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 10000;
    }
    return arr;
}

// Print first 10 elements
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
    
    int sizes[] = {10000, 20000};
    
    for (int size : sizes) {
        cout << "\n======= Array Size: " << size << " =======" << endl;
        
        vector<int> originalArray = generateRandomArray(size);
        vector<int> arr1 = originalArray; // For sequential merge sort
        vector<int> arr2 = originalArray; // For parallel merge sort
        
        printFirst10(originalArray, "Original array");
        
        double start, end;
        
        cout << "\nMerge Sort:" << endl;
        
        // Sequential Merge Sort
        start = omp_get_wtime();
        mergeSortSequential(arr1, 0, arr1.size() - 1);
        end = omp_get_wtime();
        cout << "  Sequential: " << (end - start) << " seconds" << endl;
        printFirst10(arr1, "  Sorted");
        
        // Parallel Merge Sort
        start = omp_get_wtime();
        mergeSortParallel(arr2, 0, arr2.size() - 1);
        end = omp_get_wtime();
        cout << "  Parallel:   " << (end - start) << " seconds" << endl;
        printFirst10(arr2, "  Sorted");
    }
    
    return 0;
}
