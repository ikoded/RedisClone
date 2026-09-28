#pragma once
#include <unordered_map> // hashmap that stores data
#include <variant>
#include <string>
#include <vector>

/*

Redis values constants

*/
using redis_string = std::string;
using redis_list = std::vector<std::string>;
using redis_map = std::unordered_map<std::string, std::string>;
// this is a variant meaning it can be any of these values
using redis_value = std::variant<redis_string,redis_list,redis_map>;

class Redis{
    public:
        // getters & setters
        redis_value get_redis_value(std::string key);
        void set_redis_value(std::string key, redis_value value);

        static std::vector<std::string> parse_query(std::string query);
        void process_query(std::vector<std::string> query_vector);

        Redis(); // Initializes dict from storage
    private:
        std::unordered_map<std::string,redis_value> redis_dict;
};