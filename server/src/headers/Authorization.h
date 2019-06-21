//
// Created by quent on 19/06/2019.
//

#ifndef PROJET_AUTHORIZATION_H
#define PROJET_AUTHORIZATION_H

#include <pistache/http_header.h>
#include <iostream>

using namespace Pistache;

class Authorization : public Http::Header::Header
{
private:
    std::string uri_;
public:
    NAME("Authorization")

    Authorization()
            :uri_(){}

    Authorization(const std::string& uri){
        this->uri_ = uri;
    }

    Authorization(const char* uri){
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

    const string getUri() const
    {
        return this->uri_;
    }
};


#endif //PROJET_AUTHORIZATION_H
