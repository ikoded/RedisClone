#include "../include/Redis.h"

// needs fixed
// std::string Redis::get_redis_value(std::string key){
//     if(redis_dict.count(key)){
//         auto& redis_type = redis_dict[key];

//         if(std::holds_alternative<redis_string>(redis_type)){ // check if string
//             auto& str = std::get<redis_string>(redis_type);
//             return str;
//         }else if(std::holds_alternative<redis_list>(redis_type)){ // check if list
//             auto& list = std::get<redis_list>(redis_type);
//             return list;
//         }else if(std::holds_alternative<redis_map>(redis_type)){ // check if map
//             auto& map = std::get<redis_map>(redis_type);
//             return map;
//         }
//     }
//     return ""; // failure, value cannot be ""
// }

// void Redis::set_redis_value(std::string key, std::string value){
//     redis_dict[key] = value;
// }

/*

Used to parse the query into preformatted vector

Supported format is as follows:

Get:
GET user:{id} {field}

Set:
SET user:{id} {field} {value}

Delete:
DELETE user:{id} (optional){field}

*/
std::vector<std::string> Redis::parse_query(std::string query){
    std::vector<std::string> words_parsed;
    size_t pos;
    auto nposcheck = [](int x){
        if(x==std::string::npos) return 1;
        return 0;
    };
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

    pos = query.find(":");
    if(nposcheck(pos)==1) return std::vector<std::string>{}; // makes sure pos did not fail
    // grab user portion, future proof way in case change
    words_parsed.push_back(query.substr(0,pos));
    query.erase(0,pos+1);

    if(words_parsed.front()=="DELETE" && query.find(" ") == std::string::npos){
        words_parsed.push_back(query);
        // leave because delete supports deleting user just by id (GET will in future)
        return words_parsed;
    }else{
        pos = query.find(" ");
        if(nposcheck(pos)==1) return std::vector<std::string>{};
        // grab id
        words_parsed.push_back(query.substr(0,pos));
        query.erase(0,pos+1);
    }
    

    // for now just use strings
    // GET still has field left (in future may have nothing to support grab all user fields)
    // SET still has field/value left
    // DELETE still has either nothing OR field left (can delete user or field of user)
    if(words_parsed.front() == "SET"){
        pos = query.find(" ");
        if(nposcheck(pos)==1) return std::vector<std::string>{}; // fails because a value must be set
        // grab the field and value to set to
        std::string field = query.substr(0,pos);
        std::string value = query.substr(pos+1,query.length());
        words_parsed.push_back(field);
        words_parsed.push_back(value);
    }else if(words_parsed.front() == "DELETE"){
        if(query.length()>0){ // field is left
            words_parsed.push_back(query);
        }
    }else{
        // end of query for GET
        words_parsed.push_back(query);
    }

    return words_parsed;
}

void Redis::process_query(std::vector<std::string> query_vector){
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