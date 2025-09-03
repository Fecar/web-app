#include "crow.h"
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/mustache.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <ostream>
#include <pqxx/pqxx>
#include <sstream>
#include <stdexcept>
#include <string>

std::string get_postgres_conn_string()
{
    const char* host = std::getenv("DB_HOST");
    const char* user = std::getenv("DB_USER");
    const char* password = std::getenv("DB_PASSWORD");
    const char* dbname = std::getenv("DB_NAME");

    if (!host || !user || !password || !dbname)
    {
        throw std::runtime_error("Variáveis de ambiente do banco de dados não definidas.");
    }

    std::stringstream ss;
    ss << "dbname=" << dbname << " user=" << user << " password=" << password << " host=" << host << " port=5432";
    return ss.str();
}

void sendFileDynamic(crow::response& res, std::string filename, std::string contentType, crow::mustache::context ctx);
void sendHtmlDynamic(crow::response& res, std::string filename, crow::mustache::context ctx);

void sendFile(crow::response& res, std::string filename, std::string contentType);
void sendHtml(crow::response& res, std::string filename);
void sendScript(crow::response& res, std::string filename);
void sendImage(crow::response& res, std::string filename);
void sendStyle(crow::response& res, std::string filename);

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](const crow::request& req, crow::response& res) {
        sendHtml(res, "index");
    });

    CROW_ROUTE(app, "/css/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename) {
        sendStyle(res, filename);
    });

    char* _port = std::getenv("PORT");
    uint16_t PORT = static_cast<uint16_t>(_port != NULL ? std::stoi(_port) : 18080);
    app.port(PORT).multithreaded().run();

    return 0;
}

void sendFile(crow::response& res, std::string filename, std::string contentType)
{

    std::string ROOT_DIR = "./static/";
    std::string filename_path = ROOT_DIR.append(filename);

    try
    {
        std::ifstream inputContent;
        inputContent.open(filename_path, std::ifstream::in);
        inputContent.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        std::ostringstream stringstream;
        stringstream << inputContent.rdbuf();
        inputContent.close();

        res.set_header("Content-Type", contentType);
        res.write(stringstream.str());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        res.code = 404;
        res.write("Content not found ... ");
    }

    res.end();
}

void sendHtml(crow::response& res, std::string filename)
{
    sendFile(res, filename.append(".html"), "text/html");
}

void sendScript(crow::response& res, std::string filename)
{
    std::string path = "js/" + filename;
    sendFile(res, path, "text/script");
}

void sendImage(crow::response& res, std::string filename)
{
    std::string path = "img/" + filename;
    sendFile(res, path, "image/jpeg");
}

void sendStyle(crow::response& res, std::string filename)
{
    sendFile(res, "css/" + filename, "text/css");
}
