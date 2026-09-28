#include "../include/Redis.h"

redis_value Redis::get_redis_value(std::string key){
    if(redis_dict.count(key)){
        auto& redis_type = redis_dict[key];

        if(std::holds_alternative<redis_string>(redis_type)){ // check if string
            auto& str = std::get<redis_string>(redis_type);
            return str;
        }else if(std::holds_alternative<redis_list>(redis_type)){ // check if list
            auto& list = std::get<redis_list>(redis_type);
            return list;
        }else if(std::holds_alternative<redis_map>(redis_type)){ // check if map
            auto& map = std::get<redis_map>(redis_type);
            return map;
        }
    }
    return ""; // failure, value cannot be ""
}

void Redis::set_redis_value(std::string key, redis_value value){
    redis_dict[key] = value;
}

/*

Useful for parsing the query

*/
std::vector<std::string> Redis::parse_query(std::string query){
    std::vector<std::string> words_parsed;
    size_t pos;
    // Grab set, get, delete
    if(query.substr(0,3)=="GET"){
        words_parsed.push_back("GET");
        query.erase(0,4); // erase space too
    }else if(query.substr(0,3)=="SET"){
        words_parsed.push_back("SET");
        query.erase(0,4);
    }else if(query.substr(0,6)=="DELETE"){
        words_parsed.push_back("DELETE");
        query.erase(0,7);
    }else{
        return words_parsed; // return empty to tell caller it failed
    }

    while((pos = query.find(":")) != std::string::npos){ // returns index of first space
        std::string word = query.substr(0, pos);
        words_parsed.push_back(word);

        // pos + 1 because pos is index
        // erase needs n characters from 0 to erase, so pos + space (index+1 to get n characters)
        query.erase(0, pos + 1);
    }
    // end of query
    words_parsed.push_back(query);

    return words_parsed;
}

void process_query(std::vector<std::string> query_vector){
    std::string keyword = query_vector.front();

    if(keyword=="GET"){
        // will send to browser in JSON response
    }else if(keyword=="SET"){

    }else if(keyword=="DELETE"){

    }else{
        // shouldn't ever get here
        return;
    }
}

Redis::Redis(){
    /*
    
    Initializes Redis Dict When Program Starts

    */
}