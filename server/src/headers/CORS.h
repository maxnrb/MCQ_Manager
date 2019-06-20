//
// Created by quent on 18/06/2019.
//

#ifndef PROJET_CORS_H
#define PROJET_CORS_H

#include <pistache/http_header.h>

using namespace Pistache;

class CORS : public Http::Header::Header
{
private:
    std::string uri_;
public:
    NAME("Access-Control-Allow-Credentials")

    CORS()
    :uri_(){}

    CORS(const std::string& uri){
        this->uri_ = uri;
    }

    CORS(const char* uri){
        this->uri_ = uri;
    }

    void parse(const std::string& data)
    {
        uri_ = data;
    }

    void write(std::ostream& os) const {
        os << uri_;
    }

    void setUri(std::string uri) {
        uri_ = std::move(uri);
    }
};


#endif //PROJET_CORS_H
