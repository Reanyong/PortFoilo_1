//**#**
#include <iostream>
#include <pqxx/pqxx>

int main() {
    try {
        // PostgreSQL 연결 문자열 (환경에 맞게 수정)
        pqxx::connection conn("dbname=game_db user=devuser password=3567 host=localhost port=5432");

        if (conn.is_open()) {
            std::cout << "Connected to PostgreSQL successfully!\n";

            // 트랜잭션 시작
            pqxx::work txn(conn);

            // 사용자 목록 조회
            pqxx::result result = txn.exec("SELECT user_id, username, email FROM user_account");

            // 결과 출력
            std::cout << "User List:\n";
            for (const auto& row : result) {
                std::cout << "ID: " << row["user_id"].as<int>()
                    << " | Username: " << row["username"].c_str()
                    << " | Email: " << row["email"].c_str() << std::endl;
            }
        }
        else {
            std::cerr << "Connection failed!\n";
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
//**#**