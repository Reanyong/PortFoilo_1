#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <pqxx/pqxx>
#include "backend_api.grpc.pb.h"

#ifdef _WIN32
#include <windows.h>
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using backend::BackendService;
using backend::UserRequest;
using backend::UserResponse;
using backend::NewUser;
using backend::ScoreUpdate;
using backend::ScoreResponse;
using backend::RankingRequest;
using backend::RankingResponse;
using backend::RankingEntry;

const std::string DB_CONN = "dbname=fps_game_db user=fps_user password=3567 hostaddr=127.0.0.1 port=5432";

class BackendServiceImpl final : public BackendService::Service {
private:
    std::unique_ptr<pqxx::connection> db_conn;

public:
    BackendServiceImpl() {
        try {
            db_conn = std::make_unique<pqxx::connection>(DB_CONN);
            std::cout << "Successfully connected to database" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Database connection error: " << e.what() << std::endl;
            throw; // 서버를 초기화하지 못하면 중단
        }
    }

    // 유저 정보 조회
    Status GetUser(ServerContext* context, const UserRequest* request, UserResponse* response) override {
        try {
            pqxx::work txn(*db_conn);
            std::string query = "SELECT id, username, email, created_at FROM Users WHERE id = " +
                txn.quote(request->user_id());

            pqxx::result result = txn.exec(query);

            if (result.empty()) {
                response->set_success(false);
                response->set_message("User not found");
                return Status::OK;
            }

            response->set_user_id(result[0]["id"].as<std::string>());
            response->set_username(result[0]["username"].as<std::string>());
            response->set_email(result[0]["email"].as<std::string>());
            response->set_created_at(result[0]["created_at"].as<std::string>());
            response->set_success(true);
            response->set_message("User found successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // 새 유저 추가
    Status AddUser(ServerContext* context, const NewUser* request, UserResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 중복 확인
            std::string check_query = "SELECT COUNT(*) FROM Users WHERE username = " +
                txn.quote(request->username()) + " OR email = " + txn.quote(request->email());

            pqxx::result check_result = txn.exec(check_query);
            if (check_result[0][0].as<int>() > 0) {
                response->set_success(false);
                response->set_message("Username or email already exists");
                return Status::OK;
            }

            // 유저 추가
            std::string query = "INSERT INTO Users (username, email) VALUES (" +
                txn.quote(request->username()) + ", " + txn.quote(request->email()) +
                ") RETURNING id, username, email, created_at";

            pqxx::result result = txn.exec(query);
            txn.commit();

            response->set_user_id(result[0]["id"].as<std::string>());
            response->set_username(result[0]["username"].as<std::string>());
            response->set_email(result[0]["email"].as<std::string>());
            response->set_created_at(result[0]["created_at"].as<std::string>());
            response->set_success(true);
            response->set_message("User created successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // 점수 업데이트
    Status UpdateScore(ServerContext* context, const ScoreUpdate* request, ScoreResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 먼저 유저가 존재하는지 확인
            std::string check_query = "SELECT id FROM Users WHERE id = " +
                txn.quote(request->user_id());

            pqxx::result check_result = txn.exec(check_query);
            if (check_result.empty()) {
                response->set_success(false);
                response->set_message("User not found");
                return Status::OK;
            }

            // 점수가 이미 존재하는지 확인
            std::string score_check = "SELECT id FROM Scores WHERE user_id = " +
                txn.quote(request->user_id());

            pqxx::result score_check_result = txn.exec(score_check);

            std::string query;
            if (score_check_result.empty()) {
                // 새 점수 레코드 생성
                query = "INSERT INTO Scores (user_id, score) VALUES (" +
                    txn.quote(request->user_id()) + ", " +
                    std::to_string(request->score()) +
                    ") RETURNING score";
            }
            else {
                // 기존 점수 업데이트
                query = "UPDATE Scores SET score = " +
                    std::to_string(request->score()) +
                    " WHERE user_id = " +
                    txn.quote(request->user_id()) +
                    " RETURNING score";
            }

            pqxx::result result = txn.exec(query);
            txn.commit();

            response->set_user_id(request->user_id());
            response->set_new_score(result[0]["score"].as<int>());
            response->set_success(true);
            response->set_message("Score updated successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // 랭킹 조회
    Status GetRanking(ServerContext* context, const RankingRequest* request, RankingResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            std::string query =
                "SELECT u.id AS user_id, u.username, s.score, "
                "RANK() OVER (ORDER BY s.score DESC) AS rank "
                "FROM Users u "
                "JOIN Scores s ON u.id = s.user_id "
                "ORDER BY s.score DESC "
                "LIMIT " + std::to_string(request->limit());

            pqxx::result result = txn.exec(query);

            for (const auto& row : result) {
                auto entry = response->add_rankings();
                entry->set_user_id(row["user_id"].as<std::string>());
                entry->set_username(row["username"].as<std::string>());
                entry->set_rank(row["rank"].as<int>());
                entry->set_score(row["score"].as<int>());
            }

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        std::cerr << "Failed to get console mode: " << GetLastError() << std::endl;
    }
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        std::cerr << "Failed to set console mode: " << GetLastError() << std::endl;
    }
#endif

    try {
        std::locale::global(std::locale("ko_KR.UTF-8"));
    }
    catch (const std::exception&) {
        std::cerr << "Failed to set locale, proceeding with default" << std::endl;
    }

    try {
        // ASCII 로고 사용
        std::string logo = R"(
            ██╗      █████╗ ███╗   ██╗ ██████╗ ███████╗
            ██║     ██╔══██╗████╗  ██║██╔════╝║██║ 
            ██║     ███████║██╔██╗ ██║██║     ║███████╗
            ██║     ██╔══██║██║╚██╗██║██║     ║██║ 
            ███████╗██║  ██║██║ ╚████║╚██████╗║███████╗
            ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝
                      Backend API Server
        )";
        std::cout << logo << std::endl;

        // 로딩 효과
        const char* animation = "|/-\\";
        for (int i = 0; i < 10; i++) {
            std::cout << "\rServer is starting... " << animation[i % 4] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        std::cout << "\rServer started!            " << std::endl;

        // 서버 실행
        std::string server_address("0.0.0.0:50051");
        BackendServiceImpl service;

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Backend API Server listening on " << server_address << std::endl;
        server->Wait();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
}