# Turn-Based Adventure Game (Trò Chơi Phiêu Lưu Theo Lượt)

Hệ thống trò chơi phiêu lưu chiến đấu theo lượt (Turn-Based Adventure Game) được xây dựng bằng C++11, áp dụng triệt để các nguyên lý thiết kế hướng đối tượng (OOP), Mô hình Đa hình (Polymorphism), Pattern Prototype (`clone()`), Cơ chế RAII & Smart Pointers (`std::unique_ptr`, `std::shared_ptr`), cùng Giao diện Console UTF-8 tiếng Việt trực quan.

---

## 📋 Danh Sách Tính Năng Đã Hoàn Thành

### 1. Quản Lý Nhân Vật (Character Management)
- **4 Lớp Nhân Vật**:
  - ⚔️ **Chiến binh (Warrior)**: Tấn công vật lý ổn định bằng `attackPower`.
  - 🏹 **Cung thủ (Archer)**: Tấn công thường ở 2 lượt đầu, tung đòn **Chí mạng (`criticaldmg = attackPower * 2`)** ở mỗi lượt thứ 3 (3, 6, 9...).
  - 🧙‍♂️ **Pháp sư (Mage)**: Tấn công phép bằng `spellDamage` tiêu tốn `manaCost`. Khi cạn mana tự động chuyển sang đòn dự phòng `fallbackDamage`.
  - 🩺 **Trị thương (Healer)**: Hồi máu `healAmount` cho đồng đội đang sống có HP thấp nhất (không vọt quá `maxHp` và không heal kẻ địch). Khi cạn mana chuyển sang đòn dự phòng và hồi lại mana (50% fallback damage).
- **Chức năng Roster**: Thêm mới, Cập nhật, Xóa (xóa dây chuyền khỏi các đội hình), Tìm kiếm theo ID hoặc Tên (không phân biệt hoa/thường), Hiển thị danh sách dạng bảng căn chỉnh.

### 2. Quản Lý Đội Hình (Team Management)
- Tạo đội hình mới, Đổi tên đội hình, Xóa đội hình.
- Thêm / Xóa nhân vật khỏi đội hình với các ràng buộc duy nhất và giới hạn số lượng.
- Xem chi tiết danh sách đội hình và danh sách nhân vật thuộc đội hình.

### 3. Hệ Thống Chiến Đấu (Battle Engine)
- **Độc Lập & An Toàn Bộ Nhớ**: Khởi tạo bản sao nhân vật trong trận đấu bằng Prototype Pattern (`clone()`), không làm ảnh hưởng đến chỉ số nhân vật gốc trong Roster.
- **Tự Động Chọn Mục Tiêu**:
  - Tấn công: Chọn ngẫu nhiên kẻ địch còn sống.
  - Hồi máu: Chọn đồng đội còn sống bị mất HP nhiều nhất.
- **Battle Log Chi Tiết**: Ghi nhận chi tiết từng lượt đấu (Đội hành động, Loại hành động `Spell Damage`, `Heal`, `criticaldmg`, `Fallback Damage`, `Attack Power`, Sát thương/Lượng máu hồi, Mana tiêu tốn/hồi phục, Trạng thái mục tiêu).
- **Bảng Thống Kê Sau Trận Đấu**: Thống kê HP còn lại, Sát thương gây ra, Sát thương nhận vào, Mana đã tiêu tốn, và cột **"Hồi HP"** (tự động hiển thị linh hoạt khi trận đấu có Healer).

### 4. Thống Kê Trận Đấu & Lưu Trữ Dữ Liệu (Persistence)
- Đọc / Ghi dữ liệu Roster và Team dưới dạng định dạng văn bản pipe-separated (`characters.txt`, `teams.txt`).
- **Bảng Xếp Hạng Trận Đấu**: Tự động lưu vết kết quả trận đấu ra `team_win_loss_stats.txt`. Menu thống kê hiển thị danh sách các đội xếp theo số trận thắng giảm dần.

### 5. Bộ Kiểm Thử Tự Động (Master Unit Test Suite)
- Tích hợp 63 unit test cases phủ 100% toàn bộ 8 modules hệ thống, đảm bảo tính ổn định và không rò rỉ bộ nhớ.

---

## ⚙️ Các Giới Hạn Đã Chọn (System Constraints & Invariants)

1. **Giới Hạn Số Lượng**:
   - **Roster nhân vật**: Tối đa `100` nhân vật (`MAX_CHARACTERS = 100`).
   - **Danh sách đội hình**: Tối đa `100` đội hình (`MAX_TEAMS = 100`).
   - **Kích thước đội hình**: Tối đa `5` nhân vật / đội (`MAX_TEAM_SIZE = 5`).
2. **Giới Hạn Thuộc Tính & Nhập Liệu**:
   - ID: Phải là số nguyên dương (`ID > 0`), không trùng lặp.
   - Tên: Không được rỗng, không chứa ký tự đặc biệt `|` hoặc `,` (tránh vỡ định dạng file text).
   - Chỉ số HP, Mana, Sát thương, Mana Cost: Phải là số nguyên dương (`> 0`).
   - Lượng máu hồi phục (`healAmount`): Phải là số nguyên dương và **không được vượt quá `maxHp`** của chính Healer đó.
   - Luôn có cơ chế thông báo lỗi và yêu cầu người dùng nhập lại khi nhập sai.
3. **Giới Hạn Trận Đấu**:
   - Mỗi trận đấu yêu cầu 2 đội hình hợp lệ, mỗi đội có từ 1 đến 5 nhân vật còn sống.
   - Giới hạn tối đa **100 lượt đấu (rounds)**. Nếu quá 100 rounds chưa có đội nào bị hạ hoàn toàn, trận đấu được xử lý kết thúc với kết quả **Hòa (DRAW)**.
