#include <iostream>
#include <pqxx/pqxx>

int main() {
    try {
        pqxx::connection conn("dbname=game_db user=devuser password=devpassword host=localhost");
        if (conn.is_open()) {
            std::cout << "Connected to PostgreSQL successfully!" << std::endl;

            pqxx::work txn(conn);
            pqxx::result result = txn.exec("SELECT username, email FROM user_account");

            for (const auto& row : result) {
                std::cout << "Username: " << row["username"].c_str()
                    << ", Email: " << row["email"].c_str() << std::endl;
            }
        }
        else {
            std::cerr << "Connection failed!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
