#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

using Clock = std::chrono::steady_clock;
using Seconds = std::chrono::duration<double>;

struct MaxSubarrayResult {
    long long sum;
    std::size_t left;
    std::size_t right;
};

struct ExperimentResult {
    std::string algorithm;
    std::size_t n;
    bool recorded;
    bool memory_error;
    double average_seconds;
    std::vector<double> seed_times;
    long long sample_answer;
};

constexpr double TIME_LIMIT_SECONDS = 180.0;

std::vector<std::size_t> getInputSizes() {
    return {100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL, 100000000ULL};
}

std::vector<unsigned int> getSeeds() {
    return {20250324U, 20250325U, 20250326U, 20250327U, 20250328U};
}

std::vector<int> generateRandomData(std::size_t n, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    std::vector<int> data;
    data.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        data.push_back(dist(rng));
    }
    return data;
}

MaxSubarrayResult bruteForceCubic(const std::vector<int>& a) {
    MaxSubarrayResult best{std::numeric_limits<long long>::lowest(), 0, 0};
    const std::size_t n = a.size();

    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i; j < n; ++j) {
            long long sum = 0;
            for (std::size_t k = i; k <= j; ++k) {
                sum += static_cast<long long>(a[k]);
            }
            if (sum > best.sum) {
                best = {sum, i, j};
            }
        }
    }
    return best;
}

MaxSubarrayResult bruteForceQuadratic(const std::vector<int>& a) {
    MaxSubarrayResult best{std::numeric_limits<long long>::lowest(), 0, 0};
    const std::size_t n = a.size();

    for (std::size_t i = 0; i < n; ++i) {
        long long sum = 0;
        for (std::size_t j = i; j < n; ++j) {
            sum += static_cast<long long>(a[j]);
            if (sum > best.sum) {
                best = {sum, i, j};
            }
        }
    }
    return best;
}

MaxSubarrayResult maxCrossingSubarray(const std::vector<int>& a, std::size_t left, std::size_t mid, std::size_t right) {
    long long leftSum = std::numeric_limits<long long>::lowest();
    long long sum = 0;
    std::size_t bestLeft = mid;
    for (std::size_t i = mid + 1; i-- > left;) {
        sum += static_cast<long long>(a[i]);
        if (sum > leftSum) {
            leftSum = sum;
            bestLeft = i;
        }
        if (i == 0) {
            break;
        }
    }

    long long rightSum = std::numeric_limits<long long>::lowest();
    sum = 0;
    std::size_t bestRight = mid + 1;
    for (std::size_t j = mid + 1; j <= right; ++j) {
        sum += static_cast<long long>(a[j]);
        if (sum > rightSum) {
            rightSum = sum;
            bestRight = j;
        }
    }

    return {leftSum + rightSum, bestLeft, bestRight};
}

MaxSubarrayResult divideAndConquerRec(const std::vector<int>& a, std::size_t left, std::size_t right) {
    if (left == right) {
        return {static_cast<long long>(a[left]), left, right};
    }

    const std::size_t mid = left + (right - left) / 2;
    const MaxSubarrayResult leftResult = divideAndConquerRec(a, left, mid);
    const MaxSubarrayResult rightResult = divideAndConquerRec(a, mid + 1, right);
    const MaxSubarrayResult crossResult = maxCrossingSubarray(a, left, mid, right);

    if (leftResult.sum >= rightResult.sum && leftResult.sum >= crossResult.sum) {
        return leftResult;
    }
    if (rightResult.sum >= leftResult.sum && rightResult.sum >= crossResult.sum) {
        return rightResult;
    }
    return crossResult;
}

MaxSubarrayResult divideAndConquer(const std::vector<int>& a) {
    return divideAndConquerRec(a, 0, a.size() - 1);
}

MaxSubarrayResult incrementalApproach(const std::vector<int>& a) {
    long long bestSum = static_cast<long long>(a[0]);
    long long currentSum = static_cast<long long>(a[0]);
    std::size_t bestLeft = 0;
    std::size_t bestRight = 0;
    std::size_t currentLeft = 0;

    for (std::size_t i = 1; i < a.size(); ++i) {
        const long long value = static_cast<long long>(a[i]);
        if (currentSum + value < value) {
            currentSum = value;
            currentLeft = i;
        } else {
            currentSum += value;
        }

        if (currentSum > bestSum) {
            bestSum = currentSum;
            bestLeft = currentLeft;
            bestRight = i;
        }
    }

    return {bestSum, bestLeft, bestRight};
}

std::optional<std::pair<MaxSubarrayResult, double>> measureAlgorithm(
    const std::function<MaxSubarrayResult(const std::vector<int>&)>& algorithm,
    const std::vector<int>& data
) {
    const auto start = Clock::now();
    const MaxSubarrayResult result = algorithm(data);
    const auto end = Clock::now();
    const double elapsed = std::chrono::duration_cast<Seconds>(end - start).count();

    if (elapsed > TIME_LIMIT_SECONDS) {
        return std::nullopt;
    }
    return std::make_optional(std::make_pair(result, elapsed));
}

void writeAverageCsv(const std::string& fileName, const std::vector<ExperimentResult>& results) {
    std::ofstream fout(fileName);
    fout << "algorithm,n,average_seconds,status,sample_answer\n";
    for (const auto& r : results) {
        fout << r.algorithm << ',' << r.n << ',';
        if (r.recorded) {
            fout << std::fixed << std::setprecision(3) << r.average_seconds << ",RECORDED," << r.sample_answer;
        } else if (r.memory_error) {
            fout << "N/A,MEMORY_LIMIT,N/A";
        } else {
            fout << "N/A,SKIPPED_OR_TIMEOUT,N/A";
        }
        fout << "\n";
    }
}

