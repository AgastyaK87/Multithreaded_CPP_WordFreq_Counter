#include "word_counter.h"
#include <iostream>
#include <chrono> // For timing
#include <iomanip> // For formatting output
#include <thread>  // For hardware_concurrency()

// A simple timer class
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed_ms() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start_;
        return diff.count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};


int main() {
    const std::string filename = "../moby_dick.txt"; // Make sure book.txt is in the root
    const int num_threads = std::thread::hardware_concurrency();

    std::cout << "Loading file: " << filename << std::endl;
    std::cout << "Using " << num_threads << " threads (based on hardware concurrency)" << std::endl;

    std::vector<std::string> words;
    try {
        words = load_words(filename);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "Loaded " << words.size() << " words." << std::endl;
    std::cout << "---" << std::endl;

    // --- 1. Single-Threaded ---
    std::cout << "Running Single-Threaded..." << std::endl;
    Timer timer_st;
    WordMap counts_st = count_single_threaded(words);
    double time_st = timer_st.elapsed_ms();
    std::cout << "Done." << std::endl << std::endl;

    // --- 2. Naive Multithreaded ---
    std::cout << "Running Naive Multithreaded..." << std::endl;
    Timer timer_naive;
    WordMap counts_naive = count_naive_multithreaded(words, num_threads);
    double time_naive = timer_naive.elapsed_ms();
    std::cout << "Done." << std::endl << std::endl;

    // --- 3. Fast Multithreaded ---
    std::cout << "Running Fast Multithreaded (Thread-Local)..." << std::endl;
    Timer timer_fast;
    WordMap counts_fast = count_fast_multithreaded(words, num_threads);
    double time_fast = timer_fast.elapsed_ms();
    std::cout << "Done." << std::endl << std::endl;

    // --- Results ---
    std::cout << "--- 📊 Performance Results ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Single-Threaded: " << time_st << " ms" << std::endl;
    std::cout << "Naive Multithreaded: " << time_naive << " ms" << std::endl;
    std::cout << "Fast Multithreaded: " << time_fast << " ms" << std::endl;

    std::cout << "\n--- Speedup (vs Single-Threaded) ---" << std::endl;
    std::cout << "Naive: " << (time_st / time_naive) << "x" << std::endl;
    std::cout << "Fast:  " << (time_st / time_fast) << "x" << std::endl;

    // --- Correctness Check ---
    std::cout << "\n--- ✅ Correctness Check ---" << std::endl;
    if (counts_st == counts_naive && counts_st == counts_fast) {
        std::cout << "PASSED: All maps are identical." << std::endl;
    } else {
        std::cout << "FAILED: Maps do not match!" << std::endl;
        if (counts_st != counts_naive) {
            std::cout << "  (Single-Threaded vs Naive FAILED)" << std::endl;
        }
        if (counts_st != counts_fast) {
            std::cout << "  (Single-Threaded vs Fast FAILED)" << std::endl;
        }
    }

    // Optional: Print top 5 words from the base version
    // (This requires a bit more logic to sort the map)

    return 0;
}