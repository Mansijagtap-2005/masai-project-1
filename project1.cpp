#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cctype> // For tolower
#include <filesystem>

namespace fs = std::filesystem;

const int TOTAL_BOOKS = 64;
const int TOP_PAIRS = 10;

std::unordered_map<std::string, int> getWordFrequency(const std::string &filename) {
    std::unordered_map<std::string, int> wordFreq;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Warning: Could not open file " << filename << "\n";
        return wordFreq;
    }
    std::string word;
    while (file >> word) {
        // Convert word to lowercase and remove non-alphanumeric characters
        std::string cleanWord;
        for (char ch : word) {
            if (std::isalnum(ch)) cleanWord += std::tolower(ch);
        }
        if (!cleanWord.empty()) wordFreq[cleanWord]++;
    }
    return wordFreq;
}

double cosineSimilarity(const std::unordered_map<std::string, int> &freq1,
                        const std::unordered_map<std::string, int> &freq2) {
    double dotProduct = 0, magnitude1 = 0, magnitude2 = 0;

    for (const auto &pair : freq1) {
        auto it = freq2.find(pair.first);
        if (it != freq2.end()) {
            dotProduct += pair.second * it->second;
        }
    }

    for (const auto &pair : freq1) {
        magnitude1 += pair.second * pair.second;
    }
    for (const auto &pair : freq2) {
        magnitude2 += pair.second * pair.second;
    }

    if (magnitude1 == 0 || magnitude2 == 0) return 0.0;
    return dotProduct / (std::sqrt(magnitude1) * std::sqrt(magnitude2));
}

struct SimilarityResult {
    int book1;
    int book2;
    double similarity;

    bool operator<(const SimilarityResult &other) const {
        return similarity > other.similarity;
    }
};

int main() {

    std::vector<std::unordered_map<std::string, int>> bookFrequencies;
    for (int i = 1; i <= TOTAL_BOOKS; ++i) {
        std::string filename = "book" + std::to_string(i) + ".txt";
        if (fs::exists(filename)) {
            bookFrequencies.push_back(getWordFrequency(filename));
        } else {
            bookFrequencies.push_back({});
            std::cerr << "Warning: " << filename << " not found.\n";
        }
    }

    std::vector<SimilarityResult> similarities;
    for (int i = 0; i < TOTAL_BOOKS; ++i) {
        if (bookFrequencies[i].empty()) continue;
        for (int j = i + 1; j < TOTAL_BOOKS; ++j) {
            if (bookFrequencies[j].empty()) continue;
            double similarity = cosineSimilarity(bookFrequencies[i], bookFrequencies[j]);
            similarities.push_back({i + 1, j + 1, similarity});
        }
    }

    std::sort(similarities.begin(), similarities.end());

    std::cout << "Top " << TOP_PAIRS << " similar pairs of books:\n";
    std::cout << std::fixed << std::setprecision(2);
    for (int i = 0; i < TOP_PAIRS && i < similarities.size(); ++i) {
        std::cout << "Book " << similarities[i].book1 << " and Book "
                  << similarities[i].book2 << " - Similarity: "
                  << similarities[i].similarity << "\n";
    }

    return 0;
}
