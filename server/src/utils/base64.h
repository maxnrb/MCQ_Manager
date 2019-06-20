//
// Created by quent on 19/06/2019.
//

/**
 * Source: https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
 */

#ifndef PROJET_BASE64_H
#define PROJET_BASE64_H

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);


#endif //PROJET_BASE64_H