4. **Quản Lý Tài Nguyên (RAII & Memory Leak Prevention)**:
   - Toàn bộ việc cấp phát động đều sử dụng `std::unique_ptr` hoặc `std::shared_ptr`. Quản lý bộ nhớ hoàn toàn tự động, đảm bảo 0% memory leak kể cả khi ứng dụng bị ngắt đột xuất hay thoát từ menu chính.

---

## 📁 Cấu Trúc Thư Mục (Directory Structure)

```txt
turn-based-adventure-game/
├── include/                   # Header files (.h) định nghĩa interface & domain model
│   ├── Character.h            # Lớp cơ sở abstract Character
│   ├── Warrior.h              # Lớp nhân vật Chiến binh
│   ├── Archer.h               # Lớp nhân vật Cung thủ (Critical 3rd turn)
│   ├── Mage.h                 # Lớp nhân vật Pháp sư
│   ├── Healer.h               # Lớp nhân vật Trị thương
│   ├── Team.h                 # Lớp Đội hình
│   ├── CharacterRoster.h      # Quản lý danh sách nhân vật
│   ├── TeamManager.h          # Quản lý danh sách đội hình
│   ├── TeamRecord.h           # Struct thống kê kết quả trận đấu của đội
│   ├── BattleState.h          # Enum & helper trạng thái trận đấu
│   ├── BattleEngine.h         # Logic điều khiển trận đấu & thống kê
│   ├── DataFileManager.h      # Đọc/Ghi dữ liệu file text (Persistence)
│   ├── Menu.h                 # Giao diện Console tiếng Việt & helper nhập liệu
│   ├── GameApp.h              # Điều phối luồng ứng dụng chính
│   └── Utils.h                # Thuật toán tiện ích (trim, split, parse)
├── src/                       # Source files (.cpp) triển khai chi tiết logic
│   ├── Character.cpp
│   ├── Warrior.cpp
│   ├── Archer.cpp
│   ├── Mage.cpp
│   ├── Healer.cpp
│   ├── Team.cpp
│   ├── CharacterRoster.cpp
│   ├── TeamManager.cpp
│   ├── BattleState.cpp
│   ├── BattleEngine.cpp
│   ├── DataFileManager.cpp
│   ├── Menu.cpp
│   ├── GameApp.cpp
│   └── Utils.cpp
├── data/                      # Thư mục chứa dữ liệu lưu trữ
│   ├── characters.txt         # Lưu danh sách nhân vật
│   ├── teams.txt              # Lưu danh sách đội hình
│   └── team_win_loss_stats.txt # Lưu lịch sử thắng/thua các đội
├── tests/                     # Thư mục chứa các bản kiểm thử Unit Test
│   ├── RunAllTests.cpp        # Master Test Runner (chạy toàn bộ 63 test cases)
│   ├── TestCharacterAndSubclasses.cpp
│   ├── TestCharacterRoster.cpp
│   ├── TestTeamManager.cpp
│   ├── TestBattleEngine.cpp
│   ├── TestDataFileManager.cpp
│   ├── TestTeam.cpp
│   ├── TestUtils.cpp
│   ├── TestMenu.cpp
│   └── TestGameApp.cpp
├── main.cpp                   # Entry point chính của ứng dụng
├── run_tests.bat              # Batch script tự động biên dịch và chạy kiểm thử
├── .gitignore                 # Cấu hình bỏ qua các file build và tệp tài liệu
└── README.md                  # Tài liệu hướng dẫn dự án
```

---

## 🛠️ Hướng Dẫn Biên Dịch Và Chạy (Build & Execution)

### Yêu Cầu Môi Trường
- Trình biên dịch C++ hỗ trợ **C++11** (ví dụ: `g++` 4.8 trở lên hoặc MinGW-w64 trên Windows).
- Hệ điều hành: Windows / Linux / macOS (Ưu tiên Terminal hỗ trợ UTF-8).

---

### 1. Biên Dịch Và Chạy Chương Trình Chính (Main Application)

#### Trên Windows (Command Prompt / PowerShell):
Chuyển bảng mã Console sang UTF-8 để hiển thị tiếng Việt không bị lỗi font:
```cmd
chcp 65001
```

Biên dịch ứng dụng chính:
```cmd
g++ -std=c++11 -Iinclude src/*.cpp main.cpp -o turn_based_game.exe
```

Chạy chương trình:
```cmd
.\turn_based_game.exe
```

---

### 2. Biên Dịch Và Chạy Bộ Kiểm Thử (Unit Tests)

#### Cách 1: Chạy tự động tất cả các bản kiểm thử qua Script (Khuyên dùng)
Chạy script `run_tests.bat` để tự động biên dịch và thực thi toàn bộ 8 bộ test module & runner master:
```cmd
.\run_tests.bat
```

#### Cách 2: Biên dịch và chạy thủ công Master Test Runner
Biên dịch runner master:
```cmd
g++ -std=c++11 -Iinclude src/*.cpp tests/RunAllTests.cpp -o run_all_tests.exe
```

Chạy runner master:
```cmd
.\run_all_tests.exe
```

Kết quả mong đợi:
```txt
======================================================================
                      FULL SYSTEM TEST SUMMARY                        
======================================================================
 Total Test Cases  : 63
 Passed Test Cases : 63 (100.0%)
 Failed Test Cases : 0
 Overall Status    : PASSED (100%)
======================================================================
```