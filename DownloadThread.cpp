#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

// Updated to include genres
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(movie, rank, title, description, rating, id, year, genres);

void DownloadThread::operator()(CommonObjects& common)
{
    auto rapidpi_key = "5cb1285e65msh7a137bc10d6fe08p150289jsn43e049875621";// API key
    auto host = "https://imdb-top-100-movies.p.rapidapi.com";// API host URL

    httplib::Client cli(host);// Create an HTTP client
    httplib::Headers heads = { { "x-rapidapi-key", rapidpi_key } }; // Set headers

    auto res = cli.Get("/", heads); // Fetch movie data
    if (res->status == 200)
    {
        auto json_result = nlohmann::json::parse(res->body);// Parse JSON response

        // Clear previous data
        common.movies.clear();

        for (const auto& item : json_result)
        {
            movie mv;
            mv.rank = item.at("rank").get<int>();
            mv.title = item.at("title").get<std::string>();
            mv.description = item.at("description").get<std::string>();
            mv.rating = item.at("rating").get<std::string>();
            mv.id = item.at("id").get<std::string>();
            mv.year = item.at("year").get<int>();

            // Parse genres
            for (const auto& genre : item.at("genre"))
            {
                mv.genres.push_back(genre.get<std::string>());
            }

            common.movies.push_back(mv);// Add movie to the list
        }

        if (common.movies.size() > 0)
            common.data_ready = true;// Indicate data is ready
    }
}

void DownloadThread::SetUrl(std::string_view new_url)
{
    _download_url = new_url;// Set the download URL
}

