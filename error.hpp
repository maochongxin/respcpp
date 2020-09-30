#ifndef RESP_ERROR_HPP_
#define RESP_ERROR_HPP_

#include <iostream>
#include <string>

namespace errors {

class Error : public std::exception {
public:
    std::string where_, what_, detail_;

    Error(std::string&& where,
        std::string&& what,
        std::string&& detail): where_(where),
                            what_(what),
                            detail_(detail) {}
    friend std::ostream& operator<<(std::ostream& out, Error& error) {
        return out << error.to_string() << std::endl; 
    }
    std::string to_string() {
        return where_ + ": " + what_ + ", detail: " + detail_;
    }
};


}


#endif