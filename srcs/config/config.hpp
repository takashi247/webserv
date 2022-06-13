#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <sstream>
#include <vector>
// class ConfigError
// {
//  public:
// 	int line;
// 	virtual void MakeUnexpected(std::string error_msg);

// };

void MakeUnexpected(std::string msg);
int ParseInt(std::stringstream &ss, int max);
bool ParseBool(std::stringstream &ss);
std::string ParseString(std::stringstream &ss);
std::vector<std::string> ParseVector(std::stringstream &ss);

#endif
