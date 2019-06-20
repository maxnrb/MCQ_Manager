#include <pistache/endpoint.h>
#include "HelloHandler.h"
#include "headers/CORS.h"
#include "Database.h"
#include "headers/Authorization.h"

using namespace Pistache;

int main() {
    Http::Header::Registry::instance().registerHeader<CORS>();
    Http::Header::Registry::instance().registerHeader<Authorization>();

    Address addr(Ipv4::any(), Port(8080));

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(std::make_shared<HelloHandler>());
    server.serve();

    return EXIT_SUCCESS;
}