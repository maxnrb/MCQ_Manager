#include <iostream>
#include <QtWidgets/QApplication>
#include "Qt/ImageDownloader.h"
#include <pistache/endpoint.h>
#include "headers/CORS.h"
#include "headers/Authorization.h"
#include "HelloHandler.h"

using namespace Pistache;

int main(int argc, char** argv)
{
    Http::Header::Registry::instance().registerHeader<CORS>();
    Http::Header::Registry::instance().registerHeader<Authorization>();

    Address addr(Ipv4::any(), Port(8080));

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    std::shared_ptr<HelloHandler> handler = std::make_shared<HelloHandler>(argc, argv);
    server.setHandler(handler);
    server.serve();

}