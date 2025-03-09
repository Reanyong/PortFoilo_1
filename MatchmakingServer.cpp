#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <vector>
#include <map>
#include <algorithm>
#include <grpcpp/grpcpp.h>
#include <pqxx/pqxx>
#include "matchmaking.grpc.pb.h"

#ifdef _WIN32
#include <windows.h>
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using matchmaking::MatchmakingService;
using matchmaking::JoinRequest;
using matchmaking::CancelRequest;
using matchmaking::StatusRequest;
using matchmaking::MatchmakingResponse;
using matchmaking::MMRRequest;
using matchmaking::MMRUpdate;
using matchmaking::MMRResponse;

const std::string DB_CONN = "dbname=fps_game_db user=fps_user password=3567 hostaddr=127.0.0.1 port=5432";

// 매치메이킹 큐 플레이어 정보를 담는 구조체
struct PlayerInfo {
    std::string user_id;
    std::string game_mode;
    std::string region;
    float mmr;
    std::chrono::system_clock::time_point join_time;
};

// 매치 그룹 구조체
struct MatchGroup {
    std::string match_id;
    std::string game_mode;
    std::string region;
    std::vector<std::string> player_ids;
    std::string server_address;
    std::chrono::system_clock::time_point created_at;
};

// 매치메이킹 큐 관리 클래스
class MatchmakingQueue {
private:
    std::vector<PlayerInfo> queue;
    std::vector<MatchGroup> active_matches;
    std::mutex queue_mutex;
    std::mutex match_mutex;

public:
    // 큐에 플레이어 추가
    void AddPlayer(const PlayerInfo& player) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        // 이미 큐에 있는지 확인
        auto it = std::find_if(queue.begin(), queue.end(),
            [&](const PlayerInfo& p) { return p.user_id == player.user_id; });

        if (it != queue.end()) {
            *it = player; // 정보 업데이트
        }
        else {
            queue.push_back(player); // 새로 추가
        }
    }

    // 큐에서 플레이어 제거
    bool RemovePlayer(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto it = std::find_if(queue.begin(), queue.end(),
            [&](const PlayerInfo& p) { return p.user_id == user_id; });

        if (it != queue.end()) {
            queue.erase(it);
            return true;
        }
        return false;
    }

    // 플레이어 큐 상태 확인
    bool GetPlayerStatus(const std::string& user_id, PlayerInfo& player_info) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto it = std::find_if(queue.begin(), queue.end(),
            [&](const PlayerInfo& p) { return p.user_id == user_id; });

        if (it != queue.end()) {
            player_info = *it;
            return true;
        }
        return false;
    }

    // 플레이어가 매치에 있는지 확인
    bool IsPlayerInMatch(const std::string& user_id, MatchGroup& match) {
        std::lock_guard<std::mutex> lock(match_mutex);
        for (const auto& m : active_matches) {
            auto it = std::find(m.player_ids.begin(), m.player_ids.end(), user_id);
            if (it != m.player_ids.end()) {
                match = m;
                return true;
            }
        }
        return false;
    }

    // 매치 추가
    void AddMatch(const MatchGroup& match) {
        std::lock_guard<std::mutex> lock(match_mutex);
        active_matches.push_back(match);
    }

    // 큐 처리 및 매치 생성 (주기적으로 호출)
    std::vector<MatchGroup> ProcessQueue() {
        std::lock_guard<std::mutex> queue_lock(queue_mutex);
        std::lock_guard<std::mutex> match_lock(match_mutex);

        std::vector<MatchGroup> new_matches;
        if (queue.empty()) return new_matches;

        // 게임 모드와 지역별로 플레이어 그룹화
        std::map<std::string, std::vector<PlayerInfo>> groups;
        for (const auto& player : queue) {
            std::string key = player.game_mode + "_" + player.region;
            groups[key].push_back(player);
        }

        // 각 그룹에서 충분한 플레이어가 모이면 매치 생성
        for (auto& group_pair : groups) {
            auto& players = group_pair.second;

            // 게임 모드에 따른 필요 플레이어 수 설정
            int required_players = 16; // 기본값 (데스매치)

            std::string game_mode = players[0].game_mode;
            std::string region = players[0].region;

            if (game_mode == "battlefield") {
                required_players = 64; // 배틀필드 모드
            }

            // 충분한 플레이어가 모이면 매치 생성
            if (players.size() >= required_players) {
                // MMR 기준으로 정렬
                std::sort(players.begin(), players.end(),
                    [](const PlayerInfo& a, const PlayerInfo& b) { return a.mmr < b.mmr; });

                // 매치 ID 생성 (단순화를 위해 타임스탬프 사용)
                auto now = std::chrono::system_clock::now();
                auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()).count();
                std::string match_id = "match_" + std::to_string(timestamp);

                // 서버 주소 할당 (지역에 따라 다른 주소 할당 가능)
                std::string server_address = "game-server-" + region + ".lanos.com:10000";

                // 매치 생성
                MatchGroup match;
                match.match_id = match_id;
                match.game_mode = game_mode;
                match.region = region;
                match.server_address = server_address;
                match.created_at = now;

                // 필요한 수의 플레이어 추가
                for (int i = 0; i < required_players && i < players.size(); i++) {
                    match.player_ids.push_back(players[i].user_id);

                    // 큐에서 제거
                    auto it = std::find_if(queue.begin(), queue.end(),
                        [&](const PlayerInfo& p) { return p.user_id == players[i].user_id; });
                    if (it != queue.end()) {
                        queue.erase(it);
                    }
                }

                new_matches.push_back(match);
                active_matches.push_back(match);
            }
        }

        return new_matches;
    }
};

