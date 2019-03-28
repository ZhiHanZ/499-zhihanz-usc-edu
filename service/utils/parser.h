#ifndef SERVICE_UTILS_PARSER_H_
#define SERVICE_UTILS_PARSER_H_
#include <vector>
#include <string>
#include <sstream>
namespace services{
namespace parser {
template <typename T>
// parsing a vector into a string delimiter is "\n"
T Parser(const std::vector<T> &vecs) {
  std::string parse{};
  for (const auto &vec : vecs) {
    parse += vec;
    parse += "\n";
  }
  return parse;
}
template <typename T>
// Deparse a strint into a vector
auto Deparser(const T &string) {
  std::istringstream ss(string);
  std::string token;
  std::string delimiter{"\n"};
  std::vector<T> vec;
  while (std::getline(ss, token, '\n')) {
    vec.push_back(token);
  }
  return vec;
}
}  // namespace parser
} // namespace service

#endif //  SERVICE_UTILS_PARSER_H_
