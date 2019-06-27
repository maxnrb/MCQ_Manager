//
// Created by quent on 18/06/2019.
//

#include "Server.h"
#include <string.h>

Server::Server(int argc, char** argv) : argc(argc), argv(argv)
{
    if(argc == 3)
    {
        string b64 = Utils::getBase64ImgFromUrl((string) argv[1] + "1/1.jpg", argc, argv, console_output);

        if (b64 != "")
        {
            url_scans = argv[1];

            if(strcmp(argv[2],"true") == 0 || strcmp(argv[2],"false") == 0)
            {
                console_output = strcmp(argv[2],"true") == 0;
            }
        }
        else
        {
            std::cout << "Your url seems to be wrong." << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << "Usage: ./Program <url> <output>\n Please don't forget the final / at the end of the url." << std::endl;
        exit(1);
    }
    db = new Database("127.0.0.1", "mcq_user", "mcq_password");
    db->connect();
    if(console_output) std::cout << "Listening for requests..." << std::endl;
}

map<string, string> Server::parseParameters(string params)
{
    map<string, string> parsed;
    for (string param : Utils::explode(params, '&'))
    {
        vector<string> p = Utils::explode(param, '=');
        parsed.insert(std::pair<string, string>(p[0], p[1]));
    }
    return parsed;
}

void Server::onRequest(const Http::Request &request, Http::ResponseWriter response)
{
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.headers().add<CORS>("true");

    if(console_output) std::cout << "Request Received " << request.method() << std::endl;
    if(console_output) std::cout << "Resource: " << request.resource() << std::endl;

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
        if(console_output) std::cout << "Request authenticate" << std::endl;

        std::vector<string> auth_params = Utils::explode(decoded, ':');
        string login = auth_params[0];
        string password = auth_params[1];

        if (db->checkUser(login, password))
        {
            if(console_output) std::cout << "Request success" << std::endl;
            string token = Utils::generateRandomString(20);
            if(console_output) std::cout << "Set token to " << token << std::endl;
            db->setToken(login, token);
            response.send(Http::Code::Ok, token);
        } else
        {
            if(console_output) std::cout << "Request unsuccessful" << std::endl;
            response.send(Http::Code::Unauthorized);
        }
    } else
    {
        if (request.headers().tryGet<Authorization>() == nullptr)
        {
            if(console_output) std::cout << "Header Authorization not found" << std::endl;
            response.send(Http::Code::Unauthorized);
            if(console_output) std::cout << "-------------------------------------" << std::endl;
            return;
        }
        //Token check
        if (!Utils::contains(request.headers().tryGet<Authorization>()->getUri(), " "))
        {
            if(console_output) std::cout << "Bad Authorization header" << std::endl;
            response.send(Http::Code::Unauthorized);
            if(console_output) std::cout << "-------------------------------------" << std::endl;
            return;
        }
        string token = Utils::explode(request.headers().tryGet<Authorization>()->getUri(), ' ')[1];
        if(console_output) std::cout << "Request token " << token << std::endl;

        if (db->checkToken(token))
        {
            if(console_output) std::cout << "-----REQUEST TYPE: " << request.method() << "-----" << std::endl;
            if(console_output) std::cout << "->Ressource: ";
            if(console_output) std::cout << request.resource() << std::endl;
            if(console_output) std::cout << "->Body: " << request.body() << std::endl;

            if (request.method() == Http::Method::Get)
            {
                if (request.resource() == "/grade")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    int student_id = std::stoi(request.query().get("student_id").get());

                    Test* test = db->getTestById(test_id);

                    int note = 0;

                    for(Question* question : test->getQuestions())
                    {
                        int scale = question->getScale();

                        vector<StudentAnswer*> student_answers = db->getStudentAnswersByQuestion(question->getId(), student_id);

                        bool isCorrect = true;
                        for(StudentAnswer* student_answer : student_answers)
                        {
                            Answer* a = db->getAnswerById(student_answer->getAnswerId());
                            if(a->isGoodAnswer())
                            {
                                if(!student_answer->isState())
                                {
                                    isCorrect = false;
                                }
                            }
                            else if(!a->isGoodAnswer())
                            {
                                if(student_answer->isState())
                                {
                                    isCorrect = false;
                                }
                            }
                            delete a;
                        }
                        if(isCorrect)
                        {
                            note += scale;
                        }
                    }

                    delete test;
                    response.send(Http::Code::Ok, std::to_string(note));
                }
                if (request.resource() == "/test_image")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    int student_id = std::stoi(request.query().get("student_id").get());
                    if(!db->isStudentCorrected(student_id, test_id))
                    {
                        if(console_output) std::cout << "Scanning image for student " << student_id << " at test " << test_id
                                  << std::endl;
                        string b64 = Utils::getBase64ImgFromUrl(
                                url_scans + std::to_string(test_id) + "/" + std::to_string(student_id) +
                                ".jpg", argc, argv, console_output);

                        if (b64 != "")
                        {
                            vector<Test *> tests = db->getTestsByStudent(student_id);
                            bool participate = false;
                            for (Test *t : tests)
                            {
                                if (t->getId() == test_id)
                                {
                                    participate = true;
                                }
                            }

                            if (!participate) db->setParticipate(student_id, test_id);
                            if(console_output) std::cout << "Test scanned and saved" << std::endl;
                            response.send(Http::Code::Ok, b64);
                        } else
                        {
                            if(console_output) std::cout << "Image not found" << std::endl;
                            response.send(Http::Code::Not_Found);
                        }
                    }
                    else
                    {
                        if(console_output) std::cout << "Student already corrected. Return correction image instead" << std::endl;
                        QImage image = Utils::getImageFromUrl(
                                url_scans + std::to_string(test_id) + "/" +
                                std::to_string(student_id) + ".jpg", argc, argv, console_output);

                        QtTest *qtest = new QtTest(image);

                        response.send(Http::Code::Ok, qtest->getImageBase64());

                        delete qtest;
                    }
                }
                else if (request.resource() == "/correction")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    int student_id = std::stoi(request.query().get("student_id").get());
                    if(console_output) std::cout << "Correcting student " << student_id << " at test " << test_id << std::endl;

                    QImage image = Utils::getImageFromUrl(
                            url_scans + std::to_string(test_id) + "/" +
                            std::to_string(student_id) + ".jpg", argc, argv, console_output);
                    if(!image.isNull())
                    {
                        if(!db->isStudentCorrected(student_id, test_id))
                        {
                            QtTest *qtest = new QtTest(image);
                            Test *test = db->getTestById(test_id);
                            for (int i = 0; i < test->getQuestions().size(); i++) //Pour chaque question
                            {
                                Question *question = test->getQuestions().at(i);
                                for (int j = 0; j < question->getAnswers().size(); j++)
                                {
                                    Answer *answer = question->getAnswers().at(j);

                                    bool state = qtest->getState()[question->getQuestionNumber() - 1][
                                            answer->getAnswerNumber() - 1];
                                    db->addStudentAnswer(student_id, answer->getId(), state);
                                }
                            }
                            db->setCorrected(student_id, test_id);
                            if(console_output) std::cout << "Student corrected" << std::endl;
                            response.send(Http::Code::Ok, qtest->getImageBase64());
                            delete qtest;
                            delete test;
                        }
                        else
                        {
                            if(console_output) std::cout << "Student already corrected. Return correction image instead" << std::endl;
                            QImage image = Utils::getImageFromUrl(
                                    url_scans + std::to_string(test_id) + "/" +
                                    std::to_string(student_id) + ".jpg", argc, argv, console_output);

                            QtTest *qtest = new QtTest(image);

                            response.send(Http::Code::Ok, qtest->getImageBase64());

                            delete qtest;
                        }

                    }
                    else
                    {
                        if(console_output) std::cout << "Image not found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if (request.resource() == "/correct_all")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    Test *test = db->getTestById(test_id);
                    if(console_output) std::cout << "Correcting all students of test " << test_id << std::endl;

                    vector<Student*> students = db->getStudentsByTest(test_id);

                    for(Student* student : students)
                    {
                        int student_id = student->getId();

                        QImage image = Utils::getImageFromUrl(
                                url_scans + std::to_string(test_id) + "/" +
                                std::to_string(student_id) + ".jpg", argc, argv, console_output);;

                        if (!image.isNull())
                        {
                            if (!db->isStudentCorrected(student_id, test_id))
                            {
                                QtTest *qtest = new QtTest(image);
                                for (int i = 0; i < test->getQuestions().size(); i++) //Pour chaque question
                                {
                                    Question *question = test->getQuestions().at(i);
                                    for (int j = 0; j < question->getAnswers().size(); j++)
                                    {
                                        Answer *answer = question->getAnswers().at(j);

                                        bool state = qtest->getState()[question->getQuestionNumber() - 1][
                                                answer->getAnswerNumber() - 1];
                                        db->addStudentAnswer(student_id, answer->getId(), state);
                                    }
                                }
                                db->setCorrected(student_id, test_id);
                                if(console_output) std::cout << "Student corrected" << std::endl;
                                delete qtest;
                            }
                        }
                        else
                        {
                            if(console_output) std::cout << "Student " << student->getId() << " has'nt participated to the test." << std::endl;
                        }
                    }
                    response.send(Http::Code::Ok);
                    delete test;
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
                        if(console_output) std::cout << "Groups:" << std::endl;
                        for (Group *group : groups)
                        {
                            if(console_output) std::cout << group->getName() << std::endl;
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
                        if(console_output) std::cout << "No groups found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if (request.resource() == "/students")
                {
                    int group_id = std::stoi(request.query().get("group_id").get());
                    vector<Student *> students = db->getStudents(group_id);

                    if (students.size() != 0)
                    {
                        if(console_output) std::cout << "Students:" << std::endl;
                        for (Student *student : students)
                        {
                            if(console_output) std::cout << student->getFirstName() << " " << student->getLastName() << std::endl;
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

                        for(Student* a : students)
                        {
                            delete a;
                        }
                    } else
                    {
                        if(console_output) std::cout << "No students found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if(request.resource() == "/tests")
                {
                    int group_id = std::stoi(request.query().get("group_id").get());
                    vector<Test *> tests = db->getTests(group_id);

                    if (tests.size() != 0)
                    {
                        if(console_output) std::cout << "Tests:" << std::endl;
                        for (Test *test : tests)
                        {
                            if(console_output) std::cout << test->getName() << std::endl;
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

                        for(Test* a : tests)
                        {
                            delete a;
                        }

                        response.send(Http::Code::Ok, json);
                    } else
                    {
                        if(console_output) std::cout << "No tests found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if(request.resource() == "/test")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    Test* test = db->getTestById(test_id);

                    if(console_output) std::cout << "Getting test : "  << test_id << std::endl;

                    //JSON Object
                    string json = test->serialize();
                    response.send(Http::Code::Ok, json);
                    delete test;
                }
                else if(request.resource() == "/participations")
                {
                    int student_id = std::stoi(request.query().get("student_id").get());
                    vector<Test *> tests = db->getTestsByStudent(student_id);

                    if (tests.size() != 0)
                    {
                        if(console_output) std::cout << "Tests:" << std::endl;
                        for (Test *test : tests)
                        {
                            if(console_output) std::cout << test->getName() << std::endl;
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

                        for(Test* a : tests)
                        {
                            delete a;
                        }
                    } else
                    {
                        if(console_output) std::cout << "No tests found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if(request.resource() == "/students_participate")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    if(console_output) std::cout << "Get Students for test " << test_id <<  std::endl;
                    vector<Student *> students = db->getStudentsByTest(test_id);

                    if (students.size() != 0)
                    {
                        if(console_output) std::cout << "Students:" << std::endl;
                        for (Student *st : students)
                        {
                            if(console_output) std::cout << st->getFirstName() << std::endl;
                        }

                        //JSON Object
                        string json = "[";

                        for (Student* student : students)
                        {
                            int note = -1;
                            if(student->isCorrected())
                            {
                                note = 0;
                                int student_id = student->getId();

                                Test* test = db->getTestById(test_id);

                                for(Question* question : test->getQuestions())
                                {
                                    int scale = question->getScale();

                                    vector<StudentAnswer*> student_answers = db->getStudentAnswersByQuestion(question->getId(), student_id);

                                    bool isCorrect = true;
                                    for(StudentAnswer* student_answer : student_answers)
                                    {
                                        Answer* a = db->getAnswerById(student_answer->getAnswerId());

                                        if(a->isGoodAnswer())
                                        {
                                            if(!student_answer->isState())
                                            {
                                                isCorrect = false;
                                            }
                                        }
                                        else if(!a->isGoodAnswer())
                                        {
                                            if(student_answer->isState())
                                            {
                                                isCorrect = false;
                                            }
                                        }
                                        delete a;
                                    }
                                    if(isCorrect)
                                    {
                                        note += scale;
                                    }
                                }

                                delete test;
                            }
                            student->setGrade(note);
                            json += student->serialize();
                            json += ",";
                        }
                        json.pop_back();
                        json += "]";
                        response.send(Http::Code::Ok, json);

                        for(Student* a : students)
                        {
                            delete a;
                        }
                    } else
                    {
                        if(console_output) std::cout << "No students found" << std::endl;
                        response.send(Http::Code::Not_Found);
                    }
                }
                else if(request.resource() == "/student_answers")
                {
                    int test_id = std::stoi(request.query().get("test_id").get());
                    int student_id = std::stoi(request.query().get("student_id").get());
                    if(console_output) std::cout << "Get Answers of student " << student_id << " for test " << test_id <<  std::endl;

                    string json = "[";

                    Test* test = db->getTestById(test_id);

                    for(Question* q : test->getQuestions())
                    {
                        vector<StudentAnswer*> answers =  db->getStudentAnswersByQuestion(q->getId(), student_id);
                        json += "{";
                        json += "\"question_id\":\"" + std::to_string(q->getId()) + "\",";
                        json += "\"question_number\":\"" + std::to_string(q->getQuestionNumber()) + "\",";
                        json += "\"answers\":[";
                        for(StudentAnswer* sa : answers)
                        {
                            Answer* answer = db->getAnswerById(sa->getAnswerId());
                            json += "{";
                            json += "\"answer_id\":\"" + std::to_string(answer->getId()) + "\",";
                            json += "\"answer_number\":\"" + std::to_string(answer->getAnswerNumber()) + "\",";
                            json += "\"state\":\"" + ((string) ((sa->isState())?"1":"0")) + "\"";
                            json += "},";
                            delete answer;
                        }
                        if(answers.size() != 0) json.pop_back();
                        json += "]";
                        json += "},";

                        for(StudentAnswer* a : answers)
                        {
                            delete a;
                        }
                    }

                    json.pop_back();

                    json += "]";

                    delete test;

                    response.send(Http::Code::Ok, json);
                }
                else
                {
                    if(console_output) std::cout << "Bad request" << std::endl;
                    response.send(Http::Code::Bad_Request);
                }
            }

            else if (request.method() == Http::Method::Post)
            {
                if (request.resource() == "/user")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Creating user with login " << params.at("login") << " and password "
                              << params.at("password") << std::endl;

                    if (db->addUser(params.at("login"), params.at("password")))
                    {
                        if(console_output) std::cout << "User creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "User creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }

                else if (request.resource() == "/group")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Creating group " << params.at("group_name") << std::endl;
                    if (db->addGroup(params.at("group_name")))
                    {
                        if(console_output) std::cout << "Group creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Group creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }

                else if (request.resource() == "/student")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Creating student " << params.at("group_id") << " " << params.at("first_name")
                              << " " << params.at("last_name") << std::endl;
                    if (db->addStudent(params.at("first_name"), params.at("last_name"),
                                       std::stoi(params.at("group_id"))))
                    {
                        if(console_output) std::cout << "Student creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Student creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }

                else if (request.resource() == "/test")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Creating test named " << params.at("name") << " for " << params.at("group_id") << " with " << params.at("nb_questions")
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
                        if(console_output) std::cout << "Test creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Test creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                    vector<Student*> students = db->getStudents(std::stoi(params.at("group_id")));
                    for(Student* student : students)
                    {
                        db->setParticipate(student->getId(), test_id);
                    }
                    for(Student* a : students)
                    {
                        delete a;
                    }
                }

                else if (request.resource() == "/question")
                {
                    map<string, string> params = parseParameters(request.body());

                    if(console_output) std::cout << "Adding question to test " << params.at("test_id") << " with scale " << params.at("scale") << std::endl;
                    if (db->addQuestion(stoi(params.at("test_id")), stoi(params.at("scale"))) != -1)
                    {
                        if(console_output) std::cout << "Question creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Question creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }

                else if (request.resource() == "/answer")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Adding answer to question number " << params.at("question_id") << std::endl;
                    bool good;
                    if (db->addAnswerToQuestion(stoi(params.at("question_id")), params.at("good") != "0"))
                    {
                        if(console_output) std::cout << "Answer creation success" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Answer creation failed" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else
                {
                    if(console_output) std::cout << "Bad request" << std::endl;
                    response.send(Http::Code::Bad_Request);
                }
            }
            else if (request.method() == Http::Method::Put)
            {
                if (request.resource() == "/student_answer")
                {
                    map<string, string> params = parseParameters(request.body());
                    int answer_id = std::stoi(params.at("answer_id"));
                    int student_id = std::stoi(params.at("student_id"));
                    if(console_output) std::cout << "Modify Student answer Answer " << answer_id << std::endl;

                    if(console_output) std::cout << "Update answer " << params.at("answer_id") << std::endl;
                    if (db->modifyStudentAnswer(student_id, answer_id, params.at("is_checked") == "1")?1:0)
                    {
                        if(console_output) std::cout << "Student Answer was successfully modified" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Student Answer failed to modify" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.resource() == "/group")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Set name of group " << params.at("group_id") << " to " << params.at("name") << std::endl;
                    if (db->modifyGroup(std::stoi(params.at("group_id")), params.at("name")))
                    {
                        if(console_output) std::cout << "Group was successfully modified" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Group failed to modify" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.resource() == "/student")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Update student " << params.at("student_id") << std::endl;
                    if (db->modifyStudent(std::stoi(params.at("student_id")), params.at("first_name"),params.at("last_name"),std::stoi(params.at("group_id"))))
                    {
                        if(console_output) std::cout << "Student was successfully modified" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Student failed to modify" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.resource() == "/test")
                {
                    map<string, string> params = parseParameters(request.body());
                    Test* test = db->getTestById(std::stoi(params.at("test_id")));
                    if(test->getUserId() == db->getUserIdByToken(token))
                    {
                        if(console_output) std::cout << "Update test " << params.at("test_id") << std::endl;
                        if (db->modifyTest(std::stoi(params.at("test_id")), params.at("test_name"),
                                           params.at("date")))
                        {
                            if(console_output) std::cout << "Test was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            if(console_output) std::cout << "Test failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        if(console_output) std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                        response.send(Http::Code::Forbidden);
                    }

                    delete test;
                }
                else if (request.resource() == "/answer")
                {
                    map<string, string> params = parseParameters(request.body());
                    int answer_id = std::stoi(params.at("answer_id"));
                    if(console_output) std::cout << "Modify Answer " << answer_id << std::endl;

                    Answer* answer = db->getAnswerById(answer_id);
                    Question* question = db->getQuestionById(answer->getQuestionId());
                    Test* test = db->getTestById(question->getTestId());

                    if(test->getUserId() == db->getUserIdByToken(token))
                    {
                        if(console_output) std::cout << "Update answer " << params.at("answer_id") << std::endl;
                        if (db->modifyAnswer(answer_id, (params.at("is_good") == "1")?1:0))
                        {
                            if(console_output) std::cout << "Answer was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            if(console_output) std::cout << "Answer failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        if(console_output) std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                        response.send(Http::Code::Forbidden);
                    }

                    delete test;
                    delete answer;
                    delete question;
                }
                else if (request.resource() == "/question")
                {
                    map<string, string> params = parseParameters(request.body());
                    int question_id = std::stoi(params.at("question_id"));
                    if(console_output) std::cout << "Modify Question " << question_id << std::endl;

                    Question* question = db->getQuestionById(question_id);
                    Test* test = db->getTestById(question->getTestId());

                    if(test->getUserId() == db->getUserIdByToken(token))
                    {
                        if(console_output) std::cout << "Update question " << params.at("question_id") << std::endl;
                        if (db->modifyQuestion(question_id, std::stoi(params.at("scale"))))
                        {
                            if(console_output) std::cout << "Question was successfully modified" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            if(console_output) std::cout << "Question failed to modify" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        if(console_output) std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                        response.send(Http::Code::Forbidden);
                    }

                    delete test;
                    delete question;
                }
                else if (request.resource() == "/user")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Update user " << params.at("user_id") << std::endl;
                    if (db->modifyUser(std::stoi(params.at("user_id")), params.at("login")))
                    {
                        if(console_output) std::cout << "User was successfully modified" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "User failed to modify" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else
                {
                    if(console_output) std::cout << "Bad request" << std::endl;
                    response.send(Http::Code::Bad_Request);
                }
            }
            else if (request.method() == Http::Method::Delete)
            {
                if (request.resource() == "/student")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Deleting student " << params.at("student_id") << std::endl;
                    if (db->deleteStudent(std::stoi(params.at("student_id"))))
                    {
                        if(console_output) std::cout << "Student was successfully deleted" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Student failed to delete" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.resource() == "/group")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Deleting group " << params.at("group_id") << std::endl;
                    if (db->deleteGroup(std::stoi(params.at("group_id"))))
                    {
                        if(console_output) std::cout << "Group was successfully deleted" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "Group failed to delete" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else if (request.resource() == "/test")
                {
                    map<string, string> params = parseParameters(request.body());
                    Test* test = db->getTestById(std::stoi(params.at("test_id")));
                    if(test->getUserId() == db->getUserIdByToken(token))
                    {
                        if(console_output) std::cout << "Deleting test " << params.at("test_id") << std::endl;
                        if (db->deleteTest(std::stoi(params.at("test_id"))))
                        {
                            if(console_output) std::cout << "Test was successfully deleted" << std::endl;
                            response.send(Http::Code::Ok);
                        } else
                        {
                            if(console_output) std::cout << "Test failed to delete" << std::endl;
                            response.send(Http::Code::Bad_Request);
                        }
                    }
                    else
                    {
                        if(console_output) std::cout << "Test cannot be managed by user (unauthorized)" << std::endl;
                        response.send(Http::Code::Forbidden);
                    }
                    delete test;
                }
                else if (request.resource() == "/user")
                {
                    map<string, string> params = parseParameters(request.body());
                    if(console_output) std::cout << "Demete user " << params.at("user_id") << std::endl;
                    if (db->deleteUser(std::stoi(params.at("user_id"))))
                    {
                        if(console_output) std::cout << "User was successfully deleted" << std::endl;
                        response.send(Http::Code::Ok);
                    } else
                    {
                        if(console_output) std::cout << "User failed to delete" << std::endl;
                        response.send(Http::Code::Bad_Request);
                    }
                }
                else
                {
                    if(console_output) std::cout << "Bad request" << std::endl;
                    response.send(Http::Code::Bad_Request);
                }
            }
            else
            {
                if(console_output) std::cout << "Bad request" << std::endl;
                response.send(Http::Code::Bad_Request);
            }
        }
        else
        {
            if(console_output) std::cout << "Bad token" << std::endl;
            response.send(Http::Code::Unauthorized);
        }
    }
    if(console_output) std::cout << "-------------------------------------" << std::endl;
}