void writeRawCsv(const std::string& fileName, const std::vector<ExperimentResult>& results) {
    std::ofstream fout(fileName);
    fout << "algorithm,n,status,seed1,seed2,seed3,seed4,seed5\n";
    for (const auto& r : results) {
        fout << r.algorithm << ',' << r.n << ',';
        if (r.recorded) {
            fout << "RECORDED";
            for (double t : r.seed_times) {
                fout << ',' << std::fixed << std::setprecision(3) << t;
            }
        } else if (r.memory_error) {
            fout << "MEMORY_LIMIT";
            for (int i = 0; i < 5; ++i) fout << ",N/A";
        } else {
            fout << "SKIPPED_OR_TIMEOUT";
            for (int i = 0; i < 5; ++i) fout << ",N/A";
        }
        fout << "\n";
    }
}

void printSummaryTable(const std::vector<ExperimentResult>& results, const std::vector<std::size_t>& sizes) {
    const std::vector<std::string> algorithms = {
        "Brute force O(n^3)",
        "Brute force O(n^2)",
        "Divide and Conquer",
        "Incremental Approach"
    };

    std::cout << "\n[Average running time in seconds]\n";
    std::cout << std::left << std::setw(24) << "Algorithm";
    for (auto n : sizes) {
        std::cout << std::right << std::setw(14) << n;
    }
    std::cout << '\n';

    for (const auto& algorithmName : algorithms) {
        std::cout << std::left << std::setw(24) << algorithmName;
        for (auto n : sizes) {
            const auto it = std::find_if(results.begin(), results.end(), [&](const ExperimentResult& r) {
                return r.algorithm == algorithmName && r.n == n;
            });
            if (it == results.end()) {
                std::cout << std::right << std::setw(14) << "-";
            } else if (it->recorded) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(3) << it->average_seconds;
                std::cout << std::right << std::setw(14) << oss.str();
            } else if (it->memory_error) {
                std::cout << std::right << std::setw(14) << "MEM";
            } else {
                std::cout << std::right << std::setw(14) << "N/A";
            }
        }
        std::cout << '\n';
    }
}

int main() {
    const std::vector<std::size_t> sizes = getInputSizes();
    const std::vector<unsigned int> seeds = getSeeds();

    struct AlgorithmSpec {
        std::string name;
        std::function<MaxSubarrayResult(const std::vector<int>&)> fn;
        bool skip_larger_sizes_due_to_timeout = false;
        bool skip_larger_sizes_due_to_memory = false;
    };

    std::vector<AlgorithmSpec> algorithms = {
        {"Brute force O(n^3)", bruteForceCubic, false, false},
        {"Brute force O(n^2)", bruteForceQuadratic, false, false},
        {"Divide and Conquer", divideAndConquer, false, false},
        {"Incremental Approach", incrementalApproach, false, false}
    };

    std::vector<ExperimentResult> allResults;

    for (std::size_t n : sizes) {
        std::cout << "\n=== n = " << n << " ===\n";

        std::vector<int> data;
        bool dataGenerationFailed = false;

        // Data generation is common to all algorithms for the same n and seed.
        // If memory allocation fails while generating the input, mark all remaining algorithms as memory-limited.
        for (auto& spec : algorithms) {
            if (spec.skip_larger_sizes_due_to_timeout || spec.skip_larger_sizes_due_to_memory) {
                allResults.push_back({spec.name, n, false, spec.skip_larger_sizes_due_to_memory, 0.0, {}, 0});
                continue;
            }

            std::vector<double> times;
            long long sampleAnswer = 0;
            bool timeoutOccurred = false;
            bool memoryErrorOccurred = false;

            for (unsigned int seed : seeds) {
                try {
                    std::cout << "Generating data for " << spec.name << ", n=" << n << ", seed=" << seed << " ...\n";
                    data = generateRandomData(n, seed);
                } catch (const std::bad_alloc&) {
                    memoryErrorOccurred = true;
                    spec.skip_larger_sizes_due_to_memory = true;
                    break;
                }

                std::cout << "  Running " << spec.name << " ... " << std::flush;
                auto measured = measureAlgorithm(spec.fn, data);
                if (!measured.has_value()) {
                    std::cout << "TIME LIMIT EXCEEDED\n";
                    timeoutOccurred = true;
                    spec.skip_larger_sizes_due_to_timeout = true;
                    break;
                }

                const auto& [result, elapsed] = measured.value();
                std::cout << std::fixed << std::setprecision(3) << elapsed << " sec\n";

                // Correctness check against Kadane for every seed.
                const MaxSubarrayResult reference = incrementalApproach(data);
                if (reference.sum != result.sum) {
                    throw std::runtime_error("Algorithm outputs do not match incremental approach.");
                }

                sampleAnswer = result.sum;
                times.push_back(elapsed);
            }

            if (memoryErrorOccurred) {
                allResults.push_back({spec.name, n, false, true, 0.0, {}, 0});
            } else if (timeoutOccurred || times.size() != seeds.size()) {
                allResults.push_back({spec.name, n, false, false, 0.0, {}, 0});
            } else {
                double sum = 0.0;
                for (double t : times) sum += t;
                allResults.push_back({spec.name, n, true, false, sum / static_cast<double>(times.size()), times, sampleAnswer});
            }
        }
    }

    printSummaryTable(allResults, sizes);
    writeAverageCsv("avg_results.csv", allResults);
    writeRawCsv("raw_results.csv", allResults);

    std::cout << "\nSaved: avg_results.csv, raw_results.csv\n";
    std::cout << "Tip: Import avg_results.csv into Excel to draw the graph required in the report.\n";
    return 0;
}
