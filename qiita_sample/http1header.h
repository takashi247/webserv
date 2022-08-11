#ifndef _HTTP1_HEAD_H_
#define _HTTP1_HEAD_H_

#include <vector>
#include <string>
#include <map>
#include <sstream>


class HTTP1_Header {
public:
    static std::vector<std::string>& make_response404();
    static std::vector<std::string>& make_response200(int body_length);
    static std::vector<std::string>& make_response200img(int body_length);
    static std::vector<std::string>& make_response302(std::string path);
    static std::vector<std::string>& make_responseUpgrade();
};

#endif