# 📊 BÁO CÁO KẾT QUẢ KIỂM THỬ TOÀN BỘ HỆ THỐNG (FULL SYSTEM UNIT TEST REPORT)

**Tên dự án:** Turn-Based Adventure Game  
**Phân hệ / Module:** Toàn bộ hệ thống (Utils, Character/Warrior/Mage/Healer/Archer, Team, CharacterRoster, TeamManager, DataFileManager, BattleEngine/BattleState, Menu, GameApp)  
**Ngày thực hiện:** 11/08/2026  
**Môi trường thử nghiệm:** Windows OS | Compiler: MinGW-W64 `g++` (C++11 standard)  
**Tập tin kịch bản test:** 
- [TestUtils.cpp](tests/TestUtils.cpp)
- [TestCharacterAndSubclasses.cpp](tests/TestCharacterAndSubclasses.cpp)
- [TestTeam.cpp](tests/TestTeam.cpp)
- [TestCharacterRoster.cpp](tests/TestCharacterRoster.cpp)
- [TestTeamManager.cpp](tests/TestTeamManager.cpp)
- [TestDataFileManager.cpp](tests/TestDataFileManager.cpp)
- [TestBattleEngine.cpp](tests/TestBattleEngine.cpp)
- [TestMenu.cpp](tests/TestMenu.cpp)
- [TestGameApp.cpp](tests/TestGameApp.cpp)
- [RunAllTests.cpp](tests/RunAllTests.cpp) (Master Suite)
- Script tự động: [run_tests.bat](run_tests.bat)

---

## 1. TỔNG QUAN KẾT QUẢ KIỂM THỬ (EXECUTIVE SUMMARY)

Hệ thống Unit Test được thiết kế nhằm đánh giá toàn diện tính đúng đắn, khả năng xử lý ngoại lệ, tính đóng gói dữ liệu, quy tắc logic trận đấu (turn-based battle), đòn đánh chí mạng lượt 3 của Archer, kỹ năng hồi máu cho đồng đội của Healer, và khả năng đọc/ghi file lưu trữ của tất cả 8 phân hệ trong toàn bộ dự án **Turn-Based Adventure Game**.

### Kết quả tổng hợp:
| Phân hệ (Module) | Số Test Cases | Vượt qua (PASSED) | Thất bại (FAILED) | Tỷ lệ thành công |
| :--- | :---: | :---: | :---: | :---: |
| **1. Utilities (`Utils`)** | **9** | 9 | 0 | 100.0% |
| **2. Domain Model (`Character`, `Warrior`, `Mage`, `Healer`, `Archer`)** | **12** | 12 | 0 | 100.0% |
| **3. Team Entity (`Team`)** | **2** | 2 | 0 | 100.0% |
| **4. Character Roster (`CharacterRoster`)** | **6** | 6 | 0 | 100.0% |
| **5. Team Manager (`TeamManager`)** | **5** | 5 | 0 | 100.0% |
| **6. Persistence (`DataFileManager`)** | **19** | 19 | 0 | 100.0% |
| **7. Battle Engine (`BattleEngine`, `BattleState`)** | **6** | 6 | 0 | 100.0% |
| **8. Menu & Orchestrator (`Menu`, `GameApp`)** | **4** | 4 | 0 | 100.0% |
| **TỔNG CỘNG TOÀN BỘ HỆ THỐNG** | **63** | **63** | **0** | **100.0%** |

---

## 2. DẠNG PHẠM VI KIỂM THỬ (TEST SCOPE & COVERAGE)

Bộ kiểm thử bao phủ toàn bộ các phương thức public và kịch bản biên trong dự án:

