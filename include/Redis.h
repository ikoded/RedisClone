#pragma once
#include <unordered_map> // hashmap that stores data
#include <variant>
#include <string>
#include <vector>
#include <iostream>

/*

Redis values constants
(will be implemented in future, for now string only)

*/
// using redis_string = std::string;
// using redis_list = std::vector<std::string>;
// using redis_map = std::unordered_map<std::string, std::string>;
// // this is a variant meaning it can be any of these values
// using redis_value = std::variant<redis_string,redis_list,redis_map>;

class Redis{
    public:
        // Helper functions for Redis operations
        std::string get_redis_value(std::string key, std::string field);
        void set_redis_value(std::string key, std::string field, std::string value);
        void delete_user_or_field(std::string key, std::string field);
        // Used to parse the query into preformatted vector
        static std::vector<std::string> parse_query(std::string query);
        // Process the query when it comes in
        void process_query(std::vector<std::string> query_vector);
        // Meant for debugging, prints full map
        void print_dict();
        // Initialize Redis map from persistent storage
        Redis();
    private:
        std::unordered_map<std::string,std::unordered_map<std::string,std::string>> redis_dict;
};