class MatchmakingServiceImpl final : public MatchmakingService::Service {
private:
    std::unique_ptr<pqxx::connection> db_conn;
    MatchmakingQueue match_queue;
    std::thread matchmaking_thread;
    bool running;

public:
    MatchmakingServiceImpl() : running(true) {
        try {
            db_conn = std::make_unique<pqxx::connection>(DB_CONN);
            std::cout << "Successfully connected to database" << std::endl;

            // 매치메이킹 처리 스레드 시작
            matchmaking_thread = std::thread(&MatchmakingServiceImpl::ProcessMatchmaking, this);
        }
        catch (const std::exception& e) {
            std::cerr << "Database connection error: " << e.what() << std::endl;
            throw;
        }
    }

    ~MatchmakingServiceImpl() {
        running = false;
        if (matchmaking_thread.joinable()) {
            matchmaking_thread.join();
        }
    }

    // 매치메이킹 큐 처리 스레드
    void ProcessMatchmaking() {
        while (running) {
            try {
                // 주기적으로 큐 처리
                auto new_matches = match_queue.ProcessQueue();

                // 새로 생성된 매치 처리
                for (const auto& match : new_matches) {
                    try {
                        pqxx::work txn(*db_conn);

                        // 매치 정보 저장
                        std::string match_query = "INSERT INTO Matches (match_id, game_mode, region, server_address, created_at) VALUES (" +
                            txn.quote(match.match_id) + ", " +
                            txn.quote(match.game_mode) + ", " +
                            txn.quote(match.region) + ", " +
                            txn.quote(match.server_address) + ", " +
                            "NOW()) RETURNING match_id";

                        txn.exec(match_query);

                        // 매치 플레이어 정보 저장
                        for (const auto& player_id : match.player_ids) {
                            std::string player_query = "UPDATE Matchmaking SET status = 'matched', match_id = " +
                                txn.quote(match.match_id) + ", server_address = " + txn.quote(match.server_address) +
                                " WHERE user_id = " + txn.quote(player_id) +
                                " AND status = 'waiting'";

                            txn.exec(player_query);
                        }

                        txn.commit();
                        std::cout << "Created new match: " << match.match_id << " with " << match.player_ids.size() << " players" << std::endl;
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Error processing match: " << e.what() << std::endl;
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Matchmaking processing error: " << e.what() << std::endl;
            }

            // 5초마다 처리
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    // 매치메이킹 참가
    Status JoinMatchmaking(ServerContext* context, const JoinRequest* request, MatchmakingResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 유저 존재 확인
            std::string check_query = "SELECT id FROM Users WHERE id = " +
                txn.quote(request->user_id());

            pqxx::result check_result = txn.exec(check_query);
            if (check_result.empty()) {
                response->set_success(false);
                response->set_message("User not found");
                return Status::OK;
            }

            // 이미 매치메이킹 중인지 확인
            std::string existing_query = "SELECT status, created_at FROM Matchmaking WHERE user_id = " +
                txn.quote(request->user_id()) + " AND status = 'waiting'";

            pqxx::result existing_result = txn.exec(existing_query);
            if (!existing_result.empty()) {
                response->set_user_id(request->user_id());
                response->set_status("waiting");
                response->set_created_at(existing_result[0]["created_at"].as<std::string>());
                response->set_estimated_wait_time(300); // 예상 대기 시간 (초)
                response->set_success(true);
                response->set_message("Already in matchmaking queue");
                return Status::OK;
            }

            // 이미 매치가 성사된 상태인지 확인
            std::string matched_query = "SELECT status, match_id, server_address, created_at FROM Matchmaking WHERE user_id = " +
                txn.quote(request->user_id()) + " AND status = 'matched'";

            pqxx::result matched_result = txn.exec(matched_query);
            if (!matched_result.empty()) {
                response->set_user_id(request->user_id());
                response->set_status("matched");
                response->set_match_id(matched_result[0]["match_id"].as<std::string>());
                response->set_server_address(matched_result[0]["server_address"].as<std::string>());
                response->set_created_at(matched_result[0]["created_at"].as<std::string>());
                response->set_success(true);
                response->set_message("Already matched to a game");
                return Status::OK;
            }

            // MMR 조회
            std::string mmr_query = "SELECT mmr FROM MMR_Calculations WHERE user_id = " +
                txn.quote(request->user_id()) + " ORDER BY calculated_at DESC LIMIT 1";

            pqxx::result mmr_result = txn.exec(mmr_query);
            float mmr = 1000.0f; // 기본 MMR
            if (!mmr_result.empty()) {
                mmr = mmr_result[0]["mmr"].as<float>();
            }

            // 매치메이킹 큐에 추가
            PlayerInfo player_info;
            player_info.user_id = request->user_id();
            player_info.game_mode = request->game_mode();
            player_info.region = request->region();
            player_info.mmr = mmr;
            player_info.join_time = std::chrono::system_clock::now();

            match_queue.AddPlayer(player_info);

            // 매치메이킹 기록 저장
            std::string insert_query = "INSERT INTO Matchmaking (user_id, game_mode, region, status, created_at) VALUES (" +
                txn.quote(request->user_id()) + ", " +
                txn.quote(request->game_mode()) + ", " +
                txn.quote(request->region()) + ", " +
                "'waiting', NOW()) RETURNING created_at";

            pqxx::result result = txn.exec(insert_query);
            txn.commit();

            response->set_user_id(request->user_id());
            response->set_status("waiting");
            response->set_created_at(result[0]["created_at"].as<std::string>());
            response->set_estimated_wait_time(300); // 예상 대기 시간 (초)
            response->set_success(true);
            response->set_message("Successfully joined matchmaking queue");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // 매치메이킹 취소
    Status CancelMatchmaking(ServerContext* context, const CancelRequest* request, MatchmakingResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 진행 중인 매치메이킹 찾기
            std::string query = "UPDATE Matchmaking SET status = 'cancelled' "
                "WHERE user_id = " + txn.quote(request->user_id()) +
                " AND status = 'waiting' "
                "RETURNING created_at";

            pqxx::result result = txn.exec(query);

            // 큐에서 제거
            bool removed = match_queue.RemovePlayer(request->user_id());

            if (result.empty() || !removed) {
                response->set_success(false);
                response->set_message("No active matchmaking request found");
                return Status::OK;
            }

            txn.commit();

            response->set_user_id(request->user_id());
            response->set_status("cancelled");
            response->set_created_at(result[0]["created_at"].as<std::string>());
            response->set_success(true);
            response->set_message("Matchmaking cancelled successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // 매치메이킹 상태 확인
    Status GetMatchmakingStatus(ServerContext* context, const StatusRequest* request, MatchmakingResponse* response) override {
        try {
            // 매치에 있는지 확인
            MatchGroup match;
            bool in_match = match_queue.IsPlayerInMatch(request->user_id(), match);

            if (in_match) {
                response->set_user_id(request->user_id());
                response->set_status("matched");
                response->set_match_id(match.match_id);
                response->set_server_address(match.server_address);
                response->set_created_at(std::format("{:%Y-%m-%d %H:%M:%S}", match.created_at));
                response->set_success(true);
                response->set_message("Player is in active match");
                return Status::OK;
            }

            // 큐에 있는지 확인
            PlayerInfo player_info;
            bool in_queue = match_queue.GetPlayerStatus(request->user_id(), player_info);

            if (in_queue) {
                response->set_user_id(request->user_id());
                response->set_status("waiting");

                // 예상 대기 시간 계산
                auto now = std::chrono::system_clock::now();
                auto waited = std::chrono::duration_cast<std::chrono::seconds>(
                    now - player_info.join_time).count();

                // 간단한 예상 - 최대 5분(300초) 대기, 이미 대기한 시간 차감
                int estimated = std::max(0, 300 - static_cast<int>(waited));
                response->set_estimated_wait_time(estimated);

                response->set_success(true);
                response->set_message("Player is in matchmaking queue");
                return Status::OK;
            }

            // DB에서 최근 상태 확인
            pqxx::work txn(*db_conn);
            std::string query = "SELECT status, match_id, server_address, created_at FROM Matchmaking "
                "WHERE user_id = " + txn.quote(request->user_id()) +
                " ORDER BY created_at DESC LIMIT 1";

            pqxx::result result = txn.exec(query);

            if (result.empty()) {
                response->set_user_id(request->user_id());
                response->set_status("not_in_queue");
                response->set_success(true);
                response->set_message("No matchmaking history found");
                return Status::OK;
            }

            response->set_user_id(request->user_id());
            response->set_status(result[0]["status"].as<std::string>());
            response->set_created_at(result[0]["created_at"].as<std::string>());

            // 매치가 성사된 경우
            if (!result[0]["match_id"].is_null()) {
                response->set_match_id(result[0]["match_id"].as<std::string>());
            }

            // 서버 주소가 할당된 경우
            if (!result[0]["server_address"].is_null()) {
                response->set_server_address(result[0]["server_address"].as<std::string>());
            }

            response->set_success(true);
            response->set_message("Matchmaking status retrieved successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // MMR 조회
    Status GetMMR(ServerContext* context, const MMRRequest* request, MMRResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // MMR 조회
            std::string query = "SELECT mmr, calculated_at FROM MMR_Calculations "
                "WHERE user_id = " + txn.quote(request->user_id()) +
                " ORDER BY calculated_at DESC LIMIT 1";

            pqxx::result result = txn.exec(query);

            if (result.empty()) {
                response->set_user_id(request->user_id());
                response->set_mmr(1000.0); // 기본 MMR
                response->set_rank(0);
                response->set_success(true);
                response->set_message("No MMR data found, using default");
                return Status::OK;
            }

            float mmr = result[0]["mmr"].as<float>();

            // 랭킹 조회
            std::string rank_query = "SELECT COUNT(*) + 1 as rank FROM MMR_Calculations "
                "WHERE mmr > " + std::to_string(mmr);

            pqxx::result rank_result = txn.exec(rank_query);
            int rank = rank_result[0]["rank"].as<int>();

            response->set_user_id(request->user_id());
            response->set_mmr(mmr);
            response->set_rank(rank);
            response->set_success(true);
            response->set_message("MMR retrieved successfully");

            return Status::OK;
        }
        catch (const std::exception& e) {
            return Status(grpc::StatusCode::INTERNAL, e.what());
        }
    }

    // MMR 업데이트
    Status UpdateMMR(ServerContext* context, const MMRUpdate* request, MMRResponse* response) override {
        try {
            pqxx::work txn(*db_conn);

            // 유저 존재 확인
            std::string check_query = "SELECT id FROM Users WHERE id = " +
                txn.quote(request->user_id());

            pqxx::result check_result = txn.exec(check_query);
            if (check_result.empty()) {
                response->set_success(false);
                response->set_message("User not found");
                return Status::OK;
            }

            // MMR 업데이트
            std::string insert_query = "INSERT INTO MMR_Calculations (user_id, mmr, calculated_at) VALUES (" +
                txn.quote(request->user_id()) + ", " +
                std::to_string(request->mmr()) + ", " +
                "NOW()) RETURNING mmr";

            pqxx::result result = txn.exec(insert_query);

            // 랭킹 계산
            std::string rank_query = "SELECT COUNT(*) + 1 as rank FROM MMR_Calculations "
                "WHERE mmr > " + std::to_string(request->mmr());

            pqxx::result rank_result = txn.exec(rank_query);
            int rank = rank_result[0]["rank"].as<int>();

            txn.commit();

            response->set_user_id(request->user_id());
            response->set_mmr(result[0]["mmr"].as<float>());
            response->set_rank(rank);
            response->set_success(true);
            response->set_message("MMR updated successfully");

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
                      Matchmaking Server
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
        std::string server_address("0.0.0.0:50052");
        MatchmakingServiceImpl service;

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Matchmaking Server listening on " << server_address << std::endl;
        server->Wait();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
}