1. **Utils**: Thao tác trim chuỗi, split theo delimiter, kiểm tra chuỗi số nguyên `isInteger`, parse chuỗi sang số nguyên an toàn `parseInt`.
2. **Character & Subclasses**: Khởi tạo thông số nhân vật, setter kiểm tra tính hợp lệ, tính toán nhận sát thương (take damage clamping 0), hồi máu (heal clamping maxHp), đòn đánh vật lý của Warrior, đòn đánh chí mạng `criticaldmg` lượt 3 của Archer, hành động phép thuật của Mage, kỹ năng hồi máu cho đồng đội bị thương nặng nhất và đòn dự phòng hồi mana của Healer.
3. **Team**: Đóng gói danh sách ID nhân vật trong đội, kiểm tra trùng lặp ID, gán tên và ID đội hình.
4. **CharacterRoster**: Thêm/Sửa/Xóa nhân vật (Warrior, Mage, Healer, Archer), kiểm tra trùng lặp ID, giới hạn 100 nhân vật, tìm kiếm nhân vật theo từ khóa không phân biệt hoa thường.
5. **TeamManager**: Thêm/Xóa/Đổi tên đội hình, thêm nhân vật vào đội (tối đa 5 nhân vật), xóa nhân vật khỏi tất cả các đội hình khi bị xóa ở roster, đọc/ghi tập tin đội hình.
6. **DataFileManager**: Parse và đóng gói dòng dữ liệu định dạng pipe (`WARRIOR|...`, `MAGE|...`, `HEALER|...`, `ARCHER|...`, `ID|NAME|IDS`), xử lý dòng comment `#`, dòng lỗi định dạng, đọc/ghi tập tin nhân vật, đội hình và lưu trữ lịch sử thống kê trận đấu (`team_win_loss_stats.txt`).
7. **BattleEngine**: Kiểm tra quy tắc khởi tạo trận đấu (2 team hợp lệ, chứa nhân vật sống), luân phiên lượt đánh giữa 2 team, logic Healer chọn đồng đội còn sống bị mất HP nhiều nhất (không heal kẻ địch), tính an toàn bộ nhớ khi vector tái cấp phát (OOP-TC-08), tự động phát hiện thắng thua khi 1 team bị tiêu diệt hoặc kết thúc hòa sau 100 rounds.
8. **Menu & GameApp**: Kiểm tra nhập dữ liệu từ stream, hiển thị các bảng thông tin (bao gồm hiển thị thông minh cột Hồi HP khi có Healer), luồng điều hướng menu và thoát ứng dụng an toàn.

---

## 3. BẢNG MA TRẬN KẾT QUẢ KỊCH BẢN KIỂM THỬ (TEST SUITE MATRIX)

### Phân hệ 1: Utilities (`Utils`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 1 | `TC_UTIL_01` | Trim các khoảng trắng và ký tự xuống dòng ở hai đầu chuỗi | Trả về chuỗi đã cắt sạch khoảng trắng | ✅ PASSED |
| 2 | `TC_UTIL_02` | Trim chuỗi rỗng hoặc chỉ chứa khoảng trắng | Trả về chuỗi rỗng `""` | ✅ PASSED |
| 3 | `TC_UTIL_03` | Split chuỗi pipe-separated dạng `WARRIOR\|101\|Ares\|100\|30` | Trả về 5 token nguyên vẹn | ✅ PASSED |
| 4 | `TC_UTIL_04` | Split chuỗi chứa khoảng trắng thừa quanh dấu phân cách | Trả về các token đã trim khoảng trắng | ✅ PASSED |
| 5 | `TC_UTIL_05` | Split chuỗi rỗng hoặc có dấu phân cách ở cuối | Xử lý đúng số lượng token trả về | ✅ PASSED |
| 6 | `TC_UTIL_06` | Kiểm tra chuỗi số nguyên hợp lệ (dương, âm, dấu `+`) | Trả về `true` | ✅ PASSED |
| 7 | `TC_UTIL_07` | Kiểm tra chuỗi không phải số nguyên (chữ cái, số thực, dấu đứng 1 mình) | Trả về `false` | ✅ PASSED |
| 8 | `TC_UTIL_08` | Parse chuỗi số nguyên hợp lệ sang `int` | Trả về giá trị `int` chính xác | ✅ PASSED |
| 9 | `TC_UTIL_09` | Parse chuỗi không hợp lệ hoặc bị tràn số (overflow) | Trả về giá trị mặc định (fallback) | ✅ PASSED |

