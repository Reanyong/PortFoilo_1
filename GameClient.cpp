#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "backend_api.grpc.pb.h"
#include "matchmaking.grpc.pb.h"

#ifdef _WIN32
#include <windows.h>
#endif

// 백엔드 API 클라이언트
class BackendClient {
private:
    std::unique_ptr<backend::BackendService::Stub> stub_;

public:
    BackendClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(backend::BackendService::NewStub(channel)) {
    }

    // 유저 정보 조회
    bool GetUser(const std::string& user_id) {
        backend::UserRequest request;
        request.set_user_id(user_id);
        backend::UserResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetUser(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 유저 정보 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "유저명: " << response.username() << std::endl;
            std::cout << "이메일: " << response.email() << std::endl;
            std::cout << "가입일: " << response.created_at() << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "=========================" << std::endl;
            return true;
        }
        else {
            std::cerr << "GetUser 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // 유저 추가
    bool AddUser(const std::string& username, const std::string& email) {
        backend::NewUser request;
        request.set_username(username);
        request.set_email(email);
        backend::UserResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->AddUser(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 유저 생성 결과 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "유저명: " << response.username() << std::endl;
            std::cout << "이메일: " << response.email() << std::endl;
            std::cout << "가입일: " << response.created_at() << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "=============================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "AddUser 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // 점수 업데이트
    bool UpdateScore(const std::string& user_id, int score) {
        backend::ScoreUpdate request;
        request.set_user_id(user_id);
        request.set_score(score);
        backend::ScoreResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->UpdateScore(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 점수 업데이트 결과 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "새 점수: " << response.new_score() << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "===============================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "UpdateScore 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // 랭킹 조회
    bool GetRanking(int limit) {
        backend::RankingRequest request;
        request.set_limit(limit);
        backend::RankingResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetRanking(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 랭킹 목록 (상위 " << limit << "명) ========" << std::endl;
            std::cout << "순위\t유저ID\t\t\t\t유저명\t\t점수" << std::endl;
            std::cout << "----------------------------------------------------" << std::endl;

            for (int i = 0; i < response.rankings_size(); i++) {
                const auto& entry = response.rankings(i);
                std::cout << entry.rank() << "\t"
                    << entry.user_id() << "\t"
                    << entry.username() << "\t\t"
                    << entry.score() << std::endl;
            }
            std::cout << "=====================================================" << std::endl;
            return true;
        }
        else {
            std::cerr << "GetRanking 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }
};

// 매치메이킹 클라이언트
class MatchmakingClient {
private:
    std::unique_ptr<matchmaking::MatchmakingService::Stub> stub_;

public:
    MatchmakingClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(matchmaking::MatchmakingService::NewStub(channel)) {
    }

    // 매치메이킹 참가
    bool JoinMatchmaking(const std::string& user_id, const std::string& game_mode, const std::string& region) {
        matchmaking::JoinRequest request;
        request.set_user_id(user_id);
        request.set_game_mode(game_mode);
        request.set_region(region);
        matchmaking::MatchmakingResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->JoinMatchmaking(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 매치메이킹 참가 결과 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "상태: " << response.status() << std::endl;
            std::cout << "신청 시간: " << response.created_at() << std::endl;
            std::cout << "예상 대기 시간: " << response.estimated_wait_time() << "초" << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "===================================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "JoinMatchmaking 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // 매치메이킹 취소
    bool CancelMatchmaking(const std::string& user_id) {
        matchmaking::CancelRequest request;
        request.set_user_id(user_id);
        matchmaking::MatchmakingResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->CancelMatchmaking(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 매치메이킹 취소 결과 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "상태: " << response.status() << std::endl;
            std::cout << "신청 시간: " << response.created_at() << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "===================================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "CancelMatchmaking 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // 매치메이킹 상태 확인
    bool GetMatchmakingStatus(const std::string& user_id) {
        matchmaking::StatusRequest request;
        request.set_user_id(user_id);
        matchmaking::MatchmakingResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetMatchmakingStatus(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== 매치메이킹 상태 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "상태: " << response.status() << std::endl;

            if (!response.match_id().empty()) {
                std::cout << "매치 ID: " << response.match_id() << std::endl;
            }

            if (!response.server_address().empty()) {
                std::cout << "서버 주소: " << response.server_address() << std::endl;
            }

            if (!response.created_at().empty()) {
                std::cout << "신청 시간: " << response.created_at() << std::endl;
            }

            if (response.status() == "waiting" && response.estimated_wait_time() > 0) {
                std::cout << "예상 대기 시간: " << response.estimated_wait_time() << "초" << std::endl;
            }

            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "===============================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "GetMatchmakingStatus 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }

    // MMR 조회
    bool GetMMR(const std::string& user_id) {
        matchmaking::MMRRequest request;
        request.set_user_id(user_id);
        matchmaking::MMRResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetMMR(&context, request, &response);

        if (status.ok()) {
            std::cout << "======== MMR 정보 ========" << std::endl;
            std::cout << "유저 ID: " << response.user_id() << std::endl;
            std::cout << "MMR: " << response.mmr() << std::endl;
            std::cout << "랭킹: " << response.rank() << std::endl;
            std::cout << "성공: " << (response.success() ? "예" : "아니오") << std::endl;
            std::cout << "메시지: " << response.message() << std::endl;
            std::cout << "=========================" << std::endl;
            return response.success();
        }
        else {
            std::cerr << "GetMMR 실패: " << status.error_code() << ": " << status.error_message() << std::endl;
            return false;
        }
    }
};

// 로고 출력 함수
void PrintLogo() {
    std::string logo = R"(
      ██╗      █████╗ ███╗   ██╗ ██████╗ ███████╗
      ██║     ██╔══██╗████╗  ██║██╔════╝║██║ 
      ██║     ███████║██╔██╗ ██║██║     ║███████╗
      ██║     ██╔══██║██║╚██╗██║██║     ║██║ 
      ███████╗██║  ██║██║ ╚████║╚██████╗║███████╗
      ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝
                      클라이언트
    )";

    std::cout << logo << std::endl;
}

// 로딩 효과 함수
void LoadingEffect(const std::string& message) {
    const char* animation = "|/-\\";
    for (int i = 0; i < 10; i++) {
        std::cout << "\r" << message << " " << animation[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "\r" << message << " 완료!      " << std::endl;
}

// 백엔드 API 서버 메뉴
int ShowBackendMenu() {
    int choice;
    std::cout << "\n======== 백엔드 API 서버 ========" << std::endl;
    std::cout << "1. 유저 정보 조회" << std::endl;
    std::cout << "2. 새 유저 등록" << std::endl;
    std::cout << "3. 점수 업데이트" << std::endl;
    std::cout << "4. 랭킹 조회" << std::endl;
    std::cout << "0. 메인 메뉴로 돌아가기" << std::endl;
    std::cout << "선택: ";
    std::cin >> choice;
    return choice;
}

// 매치메이킹 서버 메뉴
int ShowMatchmakingMenu() {
    int choice;
    std::cout << "\n======== 매치메이킹 서버 ========" << std::endl;
    std::cout << "1. 매치메이킹 참가" << std::endl;
    std::cout << "2. 매치메이킹 취소" << std::endl;
    std::cout << "3. 매치메이킹 상태 확인" << std::endl;
    std::cout << "4. MMR 조회" << std::endl;
    std::cout << "0. 메인 메뉴로 돌아가기" << std::endl;
    std::cout << "선택: ";
    std::cin >> choice;
    return choice;
}

// 메인 메뉴
int ShowMainMenu() {
    int choice;
    std::cout << "\n======== LANOS GAME 클라이언트 ========" << std::endl;
    std::cout << "1. 백엔드 API 서버" << std::endl;
    std::cout << "2. 매치메이킹 서버" << std::endl;
    std::cout << "0. 종료" << std::endl;
    std::cout << "선택: ";
    std::cin >> choice;
    return choice;
}

int main(int argc, char** argv) {
#ifdef _WIN32
    // Windows에서 콘솔 출력을 UTF-8로 설정
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    PrintLogo();
    LoadingEffect("클라이언트 초기화 중");

    // 서버 주소 설정
    std::string backend_address = "localhost:50051";
    std::string matchmaking_address = "localhost:50052";

    if (argc > 1) {
        backend_address = argv[1];
    }
    if (argc > 2) {
        matchmaking_address = argv[2];
    }

    std::cout << "백엔드 API 서버 주소: " << backend_address << std::endl;
    std::cout << "매치메이킹 서버 주소: " << matchmaking_address << std::endl;

    // 클라이언트 초기화
    BackendClient backend_client(
        grpc::CreateChannel(backend_address, grpc::InsecureChannelCredentials())
    );

    MatchmakingClient matchmaking_client(
        grpc::CreateChannel(matchmaking_address, grpc::InsecureChannelCredentials())
    );

    while (true) {
        int main_choice = ShowMainMenu();

        if (main_choice == 0) {
            std::cout << "프로그램을 종료합니다." << std::endl;
            break;
        }
        else if (main_choice == 1) {
            // 백엔드 API 서버 메뉴
            while (true) {
                int backend_choice = ShowBackendMenu();

                if (backend_choice == 0) {
                    break; // 메인 메뉴로 돌아가기
                }

                switch (backend_choice) {
                case 1: {
                    std::string user_id;
                    std::cout << "조회할 유저 ID 입력: ";
                    std::cin >> user_id;
                    backend_client.GetUser(user_id);
                    break;
                }
                case 2: {
                    std::string username, email;
                    std::cout << "신규 유저명 입력: ";
                    std::cin >> username;
                    std::cout << "이메일 입력: ";
                    std::cin >> email;
                    backend_client.AddUser(username, email);
                    break;
                }
                case 3: {
                    std::string user_id;
                    int score;
                    std::cout << "점수 업데이트할 유저 ID 입력: ";
                    std::cin >> user_id;
                    std::cout << "새 점수 입력: ";
                    std::cin >> score;
                    backend_client.UpdateScore(user_id, score);
                    break;
                }
                case 4: {
                    int limit;
                    std::cout << "조회할 랭킹 수 입력: ";
                    std::cin >> limit;
                    backend_client.GetRanking(limit);
                    break;
                }
                default:
                    std::cout << "잘못된 선택입니다." << std::endl;
                    break;
                }
            }
        }
        else if (main_choice == 2) {
            // 매치메이킹 서버 메뉴
            while (true) {
                int matchmaking_choice = ShowMatchmakingMenu();

                if (matchmaking_choice == 0) {
                    break; // 메인 메뉴로 돌아가기
                }

                switch (matchmaking_choice) {
                case 1: {
                    std::string user_id, game_mode, region;
                    std::cout << "유저 ID 입력: ";
                    std::cin >> user_id;
                    std::cout << "게임 모드 입력 (battlefield/deathmatch): ";
                    std::cin >> game_mode;
                    std::cout << "지역 입력 (asia/us/eu): ";
                    std::cin >> region;
                    matchmaking_client.JoinMatchmaking(user_id, game_mode, region);
                    break;
                }
                case 2: {
                    std::string user_id;
                    std::cout << "유저 ID 입력: ";
                    std::cin >> user_id;
                    matchmaking_client.CancelMatchmaking(user_id);
                    break;
                }
                case 3: {
                    std::string user_id;
                    std::cout << "유저 ID 입력: ";
                    std::cin >> user_id;
                    matchmaking_client.GetMatchmakingStatus(user_id);
                    break;
                }
                case 4: {
                    std::string user_id;
                    std::cout << "유저 ID 입력: ";
                    std::cin >> user_id;
                    matchmaking_client.GetMMR(user_id);
                    break;
                }
                default:
                    std::cout << "잘못된 선택입니다." << std::endl;
                    break;
                }
            }
        }
        else {
            std::cout << "잘못된 선택입니다." << std::endl;
        }
    }

    return 0;
}