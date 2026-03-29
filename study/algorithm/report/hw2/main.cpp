#include <chrono>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;
using std::chrono::system_clock;

bool check_sorted(int* arr, int n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

void print_arr(int* arr, int n) {
    for (size_t i = 0; i < n; i++) {
        if (i) cout << ", ";
        cout << arr[i];
    }
    cout << endl;
}

int* gen_data(int n, int seed = 0) {
    int* arr = new int[n];
    mt19937 rs(seed);
    uniform_int_distribution<int> dist(0, INT32_MAX);

    for (size_t i = 0; i < n; i++) {
        arr[i] = dist(rs);
    }
    return arr;
}

// copy this template and implement your algorihtm
int* mysort(int* arr, int n) {
    int* sorted = new int[n];
    for (size_t i = 0; i < n; i++) {
        sorted[i] = arr[i];
    }
    // implement your algorithm

    return sorted;
}
int* merge_sort(int* arr, int n) {
    // implement your algorithm
}
int* quick_sort_rand_piv(int* arr, int n) {
    // implement your algorithm
}
int* quick_sort_last_piv(int* arr, int n) {
    // implement your algorithm
}
int* heap_sort(int* arr, int n) {
    // implement your algorithm
}
int* radix_sort(int* arr, int n) {
    // implement your algorithm
}

int main(void) {
    system_clock::time_point begin;
    double duration;
    bool is_sorted;
    int* arr;
    int n;

    // example of gen_data
    arr = gen_data(10);
    cout << "Example: ";
    print_arr(arr, 10);
    delete[] arr;

    n = 100000000;
    begin = system_clock::now();
    arr = gen_data(n);
    duration = duration_cast<milliseconds>(system_clock::now() - begin).count() / 1000.0;
    cout << "Time for gen data: " << duration << "s" << endl;

    begin = system_clock::now();
    int* sorted = mysort(arr, n);
    duration = duration_cast<milliseconds>(system_clock::now() - begin).count() / 1000.0;
    cout << "Time for sort: " << duration << "s" << endl;

    is_sorted = check_sorted(sorted, n);
    cout << "Sorted(1: true, 0:false): " << check_sorted(sorted, n) << endl;
    if (!is_sorted) {
        cout << "Try the implementation again" << endl;
    }
    delete[] sorted;

    delete[] arr;
    return 0;
}