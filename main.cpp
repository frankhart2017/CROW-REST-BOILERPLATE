#include "crow_all.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <boost/filesystem.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "sha512.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using mongocxx::cursor;

using namespace std;
using namespace crow;
using namespace crow::mustache;

void getView(response &res, const string &filename, context &x) {
  res.set_header("Content-Type", "text/html");
  auto text = load("../public/" + filename + ".html").render(x);
  res.write(text);
  res.end();
}

void sendFile(response &res, string filename, string contentType) {
  ifstream in("../public/" + filename, ifstream::in);
  if(in) {
    ostringstream contents;
    contents << in.rdbuf();
    in.close();
    res.set_header("Content-Type", contentType);
    res.write(contents.str());
  } else {
    res.code = 404;
    res.write("Not found");
  }
  res.end();
}

void sendHtml(response &res, string filename) {
  sendFile(res, filename + ".html", "text/html");
}

void sendImage(response &res, string filename) {
  sendFile(res, "images/" + filename, "image/jpeg");
}

void sendScript(response &res, string filename) {
  sendFile(res, "scripts/" + filename, "text/javascript");
}

void sendStyle(response &res, string filename) {
  sendFile(res, "styles/" + filename, "text/css");
}

void notFound(response &res, const string &message) {
  res.code = 404;
  res.write(message + ": Not Found");
  res.end();
}

int main(int argc, char *argv[]) {

  crow::SimpleApp app;
  set_base(".");

  mongocxx::instance inst{};

  string mongoConnect = std::string(getenv("MONGODB_URI"));

  mongocxx::client conn{mongocxx::uri{mongoConnect}};

  auto collection = conn["heroku_nqv061hq"]["users"];

  CROW_ROUTE(app, "/signup/<string>/<string>/<string>").methods(HTTPMethod::Post)
    ([&collection](const request &req, response &res, string name, string email, string password){

      auto doc = collection.find_one(
        make_document(kvp("email", email)));

      res.set_header("Content-Type", "application/json");

      crow::json::wvalue ret;

      if(doc) {
        ret["status"] = "Email already taken!";
        res.write(crow::json::dump(ret));
        res.end();
      } else {
        auto builder = bsoncxx::builder::stream::document{};
        password = sha512(password);
        bsoncxx::document::value doc = builder
          << "name" << name
          << "email" << email
          << "password" << password
          << bsoncxx::builder::stream::finalize;

        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(doc.view());


        ret["status"] = "Signed up successfully!";


        res.write(crow::json::dump(ret));
        res.end();
      }

    });

    CROW_ROUTE(app, "/users")
    ([&collection](){
      mongocxx::options::find opts;
      opts.limit(10);
      auto docs = collection.find({}, opts);

      std::ostringstream os;
      for(auto &&doc : docs) {
        os << bsoncxx::to_json(doc)<< "\n";
      }
      return crow::response(os.str());
    });

  char* port = getenv("PORT");
  uint16_t iPort = static_cast<uint16_t>(port != NULL? stoi(port): 18080);
  cout<<"PORT = "<<iPort<<"\n";

  app.port(iPort).multithreaded().run();

  return 0;
}