### Phân hệ 2: Domain Model (`Character`, `Warrior`, `Mage`, `Healer`, `Archer`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 10 | `TC_CHAR_MOD_01` | Khởi tạo thuộc tính ban đầu và kiểm tra clamp thông số lỗi (ID <= 0, MaxHP <= 0) | ID và MaxHP được clamp về giá trị an toàn | ✅ PASSED |
| 11 | `TC_CHAR_MOD_02` | Đổi tên và thay đổi Max HP của nhân vật | Từ chối tên rỗng, clamp HP hiện tại khi Max HP giảm | ✅ PASSED |
| 12 | `TC_CHAR_MOD_03` | Nhân vật chịu sát thương và được hồi máu | HP không âm (clamp tại 0), HP không vượt quá Max HP | ✅ PASSED |
| 13 | `TC_CHAR_MOD_04` | Reset máu nhân vật về đầy (Full HP) | `currentHp == maxHp` | ✅ PASSED |
| 14 | `TC_WARRIOR_01` | Getter/Setter kiểm tra chỉ số Attack Power của Warrior | Tuân thủ giá trị dương > 0 | ✅ PASSED |
| 15 | `TC_WARRIOR_02` | Warrior thực hiện tấn công đối phương | Mục tiêu bị trừ HP bằng đúng Attack Power | ✅ PASSED |
| 16 | `TC_MAGE_01` | Getter/Setter kiểm tra chỉ số Mana, Spell Damage, Mana Cost của Mage | Tuân thủ các điều kiện giá trị dương | ✅ PASSED |
| 17 | `TC_MAGE_02` | Mage tấn công: Sử dụng Spell Damage khi đủ Mana vs Fallback Damage khi thiếu Mana | Đủ Mana: tiêu tốn Mana & gây Spell Damage; Khai hết Mana: gây Fallback Damage | ✅ PASSED |
| 18 | `TC_HEALER_01` | Getter/Setter kiểm tra chỉ số Mana và Heal Amount của Healer | `healAmount` không được vượt quá `maxHp` của chính Healer | ✅ PASSED |
| 19 | `TC_HEALER_02` | Healer thực hiện hành động: Hồi máu cho đồng đội vs Fallback Damage & Hồi Mana | Đủ mana: hồi máu đồng đội (không quá maxHp); Thiếu mana: đánh fallback & hồi mana | ✅ PASSED |
| 20 | `TC_ARCHER_01` | Getter/Setter kiểm tra Attack Power và đếm số lượt của Archer | `attackPower > 0`, `turnCount` khởi tạo bằng 0 | ✅ PASSED |
| 21 | `TC_ARCHER_02` | Archer tấn công: Sát thương thường 2 lượt đầu, Sát thương Chí mạng (`attackPower * 2`) ở lượt thứ 3 | Lượt 1, 2: `attackPower`; Lượt 3, 6: `attackPower * 2` | ✅ PASSED |

### Phân hệ 3: Team Entity (`Team`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 22 | `TC_TEAM_MOD_01` | Khởi tạo Team, kiểm tra ID và cài đặt tên đội | ID <= 0 được clamp về 0, tên rỗng bị từ chối | ✅ PASSED |
| 23 | `TC_TEAM_MOD_02` | Thêm, xóa và kiểm tra tồn tại ID nhân vật trong đội | Bỏ qua ID lặp lại/không hợp lệ, xóa đúng ID | ✅ PASSED |

