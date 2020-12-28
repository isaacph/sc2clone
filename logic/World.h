//
// Created by isaac on 5/9/2020.
//

#ifndef UNTITLED2_WORLD_H
#define UNTITLED2_WORLD_H
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <queue>
#include <string>

struct UnitCommand {
    enum Type {
        NONE, MOVE, MAKE_WORKER, WORKER_TRANSFORM, MAKE_SOLDIER, ATTACK, ATTACK_MOVE
    } type = NONE;
    glm::vec2 destination;
    int targetID;
    UnitCommand() = default;
    inline UnitCommand(Type type1, glm::vec2 dest, int targetID) : type(type1), destination(dest), targetID(targetID) {}
    inline UnitCommand(Type type1, glm::vec2 dest) : type(type1), destination(dest) {}
    inline UnitCommand(Type type1, int targetID) : type(type1), destination(glm::vec2(0)), targetID(targetID) {}
    inline UnitCommand(Type type1) : UnitCommand(type1, glm::vec2(0)) {}
    UnitCommand(std::string args);
    static UnitCommand parse(std::string args, std::vector<int>& ids);
    std::string toString();
};

struct Unit {
    int number;
    enum Type {
        WORKER, BARRACKS, SOLDIER
    } type;
    int team;
    float radius;
    glm::vec2 position;
    float direction = 0;
    float health = 100;
    float speed = 4.0f;
    bool alive = true;
    UnitCommand command;
    float load_time = 0;
    float attack_range = 0;
    float attack_speed = 0;
    float attack_timer = 0;
    inline Unit(int number) : number(number) {}
    Unit() = default;
    Unit(std::string args);
    std::string toString();
};

struct WorldEvent {
    enum Type {
        NONE, UNIT_UPDATE, COMMAND, DEAD_UNIT, PROJECTILE
    } type;
    int identity;
    UnitCommand command;
    std::vector<int> command_ids;
};

struct Projectile {
    int number;
    int source;
    int target;
    float damage;
    glm::vec2 position;
    float speed;
    bool alive;
    Projectile() = default;
    Projectile(std::string args);
    std::string toString();
};

class World {
public:
    bool root;
    World();
    std::map<int, Unit> units;
    std::set<int> unitsSelected;
    std::map<int, Projectile> projectiles;

    void local_command_units(int team, std::vector<int> ids, UnitCommand command);
    void local_create_worker(int team, glm::vec2 position, float direction = 0);
    void local_create_barracks(int team, glm::vec2 position);
    void local_create_soldier(int team, glm::vec2 position, float direction = 0);
    void local_shoot_projectile(int source, int target, float damage, float speed);

    void server_command(std::string s);

    void update(double delta);
    WorldEvent next_event();
    bool has_next_event();
    const std::map<UnitCommand::Type, const std::set<Unit::Type>> unitsThatUseCommand = {
            {UnitCommand::NONE, {Unit::WORKER, Unit::SOLDIER, Unit::BARRACKS}},
            {UnitCommand::MOVE, {Unit::WORKER, Unit::SOLDIER}},
            {UnitCommand::MAKE_WORKER, {Unit::BARRACKS}},
            {UnitCommand::MAKE_SOLDIER, {Unit::BARRACKS}},
            {UnitCommand::WORKER_TRANSFORM, {Unit::WORKER}},
            {UnitCommand::ATTACK, {Unit::SOLDIER}},
            {UnitCommand::ATTACK_MOVE, {Unit::SOLDIER}}
    };
    static std::string unit_command_string(UnitCommand cmd, std::vector<int> ids);
private:
    int unit_counter = 0;
    int projectile_counter = 0;
    int next_unit_identity();
    inline int next_projectile_identity() {
        return projectile_counter++;
    }
    std::queue<WorldEvent> events;

    Unit& create_worker(int team, glm::vec2 position, float direction = 0);
    Unit& create_barracks(int team, glm::vec2 position);
    Unit& create_soldier(int team, glm::vec2 position, float direction = 0);
    int shoot_projectile(int source_unit, int target_unit, float damage, float speed);
    void command_units(std::vector<int> ids, UnitCommand command);
    std::vector<int> filter_ids(const std::vector<int>& ids, int team, const UnitCommand& command);
};

#endif //UNTITLED2_WORLD_H
