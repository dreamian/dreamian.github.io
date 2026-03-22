#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

extern void _sort_func(std::vector<int>& a); // 측정할 정렬 함수 (예: insertion_sort  등  사용자 정의 정렬 함수로 교체 가능   )

using namespace std;
using Clock = chrono::high_resolution_clock;

struct Result {
    int N;
    double random_ms;
    double ascending_ms;
    double descending_ms;
};

template <typename SortFunc>
double measure_ms(const vector<int>& input, SortFunc sort_func, int repeat = 3) {
    vector<double> times;
    times.reserve(repeat);

    for (int r = 0; r < repeat; ++r) {
        vector<int> a = input;  // 매 반복마다 복사본 정렬
        auto t1 = Clock::now();
        sort_func(a);
        auto t2 = Clock::now();

        // verify solution (정렬이 제대로 되었는지 확인)
        if (!is_sorted(a.begin(), a.end())) {
            cerr << "Error: sorting failed!\n";
            exit(1);
        }

        double ms = chrono::duration<double, milli>(t2 - t1).count();
        times.push_back(ms);
    }

    // 중앙값 사용
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

vector<int> make_test_sizes(int max_n) {
    vector<int> sizes;

    // 너무 작은 값부터 전부 돌리면 의미가 약해서 적당히 시작
    for (int n = 1024; n <= max_n; n *= 2) {
        sizes.push_back(n);
        if (n > max_n / 2) break;
    }

    if (sizes.empty() || sizes.back() != max_n) {
        sizes.push_back(max_n);
    }

    return sizes;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int MAX_N = 65536;
    const int MIN_VAL = 1;
    const int MAX_VAL = 65536;
    const int REPEAT = 5;

    mt19937 rng(123456789);
    uniform_int_distribution<int> dist(MIN_VAL, MAX_VAL);

    auto sizes = make_test_sizes(MAX_N);
    vector<Result> results;

    for (int N : sizes) {
        vector<int> base(N);
        for (int i = 0; i < N; ++i) {
            base[i] = dist(rng);
        }

        vector<int> random_case = base;

        vector<int> ascending_case = base;
        sort(ascending_case.begin(), ascending_case.end());

        vector<int> descending_case = ascending_case;
        reverse(descending_case.begin(), descending_case.end());

        double t_random = measure_ms(random_case, _sort_func, REPEAT);
        double t_asc    = measure_ms(ascending_case, _sort_func, REPEAT);
        double t_desc   = measure_ms(descending_case, _sort_func, REPEAT);

        results.push_back({N, t_random, t_asc, t_desc});

        cout << "N=" << N
             << " | random=" << t_random << " ms"
             << " | asc=" << t_asc << " ms"
             << " | desc=" << t_desc << " ms\n";
    }

    // CSV 저장
    {
        auto filename = std::string(argv[1]) + "_results.csv";
        ofstream fout(filename);
        fout << "N,random_ms,ascending_ms,descending_ms\n";
        for (const auto& r : results) {
            fout << r.N << ","
                 << r.random_ms << ","
                 << r.ascending_ms << ","
                 << r.descending_ms << "\n";
        }
    }

    return 0;
}