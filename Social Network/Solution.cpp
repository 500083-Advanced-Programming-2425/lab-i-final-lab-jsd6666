#include <sstream>  // parsing command strings
#include <fstream>  
#include <vector>  
#include <string>   
#include <unordered_map> 

#include "Solution.h" // include header file

// implement constructor
Solution::Solution() : _outFile("Output.txt") { // initialize _outFile using 
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
    // echo command to output file first
    _outFile << commandString << std::endl;

    std::istringstream inString(commandString);
    std::string command;
    inString >> command; // read first word as command

    // cmd procssing logic here

    // handle unknown cmd / error
    _outFile << "Error: Unrecognised command." << std::endl; 
    _outFile << std::endl; 
    return false; // false for failure
    // }


    // placeholder return to be replaced
    // utputting an error for any command for now
    _outFile << "Error: Command processing not implemented." << std::endl;
    _outFile << std::endl; 
    return false;
}

// buildNetwork implementation
bool Solution::buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships) {
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

    // logic to read Users.csv will go here
    // read line by line, parse csv, create user objects, add to users map


    // logic to read Friendships.csv will go here
    // read line by line, parse ids, find user objects in users map,
    // add pointers to each others 'friends' vector


    // close files when done
    finUsers.close();
    finFriendships.close();

    return true;
}
