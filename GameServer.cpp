#include <iostream>
#include <memory>
#include <string>
#include <format>
#include <grpcpp/grpcpp.h>
#include <pqxx/pqxx>
#include "game_service.grpc.pb.h"

#ifdef _WIN32
#include <windows.h>
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using namespace game;

class GameServiceImpl final : public GameService::Service {
private:
    std::unique_ptr<pqxx::connection> db_conn;

public:
    GameServiceImpl() {
        try {
            db_conn = std::make_unique<pqxx::connection>(
                "dbname=game_db "
                "user=devuser "
                "password=3567 "
                "hostaddr=127.0.0.1 "
                "port=5432"
            );
            std::cout << "Database connection successful" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Database connection failed: " << e.what() << std::endl;
            throw;
        }
    }

    Status GetUser(ServerContext* context, const UserRequest* request,
        UserResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 매개변수를 문자열로 변환
            std::string query = "SELECT user_id, username, score, created_at "
                "FROM users WHERE user_id = " +
                txn.quote(request->user_id());

            auto result = txn.exec(query);
            if (result.empty()) {
                return Status(grpc::StatusCode::NOT_FOUND, "User not found");
            }

            const auto& row = result[0];
            response->set_user_id(row["user_id"].as<int>());
            response->set_username(row["username"].as<std::string>());
            response->set_score(row["score"].as<int>());
            response->set_created_at(row["created_at"].as<std::string>());

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status AddUser(ServerContext* context, const AddUserRequest* request,
        AddUserResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            std::string query = "INSERT INTO users (username, score) VALUES (" +
                txn.quote(request->username()) + ", 0) RETURNING user_id";

            auto result = txn.exec(query);
            txn.commit();

            response->set_success(true);
            response->set_message("User created successfully");
            response->set_user_id(result[0]["user_id"].as<int>());

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status UpdateScore(ServerContext* context, const UpdateScoreRequest* request,
        UpdateScoreResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            std::string query = "UPDATE users SET score = " +
                txn.quote(request->score()) +
                " WHERE user_id = " +
                txn.quote(request->user_id()) +
                " RETURNING score";

            auto result = txn.exec(query);
            if (result.empty()) {
                return Status(grpc::StatusCode::NOT_FOUND, "User not found");
            }

            txn.commit();

            response->set_success(true);
            response->set_message("Score updated successfully");
            response->set_new_score(result[0]["score"].as<int>());

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status GetRanking(ServerContext* context, const RankingRequest* request,
        RankingResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            std::string query = "SELECT user_id, username, score, "
                "RANK() OVER (ORDER BY score DESC) as rank "
                "FROM users "
                "ORDER BY score DESC "
                "LIMIT " + txn.quote(request->limit());

            auto result = txn.exec(query);

            for (const auto& row : result) {
                auto entry = response->add_rankings();
                entry->set_rank(row["rank"].as<int>());
                entry->set_user_id(row["user_id"].as<int>());
                entry->set_username(row["username"].as<std::string>());
                entry->set_score(row["score"].as<int>());
            }

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }
};

void PrintLogo() {
#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
#endif

    std::string logo = R"(
      ██╗      █████╗ ███╗   ██╗ ██████╗ ███████╗
      ██║     ██╔══██╗████╗  ██║██╔════╝║██║ 
      ██║     ███████║██╔██╗ ██║██║     ║███████╗
      ██║     ██╔══██║██║╚██╗██║██║     ║██║ 
      ███████╗██║  ██║██║ ╚████║╚██████╗║███████╗
      ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝
    )";

    std::cout << logo << std::endl;
}

void LoadingEffect() {
    const char* animation = "|/-\\";
    for (int i = 0; i < 10; i++) {
        std::cout << "\rServer is starting... " << animation[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "\rServer started!            " << std::endl;
}

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GameServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    std::locale::global(std::locale("ko_KR.UTF-8"));
    try {
        PrintLogo();
        LoadingEffect();
        RunServer();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
}