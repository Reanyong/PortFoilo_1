# 🎮 **게임 서비스 백엔드 (C++ gRPC & PostgreSQL)**

---

## **프로젝트 개요**
이 프로젝트는 **온라인 게임의 백엔드 시스템**을 C++로 구현하여 사용자 관리, 점수 업데이트, 랭킹 시스템 등 핵심 기능을 제공합니다.  
**gRPC 프레임워크**를 활용하여 **효율적인 클라이언트-서버 통신**을 구축하고,  
**PostgreSQL 데이터베이스**를 통해 **안정적이고 확장 가능한 백엔드 시스템**을 구현했습니다.  

---

## **기술 스택**

| **항목**      | **기술**                                          |
| ------------ | ------------------------------------------------ |
| 언어         | C++17                                             |
| IDE          | Visual Studio 2022                                |
| 프레임워크    | gRPC, Protocol Buffers, libpqxx                    |
| 데이터베이스 | PostgreSQL (WSL Ubuntu 환경)                       |
| 빌드 관리    | CMake, Visual Studio 프로젝트 시스템               |

---

## **주요 기능**

### **사용자 관리**
- 신규 사용자 등록 (`AddUser`)
- 사용자 정보 조회 (`GetUser`)
- 안전한 데이터베이스 저장

### **게임 성적 관리**
- 사용자 점수 업데이트 (`UpdateScore`)
- 실시간 점수 변경사항 반영

### **랭킹 시스템**
- 상위 N명 플레이어 랭킹 조회 (`GetRanking`)
- 점수 기반 자동 순위 계산

### **클라이언트-서버 통신**
- gRPC 기반의 효율적인 바이너리 통신
- Protocol Buffers를 활용한 데이터 직렬화 및 역직렬화

---

## 📂 **프로젝트 구조**
```
DB_Connection/
├── protos/
│   └── game_service.proto     # gRPC 서비스 및 메시지 정의
├── src/
│   ├── game_service.pb.h      # 생성된 Protocol Buffers 헤더
│   ├── game_service.pb.cc     # 생성된 Protocol Buffers 구현
│   ├── game_service.grpc.pb.h # 생성된 gRPC 서비스 헤더
│   └── game_service.grpc.pb.cc# 생성된 gRPC 서비스 구현
├── GameServer.cpp             # 서버 구현
├── GameClient.cpp             # 클라이언트 구현
├── CRUD_PostgreSQL.cpp        # 데이터베이스 CRUD 작업
├── DB_Connection.sln          # Visual Studio 솔루션 파일
└── DB_Connection.vcxproj      # Visual Studio 프로젝트 파일
```

---

## **구현 세부 사항**

### 📑 **gRPC 서비스 정의 (Protocol Buffers)**
gRPC를 통해 클라이언트와 서버 간 **효율적인 통신**을 구현했습니다.  
`game_service.proto` 파일을 통해 **서비스와 메시지 형식**을 정의하여 코드의 자동 생성을 활용했습니다.  

#### **gRPC 서비스 정의 예제**
```proto
service GameService {
  // 유저 정보 조회
  rpc GetUser (UserRequest) returns (UserResponse) {}
  // 새 유저 추가
  rpc AddUser (AddUserRequest) returns (AddUserResponse) {}
  // 점수 업데이트
  rpc UpdateScore (UpdateScoreRequest) returns (UpdateScoreResponse) {}
  // 랭킹 조회
  rpc GetRanking (RankingRequest) returns (RankingResponse) {}
}
```

---

## **서버 구현**
GameServiceImpl 클래스는 gRPC 서비스를 구현하며,
각 메소드는 PostgreSQL과 직접 통신하여 요청을 처리합니다.
데이터베이스와의 연결은 libpqxx를 사용하여 안전하게 관리했습니다.

### **서버 메소드 예제**
```C++
Status GetUser(ServerContext* context, const UserRequest* request,
               UserResponse* response) override {
    try {
        pqxx::work txn(*db_conn);
        std::string query = "SELECT user_id, username, score, created_at "
                            "FROM users WHERE user_id = " + txn.quote(request->user_id());

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
    } catch (const std::exception& e) {
        return Status(grpc::StatusCode::INTERNAL, e.what());
    }
}
```
---
## **클라이언트 구현**
클라이언트는 gRPC를 통해 서버와 직접 통신하며,
점수 조회와 랭킹 조회 요청을 처리하여 결과를 출력합니다.

### **클라이언트 메소드 예제**
```C++
bool GetRanking(int limit) {
    RankingRequest request;
    request.set_limit(limit);
    
    RankingResponse response;
    ClientContext context;
    
    Status status = stub_->GetRanking(&context, request, &response);
    if (status.ok()) {
        std::cout << "======== 상위 " << limit << "명 랭킹 ========" << std::endl;
        for (const auto& player : response.players()) {
            std::cout << "ID: " << player.user_id()
                      << " | Name: " << player.username()
                      << " | Score: " << player.score() << std::endl;
        }
        return true;
    } else {
        std::cerr << "GetRanking 실패: " << status.error_message() << std::endl;
        return false;
    }
}
```
---
## **시스템 요구사항**
운영체제: Windows 11 또는 Linux (Ubuntu WSL)
IDE: Visual Studio 2019 이상
데이터베이스: PostgreSQL 14 이상
컴파일러: C++17 호환 컴파일러
필수 라이브러리
gRPC
Protocol Buffers
libpqxx

---

## **향후 계획**
인증 시스템: JWT 기반 사용자 인증 구현
로드 밸런싱: 다중 서버 환경에서의 부하 분산 전략 적용
캐싱 메커니즘: Redis를 활용하여 빈번한 요청에 대한 캐싱 시스템 도입
로그 시스템: 구조화된 로깅 및 모니터링 시스템 통합
마이크로서비스 아키텍처: 기능별 독립적인 서비스로 분리
서버 추가 : 채팅 서버(Boost.Asio), 게임 세션 서버(Boost.Asio UDP)

---

## **결론**
이 프로젝트는 고성능 C++ 백엔드 시스템의 설계와 구현에 대한 실질적인 접근 방식을 보여줍니다.
gRPC와 PostgreSQL의 통합을 통해 안정적이고 확장 가능한 게임 서비스 백엔드를 구현하는 데 중점을 두었으며,
Modern C++ 기능과 업계 표준 라이브러리를 적극 활용하여 유연하고 유지보수 가능한 코드베이스를 구축했습니다.
향후 개선 사항을 통해 더 높은 성능과 안정성을 목표로 발전시킬 예정입니다
