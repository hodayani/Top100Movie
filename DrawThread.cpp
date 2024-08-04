#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

// Placeholder for loading images- no userd
//ImTextureID LoadTexture(const std::string& path);

void DrawAppWindow(void* common_ptr)
{
    auto* common = static_cast<CommonObjects*>(common_ptr);
    ImGui::Begin("import movie data:");//first row output

    // Search Bar
    static char search_query[200];
    ImGui::InputText("Search", search_query, sizeof(search_query));

    // Favorite and Recently Viewed Buttons (side by side)
    static bool show_favorites = false;
    if (ImGui::Button("Show Favorites"))
    {
        // Toggle the boolean flag to show/hide favorite movies
        show_favorites = !show_favorites;
    }
    ImGui::SameLine();//in order to make sure it will be the same row

    static bool show_recently_viewed = false;
    if (ImGui::Button("Show Recently Viewed"))
    {
        // Toggle the boolean flag to show/hide recently viewed movies
        show_recently_viewed = !show_recently_viewed;
    }

    // Display Movies Grid if Data is Ready
    static bool show_data = false;
    static int selected_movie_index = -1;
    static bool showDescription_modal = false;
    static bool showGenre_modal = false;

    if (common->data_ready)
    {
        show_data = true;
        common->data_ready = false;
    }

    if (show_data)
    {
        // Filter movies based on the search query
        std::vector<movie> filtered_movies;
        if (show_favorites)
        {
            // If 'Show Favorites' is enabled, use the favorites list
            filtered_movies = common->favorites;
        }
        else if (show_recently_viewed)
        {
            // If 'Show Recently Viewed' is enabled, use the recently viewed list
            filtered_movies = common->recently_viewed;
        }
        else
        {// Otherwise, filter the full list of movies based on the search query
            for (const auto& movie : common->movies)
            {
                // If the search query is empty or the movie title contains the query, add to filtered list
                if (strlen(search_query) == 0 || movie.title.find(search_query) != std::string::npos)
                {
                    filtered_movies.push_back(movie);
                }
            }
        }

        // Set the number of columns and item size for the grid
        int columns = 4;  // Number of columns
        int itemSize = 200;  // Size of each movie item (width and height)

        if (ImGui::BeginTable("MoviesGrid", columns, ImGuiTableFlags_Borders))
        {
            for (int i = 0; i < filtered_movies.size(); ++i)
            {
                const auto& movie = filtered_movies[i];

                if (i % columns == 0)
                {
                    ImGui::TableNextRow();
                }

                ImGui::TableSetColumnIndex(i % columns);
                ImGui::BeginGroup();

                // Display the movie details
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "\t \t \t \t \t \t \t %d", movie.rank); // White text
                ImGui::Text("Title: %s", movie.title.c_str());
                ImGui::Text("Rating: %s", movie.rating.c_str());
                ImGui::Text("ID: %s", movie.id.c_str());
                ImGui::Text("Year: %d", movie.year);

                // Button for genres
                if (ImGui::Button(("Show Genre##" + std::to_string(i)).c_str()))
                {
                    selected_movie_index = i;
                    showGenre_modal = true;
                }

                // Button for description
                if (ImGui::Button(("Show Description##" + std::to_string(i)).c_str()))
                {
                    selected_movie_index = i;
                    showDescription_modal = true;
                }

                // Add to Favorite button
                if (ImGui::Button(("Add to Favorite##" + std::to_string(i)).c_str()))
                {
                    auto it = common->favorites.begin();  // Iterator to keep track of the current position in the list
                    bool is_favorite = false;

                    // Find if the movie is already in the favorites list
                    for (; it != common->favorites.end(); ++it)
                    {
                        if (it->id == movie.id)
                        {
                            is_favorite = true;  // Movie is already in the favorites list
                            break;
                        }
                    }

                    if (is_favorite)
                    {
                        // Remove the movie from the favorites list
                        common->favorites.erase(it);
                    }
                    else
                    {
                        // Add the movie to the favorites list
                        common->favorites.push_back(movie);
                    }
                }


                // Add to Recently Viewed button
                if (ImGui::Button(("Add to Recently Viewed##" + std::to_string(i)).c_str()))
                {
                    auto it = common->recently_viewed.begin();  // Iterator to keep track of the current position in the list
                    bool is_recently_viewed = false;

                    // Find if the movie is already in the recently viewed list
                    for (; it != common->recently_viewed.end(); ++it)
                    {
                        if (it->id == movie.id)
                        {
                            is_recently_viewed = true;  // Movie is already in the recently viewed list
                            break;
                        }
                    }

                    if (is_recently_viewed)
                    {
                        // Remove the movie from the recently viewed list
                        common->recently_viewed.erase(it);
                    }
                    else
                    {
                        // Add the movie to the recently viewed list
                        common->recently_viewed.push_back(movie);
                    }
                }


                // User Rating section
                ImGui::Text("Your Rating:");
                for (int star = 1; star <= 5; ++star)
                {
                    if (star <= movie.user_rating)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow star
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Grey star
                    }
                    if (ImGui::Button((std::to_string(star) + "##" + std::to_string(i)).c_str()))
                    {
                        // Update user rating in both filtered_movies and common->movies
                        filtered_movies[i].user_rating = star;
                        for (auto& original_movie : common->movies)
                        {
                            if (original_movie.id == movie.id)
                            {
                                original_movie.user_rating = star;
                                break;
                            }
                        }
                    }
                    ImGui::PopStyleColor();
                    if (star < 5)
                        ImGui::SameLine();
                }

                ImGui::EndGroup();
            }
            ImGui::EndTable();
        }
    }

    // Show Movie Description Details in a Modal
    if (showDescription_modal && selected_movie_index != -1)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once); // Set initial size
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 400), ImVec2(800, 400)); // Enforce size constraints
        ImGui::OpenPopup("Movie Details");
        showDescription_modal = false;
    }

    if (ImGui::BeginPopupModal("Movie Details", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        ImVec2 content_region = ImGui::GetContentRegionAvail();// Get the available space in the modal
        ImGui::SetCursorPos(ImVec2(0, 0));// Set cursor to the top left
        ImGui::PushItemWidth(content_region.x); // Adjust width for the content
        if (selected_movie_index != -1)
        {
            auto& movie = common->movies[selected_movie_index];// Get the selected movie
            //display details :
            ImGui::Text("Title: %s", movie.title.c_str());

            ImGui::Separator();//add a sepearte line
            ImGui::TextWrapped("Description: %s", movie.description.c_str());

            ImGui::Separator();//add a sepearte line
            ImGui::Text("Rating: %s", movie.rating.c_str());

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                selected_movie_index = -1;
            }
        }
        ImGui::PopItemWidth();
        ImGui::EndPopup();
    }

    // Show Movie Genre in a Modal
    if (showGenre_modal && selected_movie_index != -1)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once); // Set initial size
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 400), ImVec2(800, 400)); // Enforce size constraints
        ImGui::OpenPopup("Genre Details");
        showGenre_modal = false;  // Reset the flag to prevent reopening
    }

    if (ImGui::BeginPopupModal("Genre Details", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        ImVec2 content_region = ImGui::GetContentRegionAvail();// Get the available space in the modal
        ImGui::SetCursorPos(ImVec2(0, 0));// Set cursor to the top left
        ImGui::PushItemWidth(content_region.x); // Adjust width for the content
        if (selected_movie_index != -1)
        {
            auto& movie = common->movies[selected_movie_index];// Get the selected movie
            //display details :

            ImGui::Text("Title: %s", movie.title.c_str());

            ImGui::Separator();// Add a separator line

            // Display genres as a numbered list
            ImGui::Text("Genres:");
            for (size_t i = 0; i < movie.genres.size(); ++i)
            {
                ImGui::Text("%zu. %s", i + 1, movie.genres[i].c_str());
            }

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                selected_movie_index = -1;
            }
        }
        ImGui::PopItemWidth();// Reset the width adjustment
        ImGui::EndPopup();// End the popup modal
    }

    ImGui::End();// End the main ImGui window
}

void DrawThread::operator()(CommonObjects& common)
{
    GuiMain(DrawAppWindow, &common);// Run the GUI main loop
    common.exit_flag = true;// Set the exit flag to true
}
/*
// Placeholder function for loading texture
ImTextureID LoadTexture(const std::string& path) {
    // Implement your texture loading function here
    return nullptr;
}
*/