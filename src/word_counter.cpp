#include "word_counter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <thread>
#include <future>

// --- Helper Implementations ---

std::string normalize(const std::string& word) {
    std::string clean_word;
    for (char c : word) {
        if (std::isalpha(c)) {
            clean_word += std::tolower(c);
        }
    }
    return clean_word;
}

WordMap process_chunk(std::vector<std::string>::const_iterator begin,
                      std::vector<std::string>::const_iterator end) {
    WordMap local_counts;
    for (auto it = begin; it != end; ++it) {
        // We are only writing to OUR OWN local_counts.
        // No other thread can see this map.
        // NO MUTEX NEEDED!
        local_counts[*it]++;
    }
    return local_counts; // Return the local map
}

std::vector<std::string> load_words(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<std::string> words;
    std::string word;
    while (file >> word) {
        std::string normalized_word = normalize(word);
        if (!normalized_word.empty()) {
            words.push_back(normalized_word);
        }
    }
    return words;
}


// --- Counter Implementations ---

// Version 1: Single-Threaded
WordMap count_single_threaded(const std::vector<std::string>& words) {
    WordMap counts;
    for (const auto& word : words) {
        counts[word]++;
    }
    return counts;
}

// Version 2: Naive Mutex
// (You implement this!)
WordMap count_naive_multithreaded(const std::vector<std::string>& words, int num_threads) {
    WordMap shared_counts;
    std::mutex word_mutex;
    size_t chunk_size = words.size() / num_threads;
    // ... your logic to spawn threads ...
    std::vector<std::thread> threads;
    threads.reserve(num_threads);


    // Each thread locks the mutex to update shared_counts
    for (int i = 0; i < num_threads; i++)
    {
        auto start_itr = words.begin() + i * chunk_size;
        auto end_itr = (i == num_threads - 1) ? words.end() : start_itr + chunk_size;
        threads.emplace_back([start_itr, end_itr, &shared_counts, &word_mutex]() {
            for (auto it= start_itr; it != end_itr; ++it) {
                std::lock_guard<std::mutex> lock(word_mutex);
                shared_counts[*it]++;
            }
        });

    }

    for (auto& t : threads) {
        t.join();
    }


    return shared_counts;
}


// Version 3: Fast Thread-Local
// (You implement this!)
WordMap count_fast_multithreaded(const std::vector<std::string>& words, int num_threads) {

    // ... your logic to spawn threads/async tasks ...
    // Each task creates its OWN local map.
    // ...

    std::vector<std::future<WordMap>> futures;

    size_t chunk_size = words.size()/num_threads;

    for (int i = 0; i<num_threads; i++)

    {
        auto start_itr = words.cbegin() + i * chunk_size;
        auto end_itr = (i == num_threads - 1) ? words.cend() : start_itr + chunk_size;
        futures.push_back(
            std::async(std::launch::async, process_chunk, start_itr, end_itr)
        );

    }
    WordMap final_counts;

    for (auto& fut : futures) {
        // fut.get() WAITS for the thread to finish
        // and "gets" the WordMap that it returned.
        WordMap local_map = fut.get();

        // Now, merge this local_map into our final_counts
        for (const auto& pair : local_map) {
            const std::string& word = pair.first;
            size_t count = pair.second;
            final_counts[word] += count;
        }
    }

    return final_counts;
}
