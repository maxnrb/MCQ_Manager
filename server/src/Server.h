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
#include "Qt/ImageDownloader.h"
#include "Qt/QtTest.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <QApplication>

using namespace Pistache;

using std::map;
using std::string;

class Server : public Http::Handler
{
private:
    Database *db;
    ImageDownloader* window;
    int argc;
    char** argv;
public:

HTTP_PROTOTYPE(Server)
    string b64;
    Server(int argc, char** argv) : argc(argc), argv(argv)
    {
        db = new Database("127.0.0.1", "mcq_user", "mcq_password");
        db->connect();
    }

    ~Server()
    {
        delete db;
    }

    map<string, string> parseParameters(string params)
    {
        map<string, string> parsed;
        for (string param : Utils::explode(params, '&'))
        {
            vector<string> p = Utils::explode(param, '=');
            parsed.insert(std::pair<string, string>(p[0], p[1]));
        }
        return parsed;
    }

    void onRequest(const Http::Request &request, Http::ResponseWriter response) override
    {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<CORS>("true");

        std::cout << "Request Received " << request.method() << std::endl;
        std::cout << "Resource: " << request.resource() << std::endl;

        //If Preflight request
        if (request.method() == Http::Method::Options)
        {
            //Pré-requête: on accepte le header authorization avec CORS
            response.headers().add<Http::Header::AccessControlAllowHeaders>("authorization");
            response.headers().add<Http::Header::AccessControlAllowMethods>("GET, POST, PUT, DELETE");
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
        } else
        {
            if (request.headers().tryGet<Authorization>() == nullptr)
            {
                std::cout << "Header Authorization not found" << std::endl;
                response.send(Http::Code::Unauthorized);
                std::cout << "-------------------------------------" << std::endl;
                return;
            }
            //Token check
            if (!Utils::contains(request.headers().tryGet<Authorization>()->getUri(), " "))
            {
                std::cout << "Bad Authorization header" << std::endl;
                response.send(Http::Code::Unauthorized);
                std::cout << "-------------------------------------" << std::endl;
                return;
            }
            string token = Utils::explode(request.headers().tryGet<Authorization>()->getUri(), ' ')[1];
            std::cout << "Request token " << token << std::endl;

            if (db->checkToken(token))
            {
                std::cout << "-----REQUEST TYPE: " << request.method() << "-----" << std::endl;
                std::cout << "->Ressource: ";
                std::cout << request.resource() << std::endl;
                std::cout << "->Body: " << request.body() << std::endl;

                if (request.method() == Http::Method::Get)
                {
                    if (request.resource() == "/test_image")
                    {
                        int test_id = std::stoi(request.query().get("test_id").get());
                        int student_id = std::stoi(request.query().get("student_id").get());
                        string b64 =  Utils::getBase64ImgFromUrl("http://10.0.1.49/"+std::to_string(test_id)+"/"+std::to_string(student_id)+".jpg", argc, argv);

                        if(b64 != "")
                        {
                            vector<Test*> tests = db->getTestsByStudent(student_id);
                            bool participate = false;
                            for(Test* t : tests)
                            {
                                if(t->getId() == test_id)
                                {
                                    participate = true;
                                }
                            }

                            if(!participate) db->setParticipate(student_id, test_id);
                            response.send(Http::Code::Ok, b64);
                        }
                        else
                        {
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if (request.resource() == "/correction")
                    {
                        int test_id = std::stoi(request.query().get("test_id").get());
                        int student_id = std::stoi(request.query().get("student_id").get());
                        string b64 =  Utils::getBase64ImgFromUrl("http://10.0.1.49/"+std::to_string(test_id)+"/"+std::to_string(student_id)+".jpg", argc, argv);

                        if(b64 != "")
                        {
                            QImage pixmap = Utils::getImageFromUrl("http://10.0.1.49/"+std::to_string(test_id)+"/"+std::to_string(student_id)+".jpg", argc, argv);
                            QtTest* test = new QtTest(pixmap);

                            std::cout << test->getNbQuestions() << std::endl;
                            std::cout << test->getNbAnswer() << std::endl;

                            delete test;

                            response.send(Http::Code::Ok);
                        }
                        else
                        {
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if (request.resource() == "/users")
                    {
                        response.send(Http::Code::Ok, db->getUsersSerialized());
                    }
                    else if (request.resource() == "/groups")
                    {
                        vector<Group *> groups = db->getGroups();
                        if (groups.size() != 0)
                        {
                            std::cout << "Groups:" << std::endl;
                            for (Group *group : groups)
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
                        } else
                        {
                            std::cout << "No groups found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if (request.resource() == "/students")
                    {
                        int group_id = std::stoi(request.query().get("group_id").get());
                        vector<Student *> students = db->getStudents(group_id);

                        if (students.size() != 0)
                        {
                            std::cout << "Students:" << std::endl;
                            for (Student *student : students)
                            {
                                std::cout << student->getFirstName() << " " << student->getLastName() << std::endl;
                            }

                            //JSON Object
                            string json = "[";

                            for (const auto student : students)
                            {
                                json += student->serialize();
                                json += ",";
                            }
                            json.pop_back();
                            json += "]";
                            response.send(Http::Code::Ok, json);
                        } else
                        {
                            std::cout << "No students found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if(request.resource() == "/tests")
                    {
                        int group_id = std::stoi(request.query().get("group_id").get());
                        vector<Test *> tests = db->getTests(group_id);

                        if (tests.size() != 0)
                        {
                            std::cout << "Tests:" << std::endl;
                            for (Test *test : tests)
                            {
                                std::cout << test->getName() << std::endl;
                            }

                            //JSON Object
                            string json = "[";

                            for (const auto test : tests)
                            {
                                json += test->serialize();
                                json += ",";
                            }
                            json.pop_back();
                            json += "]";
                            response.send(Http::Code::Ok, json);
                        } else
                        {
                            std::cout << "No tests found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if(request.resource() == "/participations")
                    {
                        int student_id = std::stoi(request.query().get("student_id").get());
                        vector<Test *> tests = db->getTestsByStudent(student_id);

                        if (tests.size() != 0)
                        {
                            std::cout << "Tests:" << std::endl;
                            for (Test *test : tests)
                            {
                                std::cout << test->getName() << std::endl;
                            }

                            //JSON Object
                            string json = "[";

                            for (const auto test : tests)
                            {
                                json += test->serialize();
                                json += ",";
                            }
                            json.pop_back();
                            json += "]";
                            response.send(Http::Code::Ok, json);
                        } else
                        {
                            std::cout << "No tests found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else if(request.resource() == "/students_participate")
                    {
                        int test_id = std::stoi(request.query().get("test_id").get());
                        std::cout << "Get Students for test " << test_id <<  std::endl;
                        vector<Student *> students = db->getStudentsByTest(test_id);

                        if (students.size() != 0)
                        {
                            std::cout << "Students:" << std::endl;
                            for (Student *st : students)
                            {
                                std::cout << st->getFirstName() << std::endl;
                            }

                            //JSON Object
                            string json = "[";

                            for (const auto student : students)
                            {
                                json += student->serialize();
                                json += ",";
                            }
                            json.pop_back();
                            json += "]";
                            response.send(Http::Code::Ok, json);
                        } else
                        {
                            std::cout << "No students found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else
                    {
                        std::cout << "Bad request" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }

                else if (request.method() == Http::Method::Post)
                {
                    if (request.resource() == "/user")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Creating user with login " << params.at("login") << " and password "
                                  << params.at("password") << std::endl;

                        if (db->addUser(params.at("login"), params.at("password")))
                        {
                            std::cout << "User creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "User creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }

                    else if (request.resource() == "/group")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Creating group " << params.at("group_name") << std::endl;
                        if (db->addGroup(params.at("group_name")))
                        {
                            std::cout << "Group creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Group creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }

                    else if (request.resource() == "/student")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Creating student " << params.at("group_id") << " " << params.at("first_name")
                                  << " " << params.at("last_name") << std::endl;
                        if (db->addStudent(params.at("first_name"), params.at("last_name"),
                                           std::stoi(params.at("group_id"))))
                        {
                            std::cout << "Student creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Student creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }

                    else if (request.resource() == "/test")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Creating test named " << params.at("name") << " for " << params.at("group_id") << " with " << params.at("nb_questions")
                                  << " questions with " << params.at("nb_answers") << " at " << params.at("date") << std::endl;
                        int test_id = db->addTest(params.at("name"), params.at("group_id"), params.at("date"), db->getUserIdByToken(token));
                        if (test_id != -1)
                        {
                            for(int i = 0; i < std::stoi(params.at(("nb_questions"))); i++)
                            {
                                int question_id = db->addQuestion(test_id, 1);
                                for(int j = 0; j < std::stoi(params.at(("nb_answers"))); j++)
                                {
                                    db->addAnswerToQuestion(question_id, false);
                                }
                            }
                            std::cout << "Test creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Test creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                        vector<Student*> students = db->getStudents(std::stoi(params.at("group_id")));
                        for(Student* student : students)
                        {
                            db->setParticipate(student->getId(), test_id);
                        }
                    }

                    else if (request.resource() == "/question")
                    {
                        map<string, string> params = parseParameters(request.body());

                        std::cout << "Adding question to test " << params.at("test_id") << " with scale " << params.at("scale") << std::endl;
                        if (db->addQuestion(stoi(params.at("test_id")), stoi(params.at("scale"))) != -1)
                        {
                            std::cout << "Question creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Question creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }

                    else if (request.resource() == "/answer")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Adding answer to question number " << params.at("question_id") << std::endl;
                        bool good;
                        if (db->addAnswerToQuestion(stoi(params.at("question_id")), params.at("good") != "0"))
                        {
                            std::cout << "Answer creation success" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Answer creation failed" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        std::cout << "Bad request" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.method() == Http::Method::Put)
                {
                    if (request.resource() == "/group")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Set name of group " << params.at("group_id") << " to " << params.at("name") << std::endl;
                        if (db->modifyGroup(std::stoi(params.at("group_id")), params.at("name")))
                        {
                            std::cout << "Group was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Group failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else if (request.resource() == "/student")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Update student " << params.at("student_id") << std::endl;
                        if (db->modifyStudent(std::stoi(params.at("student_id")), params.at("first_name"),params.at("last_name"),std::stoi(params.at("group_id"))))
                        {
                            std::cout << "Student was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Student failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else if (request.resource() == "/test")
                    {
                        map<string, string> params = parseParameters(request.body());
                        Test* test = db->getTestById(std::stoi(params.at("test_id")));
                        if(test->getUserId() == db->getUserIdByToken(token))
                        {
                            std::cout << "Update test " << params.at("test_id") << std::endl;
                            if (db->modifyTest(std::stoi(params.at("test_id")), params.at("test_name"),
                                               params.at("date")))
                            {
                                std::cout << "Test was successfully modified" << std::endl;
                                response.send(Http::Code::Ok);
                            } else
                            {
                                std::cout << "Test failed to modify" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }
                        else
                        {
                            std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                            response.send(Http::Code::Forbidden);
                        }

                        delete test;
                    }
                    else if (request.resource() == "/answer")
                    {
                        map<string, string> params = parseParameters(request.body());
                        int answer_id = std::stoi(params.at("answer_id"));

                        Answer* answer = db->getAnswerById(answer_id);
                        Question* question = db->getQuestionById(answer->getQuestionId());
                        Test* test = db->getTestById(question->getTestId());

                        if(test->getUserId() == db->getUserIdByToken(token))
                        {
                            std::cout << "Update answer " << params.at("answer_id") << std::endl;
                            if (db->modifyAnswer(answer_id, (params.at("isgood") == "1")?1:0))
                            {
                                std::cout << "Answer was successfully modified" << std::endl;
                                response.send(Http::Code::Ok);
                            } else
                            {
                                std::cout << "Answer failed to modify" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }
                        else
                        {
                            std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                            response.send(Http::Code::Forbidden);
                        }

                        delete test;
                    }
                    else if (request.resource() == "/user")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Update user " << params.at("user_id") << std::endl;
                        if (db->modifyUser(std::stoi(params.at("user_id")), params.at("login")))
                        {
                            std::cout << "User was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "User failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        std::cout << "Bad request" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.method() == Http::Method::Delete)
                {
                    if (request.resource() == "/student")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Deleting student " << params.at("student_id") << std::endl;
                        if (db->deleteStudent(std::stoi(params.at("student_id"))))
                        {
                            std::cout << "Student was successfully deleted" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Student failed to delete" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else if (request.resource() == "/group")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Deleting group " << params.at("group_id") << std::endl;
                        if (db->deleteGroup(std::stoi(params.at("group_id"))))
                        {
                            std::cout << "Group was successfully deleted" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "Group failed to delete" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else if (request.resource() == "/test")
                    {
                        map<string, string> params = parseParameters(request.body());
                        Test* test = db->getTestById(std::stoi(params.at("test_id")));
                        if(test->getUserId() == db->getUserIdByToken(token))
                        {
                            std::cout << "Deleting test " << params.at("test_id") << std::endl;
                            if (db->deleteTest(std::stoi(params.at("test_id"))))
                            {
                                std::cout << "Test was successfully deleted" << std::endl;
                                response.send(Http::Code::Ok);
                            } else
                            {
                                std::cout << "Test failed to delete" << std::endl;
                                response.send(Http::Code::Bad_Request);
                            }
                        }
                        else
                        {
                            std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                            response.send(Http::Code::Forbidden);
                        }
                    }
                    else if (request.resource() == "/user")
                    {
                        map<string, string> params = parseParameters(request.body());
                        std::cout << "Demete user " << params.at("user_id") << std::endl;
                        if (db->deleteUser(std::stoi(params.at("user_id"))))
                        {
                            std::cout << "User was successfully deleted" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            std::cout << "User failed to delete" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        std::cout << "Bad request" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else
                {
                    std::cout << "Bad request" << std::endl;
                    response.send(Http::Code::Bad_Request);
                }
            }
            else
            {
                std::cout << "Bad token" << std::endl;
                response.send(Http::Code::Unauthorized);
            }
        }
        std::cout << "-------------------------------------" << std::endl;
    }
};

#endif //PROJET_HELLOHANDLER_H