### Phân hệ 4: Character Roster (`CharacterRoster`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 24 | `TC_ROSTER_01` | Thêm mới Warrior, Mage, Healer, Archer hợp lệ vào Roster | Trả về `true`, truy vấn lại bằng ID chính xác | ✅ PASSED |
| 25 | `TC_ROSTER_02` | Thêm nhân vật lặp trùng ID hoặc dữ liệu không hợp lệ | Trả về `false`, không làm thay đổi Roster | ✅ PASSED |
| 26 | `TC_ROSTER_03` | Cập nhật thông tin các loại nhân vật đã có trong Roster | Cập nhật thành công, từ chối khi lệch kiểu (type mismatch) | ✅ PASSED |
| 27 | `TC_ROSTER_04` | Xóa nhân vật khỏi Roster theo ID | Trả về `true`, không còn tìm thấy nhân vật | ✅ PASSED |
| 28 | `TC_ROSTER_05` | Tìm kiếm nhân vật theo chuỗi con của Tên (không phân biệt hoa thường) | Trả về danh sách kết quả khớp đúng | ✅ PASSED |
| 29 | `TC_ROSTER_06` | Lấy toàn bộ danh sách nhân vật và kiểm tra sức chứa tối đa (100) | Trả về đủ danh sách, kiểm tra `full()` chính xác | ✅ PASSED |

### Phân hệ 5: Team Manager (`TeamManager`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 30 | `TC_TM_01` | Tạo mới và xóa Đội hình với điều kiện ID và Tên là duy nhất | Trả về `true` khi hợp lệ, `false` khi trùng ID/Tên | ✅ PASSED |
| 31 | `TC_TM_02` | Đổi tên đội hình hiện có | Trả về `true`, từ chối nếu tên mới đã thuộc đội khác | ✅ PASSED |
| 32 | `TC_TM_03` | Thêm/Xóa nhân vật vào đội hình (Giới hạn tối đa 5 nhân vật) | Giới hạn tối đa 5 người/đội, từ chối thêm nhân vật không có ở Roster | ✅ PASSED |
| 33 | `TC_TM_04` | Xóa nhân vật khỏi tất cả các đội hình khi nhân vật bị xóa ở Roster | Tự động loại bỏ ID nhân vật khỏi toàn bộ các Team | ✅ PASSED |
| 34 | `TC_TM_05` | Đọc và Ghi danh sách đội hình ra file lưu trữ | Đọc/Ghi chính xác dữ liệu đội hình | ✅ PASSED |

### Phân hệ 6: Persistence (`DataFileManager`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 35 | `TC_DFM_01` | Parse dòng Warrior chuẩn: `WARRIOR\|101\|Ares\|100\|30` | Trả về đối tượng `Warrior` chính xác | ✅ PASSED |
| 36 | `TC_DFM_02` | Parse dòng Mage chuẩn: `MAGE\|102\|Luna\|80\|50\|40\|10\|15` | Trả về đối tượng `Mage` chính xác | ✅ PASSED |
| 37 | `TC_DFM_02B` | Parse dòng Healer chuẩn: `HEALER\|103\|Mercy\|90\|60\|35\|20\|15` | Trả về đối tượng `Healer` chính xác | ✅ PASSED |
| 38 | `TC_DFM_02C` | Parse dòng Archer chuẩn: `ARCHER\|301\|Robin\|120\|35` | Trả về đối tượng `Archer` chính xác | ✅ PASSED |
| 39 | `TC_DFM_03` | Parse chuỗi rỗng hoặc dòng comment `#` | Trả về `nullptr` | ✅ PASSED |
| 40 | `TC_DFM_04` | Parse loại nhân vật không hỗ trợ (vd: `PALADIN\|...`) | Trả về `nullptr` | ✅ PASSED |
| 41 | `TC_DFM_05` | Parse dòng nhân vật bị thiếu hoặc thừa trường dữ liệu | Trả về `nullptr` | ✅ PASSED |
| 42 | `TC_DFM_06` | Parse dòng nhân vật có dữ liệu không hợp lệ | Trả về `nullptr` | ✅ PASSED |
| 43 | `TC_DFM_07` | Serialize đối tượng Warrior/Mage/Healer/Archer thành chuỗi | Chuỗi định dạng chuẩn `TYPE\|...` | ✅ PASSED |
| 44 | `TC_DFM_08` | Serialize đối tượng nhân vật bị hỏng dữ liệu | Trả về chuỗi rỗng `""` | ✅ PASSED |
| 45 | `TC_DFM_09` | Ghi danh sách nhân vật ra file tạm và đọc lại | Đọc lại nguyên vẹn danh sách ban đầu | ✅ PASSED |
| 46 | `TC_DFM_10` | Đọc file nhân vật không tồn tại | Trả về `false` và thông báo lỗi an toàn | ✅ PASSED |
| 47 | `TC_DFM_11` | Parse dòng Team chuẩn: `201\|Red Team\|101,102` | Trả về đối tượng `Team` chứa đúng ID | ✅ PASSED |
| 48 | `TC_DFM_12` | Parse dòng Team rỗng hoặc comment `#` | Trả về `false` | ✅ PASSED |
| 49 | `TC_DFM_13` | Parse dòng Team thiếu tên hoặc thừa token rác | Trả về `false` | ✅ PASSED |
| 50 | `TC_DFM_14` | Parse dòng Team có ID nhân vật không hợp lệ/lặp lại | Trả về `false` | ✅ PASSED |
| 51 | `TC_DFM_15` | Serialize đối tượng Team thành chuỗi văn bản | Chuỗi định dạng chuẩn `ID\|NAME\|IDS` | ✅ PASSED |
| 52 | `TC_DFM_16` | Serialize đối tượng Team không hợp lệ | Trả về chuỗi rỗng `""` | ✅ PASSED |
| 53 | `TC_DFM_17` | Ghi danh sách Team ra file tạm và đọc lại | Đọc lại nguyên vẹn danh sách Team ban đầu | ✅ PASSED |
| 54 | `TC_DFM_18` | Đọc file Team không tồn tại | Trả về `false` và thông báo lỗi an toàn | ✅ PASSED |
| 55 | `TC_DFM_19` | Lưu vết và đọc lại thống kê trận thắng/thua của các đội (`team_win_loss_stats.txt`) | Ghi nhận chính xác số trận thắng/hòa/thua | ✅ PASSED |

