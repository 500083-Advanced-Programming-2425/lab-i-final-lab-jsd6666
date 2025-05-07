#pragma once

#include <fstream>
#include <string>
#include <vector>          
#include <unordered_map>   

// forward declaration needed 
struct User;

// represents single user in the network
struct User {
    std::string id;             // 8char hexadecimal unique identifier
    std::string fullName;       // user full name (max 128 chars)
    int age;                    // user age (18-80)
    std::string countryCode;    // 3-letter country code
    double activityRate;        // activity rate (0.0 to 1.0)

    std::vector<User*> friends;

    // default constructor 
    User() : age(0), activityRate(0.0) {}

    // parameter constructor
    User(std::string uId, std::string name, int uAge, std::string cCode, double rate)
        : id(std::move(uId)), fullName(std::move(name)), age(uAge),
        countryCode(std::move(cCode)), activityRate(rate) {}
};


class Solution final {
    // output file stream 
    std::ofstream _outFile;

private:
    // container storing all users.
    // map user unique id to user object.
    std::unordered_map<std::string, User> users;

    // helper to convert country codes to country names.
    // will be used for ViewProfile command output.
    std::unordered_map<std::string, std::string> countryLookup;


    // any private helper methods
private:


public:
    // constructor initializes object
    Solution();

    bool buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships);

    bool processCommand(const std::string& commandString);


    // public helper methods
public:

};