/**
 * @file Team.h
 * @module Team
 * @brief Represents a Team entity that contains character IDs only.
 * @features Store team id, name and vector of member character IDs.
 * @input Team creation parameters.
 * @output Team identity and member list.
 */

#ifndef TURN_BASED_ADVENTURE_GAME_TEAM_H
#define TURN_BASED_ADVENTURE_GAME_TEAM_H

#include <string>
#include <vector>

namespace TurnBasedGame {

class Team {
public:
    Team() = default;
    Team(int id, const std::string& name, const std::vector<int>& characterIds = {});

    int getId() const;
    void setId(int id);

    const std::string& getName() const;
    bool setName(const std::string& name);

    const std::vector<int>& getCharacterIds() const;
    void setCharacterIds(const std::vector<int>& ids);
    void addCharacterId(int id);
    bool removeCharacterId(int id);
    bool hasCharacterId(int id) const;

private:
    int m_id{0};
    std::string m_name;
    std::vector<int> m_characterIds;
};

} // namespace TurnBasedGame

#endif // TURN_BASED_ADVENTURE_GAME_TEAM_H
