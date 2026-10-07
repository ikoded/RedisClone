#include "../include/Redis.h"

// Getters and Setters
std::string Redis::get_redis_value(std::string key, std::string field){
    auto entry = redis_dict.find(key);
    if(entry != redis_dict.end()){
        auto field_entry = entry->second.find(field);
        if(field_entry != entry->second.end()){
            return field_entry->second;
        }else{
            std::cerr << "Could not find " << key << " field " << field << std::endl;
        }
    }else{
        std::cerr << "Could not find " << key << std::endl;
    }
    return "";
    
    // Code for when using redis value
    // if(redis_dict.count(key)){
    //     auto& redis_type = redis_dict[key];

    //     if(std::holds_alternative<redis_string>(redis_type)){ // check if string
    //         auto& str = std::get<redis_string>(redis_type);
    //         return str;
    //     }else if(std::holds_alternative<redis_list>(redis_type)){ // check if list
    //         auto& list = std::get<redis_list>(redis_type);
    //         return list;
    //     }else if(std::holds_alternative<redis_map>(redis_type)){ // check if map
    //         auto& map = std::get<redis_map>(redis_type);
    //         return map;
    //     }
    // }
    // return ""; // failure, value cannot be ""
}

void Redis::set_redis_value(std::string key, std::string field, std::string value){
    std::unordered_map<std::string,std::string> dict = redis_dict[key];
    
    // set dict in  to field/value
    dict[field] = value;
    redis_dict[key] = dict;

    std::cout << "Set " << key << " field " << field << " to " << value << std::endl;
}

// Helper functions for Redis operations

void Redis::delete_user_or_field(std::string key, std::string field = ""){
    auto entry = redis_dict.find(key);
    if(entry != redis_dict.end()){
        if(field==""){
            redis_dict.erase(key);
            std::cout << "Deleted key " << key << std::endl;
        }else{
            auto field_entry = entry->second.find(field);
            if(field_entry != entry->second.end()){
                redis_dict[key].erase(field);
                std::cout << "Deleted field (" << field << ") from key (" << key << ")" << std::endl;
            }else{
                std::cerr << "Could not find key (" << key << ") field (" << field << ")" << std::endl;
            }
        }
    }else{
        std::cerr << "Could not find key " << key << std::endl;
    }
}

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

    // pop the trailing newline (for js page this needs removed)
    query.pop_back();

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
        // grab key and field (backwards in vector)
        std::string field = query_vector.back(); query_vector.pop_back();

        std::string id = query_vector.back(); query_vector.pop_back();
        std::string key = query_vector.back() + ":" + id; query_vector.pop_back();

        // get keys unordered map
        std::string return_value = get_redis_value(key,field);
        if(return_value!=""){
            std::cout << "Found: " << key << "(" << field << ") = " << return_value << std::endl; 
        }

    }else if(keyword=="SET"){
        // need the key, field, and value (backwards)
        std::string value = query_vector.back(); query_vector.pop_back();

        std::string field = query_vector.back(); query_vector.pop_back();

        std::string id = query_vector.back(); query_vector.pop_back();
        std::string key = query_vector.back() + ":" + id; query_vector.pop_back();

        // set key/field/value
        set_redis_value(key,field,value);
    }else if(keyword=="DELETE"){
        if(query_vector.size()==3){ // delete the user
            std::string id = query_vector.back(); query_vector.pop_back();
            std::string key = query_vector.back() + ":" + id;
            // delete just the user
            delete_user_or_field(key);
        }else if(query_vector.size()==4){ // delete users field
            std::string field = query_vector.back(); query_vector.pop_back();
            std::string id = query_vector.back(); query_vector.pop_back();
            std::string key = query_vector.back() + ":" + id;
            // delete users field
            delete_user_or_field(key,field);
        }
    }
}

/*

Meant for debugging, prints full map

*/
void Redis::print_dict(){
    int outercount = 1;
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>> full_dict = redis_dict;
    std::cout << "{" << std::endl;
    for(const auto& [key,value] : full_dict){

        std::cout << "\t" << key << ": {" << std::endl;
        for(const auto& [field,fvalue] : value){
            std::cout << "\t\t" << field << ": " << fvalue << std::endl;
        }
        std::cout << "\t}";
        if(full_dict.size()!=1 && outercount!=full_dict.size()){
            std::cout << ",\n";
        }
        std::cout << std::endl;

        outercount++;
    }
    std::cout << "}" << std::endl;
}

Redis::Redis(){
    /*
    
    Initializes Redis Dict When Program Starts

    */
}