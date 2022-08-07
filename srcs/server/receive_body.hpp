#ifndef RECEIVE_BODY_HPP
#define RECEIVE_BODY_HPP

#include <string>

int ParseNormalBody(size_t &remain_size, std::string &in, size_t &pos,
                    std::string &out);
int ParseChunkedBody(size_t &remain_size, std::string &in, size_t &pos,
                     std::string &out);

#endif
