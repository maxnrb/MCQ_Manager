//
// Created by quent on 18/06/2019.
//

#ifndef PROJET_HELLOHANDLER_H
#define PROJET_HELLOHANDLER_H

#include <pistache/endpoint.h>
#include "headers/CORS.h"
#include "utils/Utils.h"
#include "Database.h"
#include "headers/Authorization.h"
#include "utils/base64.h"

#include "rapidjson/document.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace Pistache;

using std::map;
using std::string;

class HelloHandler : public Http::Handler {
private:
    Database* db;
public:

    HTTP_PROTOTYPE(HelloHandler)

    HelloHandler()
    {
        db = new Database("127.0.0.1", "mcq_user", "mcq_password");
        db->connect();
    }
    ~HelloHandler()
    {
        delete db;
    }

    map<string, string> parseParameters(string params)
    {
        map<string, string> parsed;
        for(string param : Utils::explode(params, '&'))
        {
            vector<string> p = Utils::explode(param, '=');
            parsed.insert(std::pair<string, string>(p[0], p[1]));
        }
        return parsed;
    }

    void onRequest(const Http::Request& request, Http::ResponseWriter response)
    {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<CORS>("true");

        std::cout << "Request Received " << request.method() << std::endl;

        //If Preflight request
        if (request.method() == Http::Method::Options)
        {
            //Pré-requête: on accepte le header authorization avec CORS
            response.headers().add<Http::Header::AccessControlAllowHeaders>("authorization");
            response.send(Http::Code::Ok);
        }
        //If authenticate request
        else if (request.resource() == "/authenticate")
        {
            string decoded = base64_decode(Utils::explode(request.headers().tryGet<Authorization>()->getUri(), ' ')[1]);
            std::cout << "Request authenticate for: " << decoded << std::endl;

            std::vector<string> auth_params = Utils::explode(decoded, ':');
            string login = auth_params[0];
            string password = auth_params[1];

            if (db->checkUser(login, password))
            {
                std::cout << "Request success" << std::endl;
                string token = Utils::generateRandomString(20);
                std::cout << "Set token to " << token << std::endl;
                db->setToken(login, token);
                response.send(Http::Code::Ok, token);
            } else
            {
                std::cout << "Request unsuccessful" << std::endl;
                response.send(Http::Code::Unauthorized);
            }
        }
        else
        {
            if(request.headers().tryGet<Authorization>() == nullptr)
            {
                std::cout << "Header unsuccessful" << std::endl;
                response.send(Http::Code::Unauthorized);
            }
            else
            {
                //Token check
                string token = Utils::explode(request.headers().tryGet<Authorization>()->getUri(), ' ')[1];
                std::cout << "Request token " << token << std::endl;

                if(db->checkToken(token))
                {
                    std::cout << "-----REQUEST TYPE: " << request.method() << "-----" << std::endl;
                    std::cout << "->Ressource: " << request.resource() << std::endl;
                    std::cout << "->Body: " << request.body() << std::endl;

                    if (request.method() == Http::Method::Get)
                    {
                        if (request.resource() == "/groups")
                        {
                            vector<Group*> groups = db->getGroups();
                            if(groups.size() != 0)
                            {
                                std::cout << "Groups:" <<std::endl;
                                for(Group* group : groups)
                                {
                                    std::cout << group->getName() << std::endl;
                                }

                                //JSON Object
                                string json = "[";

                                for (const auto group : groups)
                                {
                                    json += group->serialize();
                                    json += ",";
                                }
                                json.pop_back();
                                json += "]";
                                response.send(Http::Code::Ok, json);
                            }
                            else
                            {
                                std::cout << "No groups found" <<std::endl;
                                response.send(Http::Code::Not_Found);
                            }
                        }
                    }

                    if (request.method() == Http::Method::Post)
                    {
                        if (request.resource() == "/user")
                        {
                            map<string, string> params = parseParameters(request.body());
                            std::cout << "Creating user with login " << params.at("login") << " and password "
                                      << params.at("password") << std::endl;

                            if(db->addUser(params.at("login"), params.at("password")))
                            {
                                std::cout << "User creation success" << std::endl;
                                response.send(Http::Code::Ok);
                            }
                            else
                            {
                                std::cout << "User creation failed" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }

                        if (request.resource() == "/group")
                        {
                            map<string, string> params = parseParameters(request.body());
                            std::cout << "Creating group " << params.at("group_name") << std::endl;
                            if(db->addGroup(params.at("group_name")))
                            {
                                std::cout << "Group creation success" << std::endl;
                                response.send(Http::Code::Ok);
                            }
                            else
                            {
                                std::cout << "Group creation failed" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }

                        if (request.resource() == "/student")
                        {
                            map<string, string> params = parseParameters(request.body());
                            std::cout << "Creating student " << params.at("group_id") << " " << params.at("first_name") << " " << params.at("last_name") << std::endl;
                            if(db->addStudent(params.at("first_name"), params.at("last_name"), std::stoi(params.at("group_id"))))
                            {
                                std::cout << "Student creation success" << std::endl;
                                response.send(Http::Code::Ok);
                            }
                            else
                            {
                                std::cout << "Student creation failed" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "Bad token" << std::endl;
                    response.send(Http::Code::Unauthorized);
                }
            }
        }
        std::cout << "-------------------------------------" << std::endl;
    }
};

#endif //PROJET_HELLOHANDLER_H
