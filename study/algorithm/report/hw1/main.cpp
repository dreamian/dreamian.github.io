#include <chrono>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;
using std::chrono::system_clock;

using e_implement_mode = enum {
    e_bruteforce,
    e_bruteforce_optimized,
    e_divide_and_conquer,
    e_incremental_approach,
    e_not_defined,
};

// test parameter to control the size of input data and the implementation mode of MSA algorithm
static int n = 100000000;
static e_implement_mode g_implement_mode = e_incremental_approach;

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
    uniform_int_distribution<int> dist(INT32_MIN, INT32_MAX);

    for (size_t i = 0; i < n; i++) {
        arr[i] = dist(rs);
    }
    return arr;
}

class MSAResult {
public:
    int start_index;
    int end_index;
    long long max_sum;

    MSAResult()
    {
        start_index = -1;
        end_index = -1;
        max_sum = 0;
    }

    MSAResult(int start, int end, long long value)
    {
        start_index = start;
        end_index = end;
        max_sum = value;
    }
};

// external function for divide and conquer approach
MSAResult find_max_crossing_subarray(int left, int mid, int right, int* arr)
{
    // Initialize left_sum to a very small number to handle cases where all numbers are negative
    long long left_sum = INT32_MIN * 100'000'000LL;

    long long sum = 0;
    int max_left = mid;

    for (int i = mid; i >= left; --i) {
        sum += arr[i];
        if (sum > left_sum) {
            left_sum = sum;
            max_left = i;
        }
    }

    long long right_sum = INT32_MIN * 100'000'000LL;
    sum = 0;
    int max_right = mid + 1;

    for (int j = mid + 1; j <= right; ++j) {
        sum += arr[j];
        if (sum > right_sum) {
            right_sum = sum;
            max_right = j;
        }
    }

    return MSAResult(max_left, max_right, left_sum + right_sum);
};

MSAResult find_max_subarray(int left, int right, int* arr)
{
    if (left == right) {
        return MSAResult(left, right, arr[left]);
    }

    int mid = left + (right - left) / 2;

    MSAResult left_result = find_max_subarray(left, mid, arr);
    MSAResult right_result = find_max_subarray(mid + 1, right, arr);
    MSAResult cross_result = find_max_crossing_subarray(left, mid, right, arr);

    if (left_result.max_sum >= right_result.max_sum && left_result.max_sum >= cross_result.max_sum) {
        return left_result;
    } else if (right_result.max_sum >= left_result.max_sum && right_result.max_sum >= cross_result.max_sum) {
        return right_result;
    } else {
        return cross_result;
    }
};

// Copy this template and implement your algorithm
// Please return the start index, end index, value(sum) of maximum subarray
// If there are only negative numbers in the array, set msa_result as {start index = -1, end index = -1, value(sum) = 0}
MSAResult mymsa(int* arr, int n) {
    MSAResult msa_result(-1, -1, 0);

    switch(g_implement_mode) {
        case e_bruteforce:
            {
                msa_result.max_sum = 0;
                for(int i = 0; i < n; ++i)
                {
                    for(int j = i; j < n; ++j)
                    {
                        long long this_sum = 0;
                        for(int k = i; k <= j; ++k)
                        {
                            this_sum += arr[k];
                        }
                        if(this_sum > msa_result.max_sum)
                        {
                            msa_result.max_sum = this_sum;
                            msa_result.start_index = i;
                            msa_result.end_index = j;
                        }
                    }
                }
            }
            break;
        case e_bruteforce_optimized:
            {
                msa_result.max_sum = 0;
                for(int i = 0; i < n; ++i)
                {
                    long long this_sum = 0;
                    for(int j = i; j < n; ++j)
                    {
                        this_sum += arr[j];
                        if(this_sum > msa_result.max_sum)
                        {
                            msa_result.max_sum = this_sum;
                            msa_result.start_index = i;
                            msa_result.end_index = j;
                        }
                    }
                }
            }
            break;
        case e_divide_and_conquer:
            {
                msa_result = find_max_subarray(0, n - 1, arr);
            }
            break;
        case e_incremental_approach:
            {
                msa_result.max_sum = 0;

                long long this_sum = 0;
                int current_start_index = 0;
                for(int j = 0; j < n; ++j)
                {
                    this_sum += arr[j];
                    if(this_sum > msa_result.max_sum)
                    {
                        msa_result.max_sum = this_sum;
                        msa_result.start_index = current_start_index;
                        msa_result.end_index = j;
                    }
                    if(this_sum < 0)
                    {
                        this_sum = 0;
                        current_start_index = j + 1;
                    }
                }
            }
            break;
        default:
            cout << "Invalid implementation mode" << endl;
            return msa_result;
    }

    if(msa_result.max_sum < 0)
    {
        msa_result.start_index = -1;
        msa_result.end_index = -1;
        msa_result.max_sum = 0;
    }

    return msa_result;
}

int main(void) {
    int seed[] = {260328, 260329, 260330, 260331, 260401};
    double avg_duration_gen_data = 0.0;
    double avg_duration_msa = 0.0;
    for(const auto& seed_value: seed)
    {
        system_clock::time_point begin;
        double duration;
        int* arr;
        begin = system_clock::now();
        arr = gen_data(n, seed_value);
        duration = duration_cast<milliseconds>(system_clock::now() - begin).count() / 1000.0;
        cout << "Time for gen data: " << duration << "s" << endl;
        avg_duration_gen_data += duration;

        begin = system_clock::now();
        MSAResult msa_result = mymsa(arr, n);
        duration = duration_cast<milliseconds>(system_clock::now() - begin).count() / 1000.0;
        avg_duration_msa += duration;
        cout << "Time for finding MSA: " << duration << "s" << endl;
        cout << "SEED: " << seed_value << ", N: " << n << ", ";
        cout << "MSA: [" << msa_result.start_index << ", " << msa_result.end_index << "], sum = " << msa_result.max_sum << endl;

        delete[] arr;
    }

    avg_duration_gen_data /= sizeof(seed) / sizeof(seed[0]);
    avg_duration_msa /= sizeof(seed) / sizeof(seed[0]);

    cout << "==============================" << endl;
    cout << "Average time for gen data: " << avg_duration_gen_data << "s" << endl;
    cout << "Average time for finding MSA: " << avg_duration_msa << "s" << endl;

    return 0;
}