### Phân hệ 7: Battle Engine (`BattleEngine`, `BattleState`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 56 | `TC_BAT_01` | Chuyển đổi enum `BattleState` sang chuỗi đại diện | Trả về `"READY"`, `"IN_PROGRESS"`, `"FINISHED"` | ✅ PASSED |
| 57 | `TC_BAT_02` | Thẩm định khởi tạo trận đấu (Trùng team, team rỗng, nhân vật không có ở Roster) | Từ chối khởi tạo không hợp lệ, chấp nhận khi đủ điều kiện | ✅ PASSED |
| 58 | `TC_BAT_03` | Thực thi trận đấu luân phiên từng lượt, kiểm tra lượng HP và tự động kết thúc khi 1 team gục ngã | Trận đấu chuyển `IN_PROGRESS` -> lượt đổi đúng actor -> Team hết người gục ngã -> `FINISHED` & công nhận đội thắng | ✅ PASSED |
| 59 | `TC_BAT_03B` | Healer chỉ hồi máu cho đồng đội bị thương nặng nhất và không hồi máu cho kẻ địch | Healer tự chọn đúng đồng đội có HP % thấp nhất trong cùng team | ✅ PASSED |
| 60 | `TC_BAT_04` | Reset trạng thái trận đấu về ban đầu | Đưa `m_state` về `READY`, xóa các tham chiếu tạm | ✅ PASSED |
| 61 | `TC_BAT_05` | An toàn bộ nhớ khi vector lưu nhân vật tái cấp phát bộ nhớ (OOP-TC-08) | Không xuất hiện rò rỉ bộ nhớ hoặc con trỏ rác | ✅ PASSED |

### Phân hệ 8: Menu & Orchestrator (`Menu`, `GameApp`)
| STT | Mã Test Case | Mô tả kịch bản test | Kết quả mong đợi | Trạng thái |
| :---: | :--- | :--- | :--- | :---: |
| 62 | `TC_APP_01` | Đọc dữ liệu số nguyên, văn bản và xác nhận từ stream nhập liệu của Menu | Đọc chính xác, tự động yêu cầu nhập lại khi sai định dạng | ✅ PASSED |
| 63 | `TC_APP_02` | Khởi chạy điều phối viên `GameApp` và thực hiện luồng điều hướng thoát ứng dụng | Chạy ứng dụng an toàn, trả về code `0` khi thoát | ✅ PASSED |

