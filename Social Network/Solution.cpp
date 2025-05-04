#include <sstream>  // parsing command strings
#include <fstream>  
#include <vector>  
#include <string>   
#include <unordered_map> 
#include <iostream>

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
bool Solution::processCommand(const std::string& commandString)
{
    // echo command to output file first
    _outFile << commandString << std::endl;

    std::istringstream inString(commandString);
    std::string command;
    inString >> command; // read first word as command

    // TODO: add actual command handling logic here

    // handle unknown cmd / error
    _outFile << "Error: Unrecognised command." << std::endl;
    _outFile << std::endl;
    return false; // false for failure
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

    std::string line, segment;
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
            }
            catch (const std::out_of_range& oor)
            {
                // handle out of range error
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
