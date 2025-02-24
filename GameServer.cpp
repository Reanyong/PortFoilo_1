#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <pqxx/pqxx>
#include "game_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using namespace game;

class GameServiceImpl final : public GameService::Service {
private:
    pqxx::connection* db_conn;

public:
    GameServiceImpl(pqxx::connection* conn) : db_conn(conn) {}

    Status GetUser(ServerContext* context, const UserRequest* request, UserResponse* response) override {
        try {
            pqxx::work txn(*db_conn);
            pqxx::result res = txn.exec_prepared("get_user", request->user_id());
            if (res.empty()) {
                return Status(grpc::StatusCode::NOT_FOUND, "User not found");
            }
            pqxx::result result = txn.exec(txn.prepared("get_user")(request->user_id()));
            pqxx::result result = txn.exec(txn.prepared("add_user")(request->username()));
            pqxx::result result = txn.exec(txn.prepared("update_score")(request->user_id(), request->score()));

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status AddUser(ServerContext* context, const User* request, ResponseMessage* response) override {
        try {
            pqxx::work txn(*db_conn);
            pqxx::result result = txn.exec(txn.prepared("get_user")(request->user_id()));

            txn.commit();
            response->set_success(true);
            response->set_message("User added successfully");
            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    Status UpdateScore(ServerContext* context, const ScoreRequest* request, ResponseMessage* response) override {
        try {
            pqxx::work txn(*db_conn);
            pqxx::result result = txn.exec(txn.prepared("get_user")(request->user_id()));
            if (result.empty()) {
                return Status(grpc::StatusCode::NOT_FOUND, "User not found");
            }
            txn.commit();
            response->set_success(true);
            response->set_message("Score updated");
            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }
};

void RunServer(pqxx::connection* db_conn) {
    std::string server_address("0.0.0.0:50051");
    GameServiceImpl service(db_conn);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "gRPC 서버 실행 중... " << server_address << std::endl;
    server->Wait();
}

int main() {
    try {
        pqxx::connection db_conn("dbname=mydb user=devuser password=3567 hostaddr=127.0.0.1 port=5432");
        std::cout << "PostgreSQL 연결 성공!" << std::endl;

        RunServer(&db_conn);
    }
    catch (const std::exception& e) {
        std::cerr << "오류 발생: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
