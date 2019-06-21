//
// Created by quent on 19/06/2019.
//

#ifndef PROJET_GROUP_H
#define PROJET_GROUP_H

#include <string>
#include "../rapidjson/prettywriter.h"

using namespace rapidjson;
using std::string;

class Group
{
private:
    int id;
    string name;

public:
    Group(int id, const string &name) : id(id), name(name){}

    int getId() const{return id;}
    void setId(int id){Group::id = id;}

    const string &getName() const{return name;}
    void setName(const string &name){Group::name = name;}

    string serialize() const
    {
        string json = "{";
        json += "\"id\":\""+ std::to_string(id) +"\",";
        json += "\"name\":\""+ name +"\"";
        json += "}";
        return json;
    }
};


#endif //PROJET_GROUP_H
