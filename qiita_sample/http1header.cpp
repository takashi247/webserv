#include "http1header.h"
#include <iostream>
std::vector<std::string>& HTTP1_Header::make_response404() {
    static std::vector<std::string> header ={
        "HTTP/1.1 404 Not Found\r\n",
        "Content-Type: text/html; charset=UTF-8\r\n",
        "Connection: close\r\n",
        "\r\n",
        "<html><body><h1>404 Not found</h1><p>The requested URL was not found on this server.</p><hr><address>Original Server</address></body></html>\r\n"
    };

    return header;
}

std::vector<std::string>& HTTP1_Header::make_response200(int body_length) {
    std::ostringstream oss;
    oss << "Content-Length: " << body_length << "\r\n";
    static std::vector<std::string> header = {
        "HTTP/1.1 200 OK\r\n",
        "Content-Type: text/html; charset=UTF-8\r\n",
        oss.str(),
        "Connection: Keep-Alive\r\n",
        "\r\n"
    };
    return header;
}

std::vector<std::string>& HTTP1_Header::make_response302(std::string path) {
    std::ostringstream oss;
    if (path != "") {
        oss << "Location: https://127.0.0.1:5001" << path.c_str() << "\r\n";
    }
    else {
        oss << "Location: https://127.0.0.1:5001\r\n";
    }
    static std::vector<std::string> header = {
        "HTTP/1.1 302 Found\r\n",
        oss.str(),
        "\r\n"
    };
    return header;
}

std::vector<std::string>& HTTP1_Header::make_responseUpgrade() {
    static std::vector<std::string> header = {
        "HTTP/1.1 101 Switching Protocols\r\n",
        "Connection: Upgrade\r\n",
        "Upgrade: h2c\r\n",
        "\r\n"
    };

    return header;
}