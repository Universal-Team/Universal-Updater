#ifndef PARSE_HPP
#define PARSE_HPP

void parse(std::string fileName);
std::string get(nlohmann::json json, const std::string &key, const std::string &key2);

#endif