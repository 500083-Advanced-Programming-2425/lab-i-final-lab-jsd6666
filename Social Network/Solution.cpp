#include <sstream>    
#include <fstream>      
#include <vector>       
#include <string>       
#include <unordered_map>
#include <set>         
#include <queue>       
#include <utility>      
#include <algorithm>    
#include <cmath>        
#include <iomanip>      
#include <iostream>     
#include <limits>       

#include "Solution.h" // include header file

// helper function trim whitespace from start and end of string
// sourced from https://stackoverflow.com/a/217605
std::string trim(const std::string& str)
{
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return ""; // String is all whitespace or empty
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// implement constructor
Solution::Solution() : _outFile("Output.txt")
{
    // populate country lookup for easy conversions
    countryLookup["DEU"] = "Germany";
    countryLookup["DNK"] = "Denmark";
    countryLookup["FIN"] = "Finland";
    countryLookup["FRA"] = "France";
    countryLookup["GBR"] = "United Kingdom";
    countryLookup["NLD"] = "Netherlands";
    countryLookup["NOR"] = "Norway";
    countryLookup["SWE"] = "Sweden";
}

// processCommand implementation
bool Solution::processCommand(const std::string& commandString) {
    _outFile << commandString << '\n';  // \n instead of std::endl

    std::stringstream ss(commandString);
    std::string commandWord;
    ss >> commandWord;

    bool commandSuccess = false;

    // command handling
    if (commandWord == "ViewProfile") {
        std::string userID, extra;
        ss >> userID >> extra;

        if (userID.empty() || !extra.empty()) {
            _outFile << "Error, extra parameter.\n";
            commandSuccess = false;
        }
        else {
            userID = trim(userID);
            auto const it = users.find(userID);
            if (it != users.end()) {
                const User& user = it->second;
                std::string countryName = "Unknown";
                auto const country_it = countryLookup.find(user.countryCode);
                if (country_it != countryLookup.end()) {
                    countryName = country_it->second;
                }
                int activityPercentage = static_cast<int>(std::round(user.activityRate * 100.0));
                size_t friendCount = user.friends.size();

                _outFile << "Name: " << user.fullName << '\n'
                    << "Age: " << user.age << '\n'
                    << "Country: " << countryName << '\n'
                    << "Activity Rate: " << activityPercentage << "%\n"
                    << "Friends: " << friendCount << '\n';
                commandSuccess = true;
            }
            else {
                _outFile << "Error\n";
                commandSuccess = false;
            }
        }
    }
    else if (commandWord == "ListFriends") {
        std::string userID;
        ss >> userID;
        userID = trim(userID);
        auto const it = users.find(userID);

        if (it != users.end()) {
            const User& user = it->second;
            size_t friendCount = user.friends.size();
            _outFile << friendCount << " friend(s) found.\n";
            for (const User* friendPtr : user.friends) {
                if (friendPtr != nullptr) {
                    _outFile << friendPtr->fullName << " [ID:" << friendPtr->id << "]\n";
                }
            }
            commandSuccess = true;
        }
        else {
            _outFile << "Error: User not found.\n";
            commandSuccess = false;
        }
    }
    else if (commandWord == "TotalUsers") {
        std::string countryCodeArgs;
        std::getline(ss, countryCodeArgs);
        countryCodeArgs = trim(countryCodeArgs);

        if (countryCodeArgs.empty()) {
            _outFile << users.size() << '\n';
            commandSuccess = true;
        }
        else {
            std::stringstream code_ss(countryCodeArgs);
            std::string singleCode;
            std::set<std::string> allowed_codes;
            while (code_ss >> singleCode) {
                allowed_codes.insert(trim(singleCode));
            }
            size_t count = 0;
            for (const auto& pair : users) {
                if (allowed_codes.count(pair.second.countryCode) > 0) {
                    count++;
                }
            }
            _outFile << count << '\n';
            commandSuccess = true;
        }
    }
    else if (commandWord == "ListMutuals") {
        std::string userID1, userID2;
        ss >> userID1 >> userID2;

        // check for missing arguments
        if (userID1.empty() || userID2.empty()) {
            _outFile << "Error: Missing arguments.\n";
            commandSuccess = false;
        }
        else {
            // check for extra parameters after userID1 and userID2
            std::string extraCheck;
            ss >> extraCheck;
            if (!extraCheck.empty()) {
                _outFile << "Error: Too many arguments.\n";
                commandSuccess = false;
            }
            else {
                userID1 = trim(userID1);
                userID2 = trim(userID2);

                auto it1 = users.find(userID1);
                auto it2 = users.find(userID2);

                if (it1 != users.end() && it2 != users.end()) {
                    const User& user1 = it1->second;
                    const User& user2 = it2->second;

                    std::vector<const User*> friends1;
                    for (const User* friendPtr : user1.friends)
                        if (friendPtr != nullptr) friends1.push_back(friendPtr);

                    std::vector<const User*> friends2;
                    for (const User* friendPtr : user2.friends)
                        if (friendPtr != nullptr) friends2.push_back(friendPtr);

                    std::sort(friends1.begin(), friends1.end());
                    std::sort(friends2.begin(), friends2.end());

                    std::vector<const User*> mutuals;
                    std::set_intersection(
                        friends1.begin(), friends1.end(),
                        friends2.begin(), friends2.end(),
                        std::back_inserter(mutuals)
                    );

                    _outFile << mutuals.size() << " mutual friend(s) found.\n";
                    for (const User* mutualPtr : mutuals)
                        _outFile << mutualPtr->fullName << " [ID:" << mutualPtr->id << "]\n";

                    commandSuccess = true;
                }
                else {
                    if (it1 == users.end()) _outFile << "Error: User " << userID1 << " not found.\n";
                    if (it2 == users.end()) _outFile << "Error: User " << userID2 << " not found.\n";
                    commandSuccess = false;
                }
            }
        }
    }

    else if (commandWord == "FindSeparation") {
        std::string userID1, userID2;
        ss >> userID1 >> userID2;

        // check for missing one user ID
        if (userID1.empty() || userID2.empty()) {
            _outFile << "Error: Missing one user ID.\n";
            commandSuccess = false;
        }
        else {
            userID1 = trim(userID1);
            userID2 = trim(userID2);

            auto const it1 = users.find(userID1);
            auto const it2 = users.find(userID2);

            if (it1 != users.end() && it2 != users.end()) {
                const User& startUser = it1->second;
                const User& endUser = it2->second;

                if (&startUser == &endUser) {
                    _outFile << "0 degree(s)\n";
                    commandSuccess = true;
                }
                else {
                    std::queue<std::pair<const User*, int>> q;
                    std::unordered_map<const User*, int> visited_distance;
                    int separation = -1;

                    q.push({ &startUser, 0 });
                    visited_distance[&startUser] = 0;

                    while (!q.empty() && separation == -1) { // early exit condition
                        const User* currentUserPtr = q.front().first;
                        int currentDist = q.front().second;
                        q.pop();

                        // early exit during friend iteration
                        for (const User* friendPtr : currentUserPtr->friends) {
                            // BFS OPTIMIZATION //
                            if (friendPtr == &endUser) {
                                separation = currentDist + 1;
                                while (!q.empty()) q.pop(); // Clear queue to exit immediately
                                break;
                            }
                            //  //

                            if (visited_distance.find(friendPtr) == visited_distance.end() && currentDist < 6) {
                                visited_distance[friendPtr] = currentDist + 1;
                                q.push({ friendPtr, currentDist + 1 });
                            }
                        }
                    }

                    if (separation != -1) {
                        _outFile << separation << " degree(s)\n";
                    }
                    else {
                        _outFile << "6 degree(s)\n";
                    }
                    commandSuccess = true;
                }
            }
            else {
                _outFile << "Error: User not found.\n";
                commandSuccess = false;
            }
        }
    }

    else if (commandWord == "FriendScore") {
        std::string userID1, userID2;
        ss >> userID1 >> userID2;

        // check for extra parameters
        std::string extraCheck;
        ss >> extraCheck;
        if (!extraCheck.empty()) {
            _outFile << "Error: Too many parameters.\n";
            commandSuccess = false;
        }
        else {
            userID1 = trim(userID1);
            userID2 = trim(userID2);

            auto const it1 = users.find(userID1);
            auto const it2 = users.find(userID2);

            if (it1 != users.end() && it2 != users.end()) {
                const User& user1 = it1->second;
                const User& user2 = it2->second;

                if (&user1 == &user2) {
                    _outFile << "Error: Cannot calculate FriendScore with self.\n";
                    commandSuccess = false;
                }
                else {
                    bool alreadyFriends = false;
                    for (const User* friendPtr : user1.friends) {
                        if (friendPtr == &user2) {
                            alreadyFriends = true;
                            break;
                        }
                    }

                    if (alreadyFriends) {
                        _outFile << std::fixed << std::setprecision(2) << 0.0 << '\n';
                        commandSuccess = true;
                    }
                    else {
                        std::set<const User*> friends1_set;
                        for (const User* friendPtr : user1.friends) {
                            if (friendPtr) friends1_set.insert(friendPtr);
                        }

                        int mutualCount = 0;
                        for (const User* friendPtr2 : user2.friends) {
                            if (friendPtr2 && friends1_set.count(friendPtr2)) {
                                mutualCount++;
                            }
                        }

                        double score = 0.0;
                        double denominator = user1.activityRate * user2.activityRate;
                        if (denominator > 1e-9) {
                            score = (mutualCount + 1.0) / denominator;
                        }
                        _outFile << std::fixed << std::setprecision(2) << score << '\n';
                        commandSuccess = true;
                    }
                }
            }
            else {
                _outFile << "Error: User not found.\n";
                commandSuccess = false;
            }
        }
    }

    else if (commandWord == "SuggestFriends") {
        std::string userID;
        ss >> userID;
        userID = trim(userID);

        auto const it = users.find(userID);
        if (it != users.end()) {
            const User& subjectUser = it->second;
            std::vector<const User*> currentFriends;

            // get subject's current friends 
            for (User* friendPtr : subjectUser.friends) {
                if (friendPtr) currentFriends.push_back(friendPtr);
            }

            // pre sort subject's friends 
            std::vector<const User*> subjectFriendsSorted = currentFriends;
            std::sort(subjectFriendsSorted.begin(), subjectFriendsSorted.end());

            // cache mutual counts for all candidates
            std::unordered_map<const User*, int> mutualCountCache;
            for (const auto& pair : users) {
                const User& candidate = pair.second;

                // skip self and direct friends
                if (&candidate == &subjectUser ||
                    std::find(currentFriends.begin(), currentFriends.end(), &candidate) != currentFriends.end()) {
                    continue;
                }

                // sort candidate's friends 
                std::vector<const User*> candidateFriendsSorted;
                for (User* friendPtr : candidate.friends) {
                    if (friendPtr) candidateFriendsSorted.push_back(friendPtr);
                }
                std::sort(candidateFriendsSorted.begin(), candidateFriendsSorted.end());

                // calculate mutuals w/ set_intersection
                std::vector<const User*> mutuals;
                std::set_intersection(
                    subjectFriendsSorted.begin(), subjectFriendsSorted.end(),
                    candidateFriendsSorted.begin(), candidateFriendsSorted.end(),
                    std::back_inserter(mutuals)
                );

                mutualCountCache[&candidate] = mutuals.size(); // Store in cache
            }

            // generate scores using cached counts
            std::vector<std::pair<double, const User*>> suggestions;
            for (const auto& entry : mutualCountCache) {
                const User* candidate = entry.first;
                int mutualCount = entry.second;

                // score calculation
                constexpr double MIN_DENOMINATOR = 1e-9;
                denominator = std::max(denominator, MIN_DENOMINATOR);
                double score = (mutualCount + 1.0) / denominator;

                suggestions.emplace_back(score, candidate);
            }

            // sort by score descending 
            std::sort(suggestions.begin(), suggestions.end(),
                [](const auto& a, const auto& b) {
                    return (a.first != b.first) ?
                        (a.first > b.first) :
                        (a.second->id < b.second->id);
                });

            // output top 5 using cached values
            _outFile << "Top 5 Suggestions:\n";
            for (int i = 0; i < std::min(5, (int)suggestions.size()); ++i) {
                const User* candidate = suggestions[i].second;
                _outFile << candidate->fullName << " [ID:" << candidate->id << "] - "
                    << mutualCountCache[candidate] << " mutual friend(s)\n";
            }
            commandSuccess = true;
        }
        else {
            _outFile << "Error: User not found.\n";
            commandSuccess = false;
        }
     }
    else {
        _outFile << "Error: Unrecognised command.\n";
    }

    _outFile << std::endl; // single flush per command
    return commandSuccess;
}

// buildNetwork implementation
bool Solution::buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships)
{
    std::ifstream finUsers(fileNameUsers);
    std::ifstream finFriendships(fileNameFriendships);

    // check files were opened successfully
    if (!finUsers.is_open()) {
        return false; // false for failure
    }
    if (!finFriendships.is_open()) {
        finUsers.close(); // close first file if second failed
        return false; // false for failure
    }

    std::string line;
    users.clear(); // clear existing user data

    while (std::getline(finUsers, line))
    {
        if (line.empty() || trim(line).empty())
        {
            continue;
        }

        std::stringstream ss(line);
        std::string id_str, fullName_str, age_str, countryCode_str, activityRate_str;

        if (std::getline(ss, id_str, ',') &&
            std::getline(ss, fullName_str, ',') &&
            std::getline(ss, age_str, ',') &&
            std::getline(ss, countryCode_str, ',') &&
            std::getline(ss, activityRate_str))
        {
            try
            {
                id_str = trim(id_str);
                countryCode_str = trim(countryCode_str);

                int age = std::stoi(trim(age_str));
                double activityRate = std::stod(trim(activityRate_str));

                std::string key_id = id_str;

                User newUser(std::move(id_str), std::move(fullName_str), age, std::move(countryCode_str), activityRate);
                users.emplace(std::move(key_id), std::move(newUser));
            }
            catch (const std::invalid_argument& ia)
            {
                // handle conversion error
                return false;
            }
            catch (const std::out_of_range& oor)
            {
                // handle out of range error
                return false;
            }
        }
        else {
            // handle csv line error
        }
    }

    // read Friendships.csv
    while (std::getline(finFriendships, line))
    {
        if (line.empty() || trim(line).empty())
        {
            continue;
        }

        std::stringstream ss(line);
        std::string userId1_str, userId2_str;

        if (std::getline(ss, userId1_str, ',') &&
            std::getline(ss, userId2_str))
        {
            userId1_str = trim(userId1_str);
            userId2_str = trim(userId2_str);

            auto it1 = users.find(userId1_str);
            auto it2 = users.find(userId2_str);

            if (it1 != users.end() && it2 != users.end())
            {
                User& user1_ref = it1->second;
                User& user2_ref = it2->second;

                user1_ref.friends.push_back(&user2_ref);
                user2_ref.friends.push_back(&user1_ref);
            }
            else
            {
                // handle missing user id in friendship
            }
        }
        else
        {
            // handle csv line error
        }
    }

    finUsers.close();
    finFriendships.close();

    return true;
}