#include "../include/Redis.h"

/*

Helper functions for Redis operations

*/
std::string_view Redis::get_redis_value(std::string_view key, std::string_view field){
    auto entry = redis_dict.find(std::string(key));
    if(entry != redis_dict.end()){
        auto field_entry = entry->second.find(std::string(field));
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

void Redis::set_redis_value(std::string_view key, std::string_view field, std::string_view value){
    std::unordered_map<std::string,std::string> dict = redis_dict[std::string(key)];
    
    // set dict in  to field/value
    dict[std::string(field)] = value;
    redis_dict[std::string(key)] = dict;

    std::cout << "Set " << key << " field " << field << " to " << value << std::endl;
}

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

NPOS check helper

*/
auto nposcheck = [](int x){
    if(x==std::string_view::npos) return 1;
    return 0;
};

/*

Content length helper

*/

int get_content_length(std::string_view content_length_line){
    int line_length = content_length_line.length();
    
    std::string content_length_string(content_length_line);
    // content length 17 because "content-length: "
    int content_length = std::stoi(content_length_string.substr(16));
    return content_length;
}

/*

Parse query helpers

*/

/*

GET request helper

Example:
    GET /user/100/name HTTP/1.1

*/
std::vector<std::string_view> process_get_request(std::string_view get_request){
    ssize_t pos;
    ssize_t prev_pos = 5;
    std::vector<std::string_view> get_parsed;
    // reserve memory up front to save
    get_parsed.reserve(4);

    get_parsed.push_back("GET");
    // check from pos 4 because index 4 should be first '/', so skip that
    pos = get_request.find("/",prev_pos); if(nposcheck(pos)==1) return {}; // makes sure pos did not fail
    // grab user first
    // subtracting to get n characters to take
    get_parsed.push_back(get_request.substr(prev_pos,pos - prev_pos));
    // add 1 to go to 1 char past the second slash
    prev_pos = pos + 1;
    // check for user id
    pos = get_request.find("/",prev_pos); if(nposcheck(pos)==1) return {};

    get_parsed.push_back(get_request.substr(prev_pos,pos - prev_pos));
    prev_pos = pos + 1;
    // check for field
    pos = get_request.find(" ",prev_pos); if(nposcheck(pos)==1) return {};

    get_parsed.push_back(get_request.substr(prev_pos,pos - prev_pos));

    return get_parsed;
}

/*

PUT request helper

Example:
    PUT /user/100/name HTTP/1.1
    body example: {"value":"Max"}

*/
std::vector<std::string_view> process_put_request(std::string_view set_request, std::string_view value){
    ssize_t pos;
    ssize_t prev_pos = 5;
    std::vector<std::string_view> get_parsed;
    get_parsed.reserve(5);

    get_parsed.push_back("PUT");
    pos = set_request.find("/",prev_pos); if(nposcheck(pos) == 1) return {};

    // get the first word
    get_parsed.push_back(set_request.substr(prev_pos, pos-prev_pos));
    prev_pos = pos + 1;

    pos = set_request.find("/",prev_pos); if(nposcheck(pos) == 1) return {};
    get_parsed.push_back(set_request.substr(prev_pos, pos - prev_pos));
    prev_pos = pos + 1;

    pos = set_request.find(" ", prev_pos); if(nposcheck(pos) == 1) return {};
    get_parsed.push_back(set_request.substr(prev_pos, pos-prev_pos));

    get_parsed.push_back(value);
    
    return get_parsed;
}

std::vector<std::string_view> process_delete_request(std::string_view delete_request){
    return {};
}

/*

Used to parse the query into preformatted vector

Supported format is as follows:

Get:
GET user:{id} {field}

Put:
PUT user:{id} {field} {value}

Delete:
DELETE user:{id} (optional){field}

*/
std::vector<std::string_view> Redis::parse_query(std::string_view query){
    std::vector<std::string_view> words_parsed;
    // keeping each line in a vector for now in case of logging or other features
    size_t pos;
    

    // first line is `GET /user/100/name HTTP/1.1` which is all I care about for now
    pos = query.find('\n'); if(nposcheck(pos) == 1) return {};
    std::string_view request = query.substr(0,pos);

    // get method requested
    pos = request.find(" "); if(nposcheck(pos) == 1) return {};
    std::string_view method = request.substr(0,pos);

    // get words_parsed for process_query function
    if(method=="GET"){
        if((words_parsed = process_get_request(request)).empty()) return {};
    }else if(method=="PUT"){
        // need to get content length and value
        pos = query.find("content-length: "); if(nposcheck(pos) == 1) return {};
        int length_line_pos = pos;
        pos = query.find('\n',pos); if(nposcheck(pos) == 1) return {};
        std::string_view content_length_line = query.substr(length_line_pos, pos);
        int content_length = get_content_length(content_length_line);

        pos = query.find("{\"value\":\"");
        if (nposcheck(pos) == 1) return {};
        // + 10 to get past and -12 to get rid of `{"value":""}` chars
        std::string_view value = query.substr(pos+10,content_length-12);
        
        if((words_parsed = process_put_request(request,value)).empty()) return {};
    }else if(method=="DELETE"){
        words_parsed.push_back("DELETE");
    }
    return words_parsed;
}

/*

Process the query when it comes in

*/
void Redis::process_query(std::vector<std::string_view> query_vector){
    std::string_view keyword = query_vector.front();

    if(keyword=="GET"){
        // will send to browser in JSON response
        // grab key and field (backwards in vector)
        std::string_view field = query_vector.back(); query_vector.pop_back();

        std::string_view id = query_vector.back(); query_vector.pop_back();
        std::string key(query_vector.back()); key += ":"; key += id; query_vector.pop_back();

        // get keys unordered map
        std::string_view return_value = get_redis_value(std::string_view(key),field);
        if(return_value!=""){
            std::cout << "Found: " << key << "(" << field << ") = " << return_value << std::endl; 
        }

    }else if(keyword=="PUT"){
        // need the key, field, and value (backwards)
        std::string_view value = query_vector.back(); query_vector.pop_back();

        std::string_view field = query_vector.back(); query_vector.pop_back();

        std::string_view id = query_vector.back(); query_vector.pop_back();
        std::string key(query_vector.back()); key += ":"; key += id; query_vector.pop_back();

        // set key/field/value
        set_redis_value(std::string_view(key),field,value);
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

/*

Initialize Redis map from persistent storage

*/
Redis::Redis(){
    
}