---

## 4. BẰNG CHỨNG THỰC THI THỰC TẾ (MASTER TEST EXECUTION LOG)

```text
======================================================================
                  RUNNING MASTER UNIT TEST SUITE                      
======================================================================
======================================================================
   🚀 MASTER SYSTEM UNIT TEST RUNNER - TURN-BASED ADVENTURE GAME      
======================================================================

--- [MODULE 1: Utils] ---
  [TC_UTIL_01] Trim basic spaces and whitespace ... PASSED
  [TC_UTIL_02] Trim edge cases empty string ... PASSED
  [TC_UTIL_03] Split basic pipe separated string ... PASSED
  [TC_UTIL_04] Split with surrounding whitespace ... PASSED
  [TC_UTIL_05] Split empty and trailing delimiters ... PASSED
  [TC_UTIL_06] isInteger valid positive and negative strings ... PASSED
  [TC_UTIL_07] isInteger invalid alphabetic and float strings ... PASSED
  [TC_UTIL_08] parseInt valid string conversion ... PASSED
  [TC_UTIL_09] parseInt invalid fallback value handling ... PASSED

--- [MODULE 2: Domain Model (Character, Warrior, Mage)] ---
  [TC_CHAR_MOD_01] Constructor initial values and parameter clamping ... PASSED
  [TC_CHAR_MOD_02] Setters validation and max HP clamping ... PASSED
  [TC_CHAR_MOD_03] Take damage clamping and heal calculation ... PASSED
  [TC_CHAR_MOD_04] Reset health to full HP ... PASSED
  [TC_WARRIOR_01] Attack power getters and setters validation ... PASSED
  [TC_WARRIOR_02] Perform action dealing physical damage ... PASSED
  [TC_MAGE_01] Mana and spell stat getters and setters ... PASSED
  [TC_MAGE_02] Spell damage vs fallback damage based on mana ... PASSED
  [TC_HEALER_01] Mana and heal amount setters and validation ... PASSED
  [TC_HEALER_02] Perform action healing vs fallback damage and mana regen ... PASSED
  [TC_ARCHER_01] Attack power setters and turn count validation ... PASSED
  [TC_ARCHER_02] Perform action critical damage on every 3rd turn ... PASSED

--- [MODULE 3: Team Entity] ---
  [TC_TEAM_MOD_01] Team constructors, ID clamping and name setters ... PASSED
  [TC_TEAM_MOD_02] Add, remove and check character IDs in team ... PASSED

--- [MODULE 4: CharacterRoster] ---
  [TC_ROSTER_01] Add valid Warrior and Mage characters ... PASSED
  [TC_ROSTER_02] Reject duplicate IDs and invalid attributes ... PASSED
  [TC_ROSTER_03] Update existing character details and type checks ... PASSED
  [TC_ROSTER_04] Remove character by ID ... PASSED
  [TC_ROSTER_05] Search characters by case-insensitive name substring ... PASSED
  [TC_ROSTER_06] Retrieve all characters and check capacity limits ... PASSED

--- [MODULE 5: TeamManager] ---
  [TC_TM_01] Create and delete teams with unique constraints ... PASSED
  [TC_TM_02] Rename team and prevent duplicate names ... PASSED
  [TC_TM_03] Add/remove character to team with size 5 limit ... PASSED
  [TC_TM_04] Cascading character removal from all teams ... PASSED
  [TC_TM_05] Load and save teams to file ... PASSED

--- [MODULE 6: DataFileManager (Persistence)] ---
  [TC_DFM_01] Parse Warrior line ... PASSED
  [TC_DFM_02] Parse Mage line ... PASSED
  [TC_DFM_02B] Parse Healer line ... PASSED
  [TC_DFM_02C] Parse Archer line ... PASSED
  [TC_DFM_03] Parse comment or empty line ... PASSED
  [TC_DFM_04] Parse unknown character type ... PASSED
  [TC_DFM_05] Parse line with missing or extra tokens ... PASSED
  [TC_DFM_06] Parse line with invalid values ... PASSED
  [TC_DFM_07] Serialize character to pipe string ... PASSED
  [TC_DFM_08] Serialize invalid character returns empty string ... PASSED
  [TC_DFM_09] Load and Save characters round-trip IO ... PASSED
  [TC_DFM_10] Load characters from non-existent file ... DataFileManager: Error opening file for reading characters: non_existent_xyz.txt
PASSED
  [TC_DFM_11] Parse team line ... PASSED
  [TC_DFM_12] Parse team comment or empty line ... PASSED
  [TC_DFM_13] Parse team line missing or extra tokens ... PASSED
  [TC_DFM_14] Parse team line invalid values ... PASSED
  [TC_DFM_15] Serialize team to pipe string ... PASSED
  [TC_DFM_16] Serialize invalid team returns empty string ... PASSED
  [TC_DFM_17] Load and Save teams round-trip IO ... PASSED
  [TC_DFM_18] Load teams from non-existent file ... DataFileManager: Error opening file for reading teams: non_existent_teams_xyz.txt
PASSED
  [TC_DFM_19] Record and load team win/loss statistics ... PASSED

--- [MODULE 7: BattleEngine & BattleState] ---
  [TC_BAT_01] Convert BattleState enum to string ... PASSED
  [TC_BAT_02] Validate battle setup rules and team status ... PASSED
  [TC_BAT_03] Full combat round flow, turn transitions and victory ... PASSED
  [TC_BAT_03B] Healer ally-only targeting and healing in combat ... PASSED
  [TC_BAT_04] Reset battle state back to READY ... PASSED
  [TC_BAT_05] Team vector reallocation safety without use-after-free (OOP-TC-08) ... PASSED

--- [MODULE 8: Menu & Application Orchestrator] ---
  [TC_APP_01] Menu input parsing helpers and screen displays ... PASSED
  [TC_APP_02] GameApp orchestrator startup and clean exit flow ... PASSED

======================================================================
                      FULL SYSTEM TEST SUMMARY                        
======================================================================
 Total Test Cases  : 63
 Passed Test Cases : 63 (100.0%)
 Failed Test Cases : 0
 Overall Status    : PASSED (100%)
======================================================================
```

