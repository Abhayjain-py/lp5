// parallel_bubble_sort.cpp
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

// Parallel Bubble Sort
void bubbleSortParallel(vector<int>& arr) {
    int n = arr.size();
    bool swapped = true;
    
    while (swapped) {
        swapped = false;
        
        #pragma omp parallel for shared(arr, swapped)
        for (int j = 1; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        
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
        vector<int> arr1 = originalArray; // For sequential bubble sort
        vector<int> arr2 = originalArray; // For parallel bubble sort
        
        printFirst10(originalArray, "Original array");
        
        double start, end;
        
        cout << "\nBubble Sort:" << endl;
        
        // Sequential Bubble Sort
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
    }
    
    return 0;
}
