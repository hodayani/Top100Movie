#pragma once
#include <atomic>
#include <string>
#include <vector>

struct movie
{
    int rank;
    std::string title;
    std::string description;
    std::string rating;
    std::string id;
    int year;
    std::vector<std::string> genres;
    int user_rating = 0;
};

struct CommonObjects
{
    std::string url;
    std::vector<movie> movies;
    std::vector<movie> recently_viewed; // A vector to store recently_viewed movies
    std::vector<movie> favorites; // A vector to store favorite movies
    std::atomic<bool> data_ready{ false };
    std::atomic<bool> exit_flag{ false };
};
