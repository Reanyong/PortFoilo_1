#include "crow_all.h"
#include <pqxx/pqxx>
#include <iostream>

crow::json::wvalue getUsers() {
    crow::json::wvalue users;
    try {
        pqxx::connection conn("dbname=game_db user=devuser password=3567 host=localhost port=5432");
        pqxx::work txn(conn);
        pqxx::result result = txn.exec("SELECT user_id, username FROM user_account");

        for (const auto& row : result) {
            users["users"].push_back({
                {"id", row["user_id"].as<int>()},
                {"username", row["username"].c_str()}
                });
        }
    }
    catch (const std::exception& e) {
        users["error"] = e.what();
    }
    return users;
}

int main() {
    crow::SimpleApp app;

    // 유저 목록 조회 API
    CROW_ROUTE(app, "/users").methods("GET"_method)([]() {
        return getUsers();
        });

    app.port(18080).multithreaded().run();
}
