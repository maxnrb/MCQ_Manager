//
// Created by quent on 19/06/2019.
//

#ifndef PROJET_SHA1_H
#define PROJET_SHA1_H


#include <cstdint>
#include <iostream>
#include <string>


class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);
    static std::string toSha1(std::string input);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};



#endif //PROJET_SHA1_H
