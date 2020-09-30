#ifndef RESP_HEADERS_HPP_
#define RESP_HEADERS_HPP_

#include <string>
#include <vector>
#include <initializer_list>

namespace resp {

const std::string CRLF = "\r\n";

class SimpleString {
public:
    SimpleString(): type_("+"), value_(CRLF) {}
    explicit SimpleString(const std::string& value): type_("+"), value_(value + CRLF) {}
    explicit SimpleString(std::string&& value): type_("+"), value_(std::move(value + CRLF)) {}
public:
    std::string get_value() {
        std::string value;
        value = type_ + value_;
        return value;
    }
private:
    std::string type_;
    std::string value_;
};

class Error {
public:
    Error(): type_("-"), value_(CRLF) {}
    explicit Error(const std::string& value): type_("-"), value_(value + CRLF) {}
    explicit Error(std::string&& value): type_("-"), value_(std::move(value + CRLF)) {}
public:
    std::string get_value() {
        std::string value;
		value = type_ + value_;
		return value;
	}
private:
	std::string type_;
	std::string value_;
};

class Integer {
public:
    Integer(): type_(":"), value_(CRLF) {}
    explicit Integer(int64_t value): type_(":"), value_(std::to_string(value) + CRLF) {} 
public:

private:
    std::string type_;
    std::string value_;
};

} // end of namespace resp

#endif // RESP_HEADERS_HPP_