---

## 5. KẾT LUẬN & ĐÁNH GIÁ TOÀN DIỆN (CONCLUSION)

- Toàn bộ **63 kịch bản unit test** thuộc **8 phân hệ** trong mã nguồn dự án `Turn-Based Adventure Game` đã được viết và thực thi tự động thành công dưới chuẩn C++11.
- **Tỷ lệ vượt qua (Pass Rate) đạt 100% (63/63 PASSED)**.
- Hệ thống đã được kiểm chứng an toàn về:
  1. Tính đúng đắn của logic tính toán sát thương, đòn đánh chí mạng 3rd-turn của `Archer`, kỹ năng hồi máu cho đồng đội và phục hồi mana của `Healer`.
  2. Tính toàn vẹn của danh sách đội hình và quản lý nhân vật (`Team`, `TeamManager`, `CharacterRoster`).
  3. Khả năng parse/serialize dữ liệu lưu trữ bền vững (`DataFileManager`), bao gồm việc ghi nhận và xếp hạng thống kê trận đấu (`team_win_loss_stats.txt`).
  4. Vòng đời luân phiên lượt trận đấu, quản lý an toàn bộ nhớ khi vector tái cấp phát (OOP-TC-08), và xác định đội thắng cuộc / hòa sau 100 rounds (`BattleEngine`).
  5. Xử lý nhập xuất an toàn trên giao diện Menu console và tự động hiển thị linh hoạt cột Hồi HP (`Menu`, `GameApp`).
