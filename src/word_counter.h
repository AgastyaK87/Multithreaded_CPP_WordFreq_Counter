#pragma once

#include <string>
#include <vector>
#include <map>

// A type alias for clarity
using WordMap = std::map<std::string, size_t>;

// --- Helper Functions ---

// Loads the file into a vector of words.
std::vector<std::string> load_words(const std::string& filename);

// Cleans a word (lowercase, remove punctuation)
std::string normalize(const std::string& word);


// --- Counter Implementations ---

// Version 1: Single-Threaded
WordMap count_single_threaded(const std::vector<std::string>& words);

// Version 2: Naive Mutex
WordMap count_naive_multithreaded(const std::vector<std::string>& words, int num_threads);

// Version 3: Fast Thread-Local
WordMap count_fast_multithreaded(const std::vector<std::string>& words, int num_threads);