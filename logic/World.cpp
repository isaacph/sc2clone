//
// Created by isaac on 5/9/2020.
//

#include "World.h"
#include "../math_util.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

World::World() : root(true) {
}

void World::update(double delta) {
    for(auto& pair : projectiles) {
        auto& projectile = pair.second;
        if(projectile.alive) {
            auto p = units.find(projectile.target);
            if(p == units.end()) {
                projectile.alive = false;
                events.push({WorldEvent::PROJECTILE, projectile.number});
            } else {
                auto& unit = p->second;
                glm::vec2 dir = unit.position - projectile.position;
                if(glm::length(dir) <= projectile.speed * delta) {
                    projectile.alive = false;
                    events.push({WorldEvent::PROJECTILE, projectile.number});
                    unit.health -= projectile.damage;
                    if(unit.health <= 0) {
                        unit.alive = false;
                    }
                    events.push({WorldEvent::UNIT_UPDATE, unit.number});
                } else {
                    projectile.position += projectile.speed * glm::normalize(dir) * delta;
                }
            }
        }
    }
    for(auto& pair : units) {
        auto& unit = pair.second;
        if(unit.alive) {
            if(unit.attack_speed > 0 && unit.attack_timer > 0) {
                unit.attack_timer -= delta;
            }
            if (unit.command.type == UnitCommand::MOVE) {
                if (unit.type == Unit::WORKER || unit.type == Unit::SOLDIER) {
                    glm::vec2 dir = unit.command.destination - unit.position;
                    if (glm::length(dir) <= unit.speed * delta) {
                        if(root) {
                            events.push({WorldEvent::COMMAND, unit.number, UnitCommand(UnitCommand::NONE)});
                        }
                        unit.command.type = UnitCommand::NONE;
                        continue;
                    }
                    unit.direction = -atan2(dir.y, dir.x);
                    unit.position += glm::normalize(dir) * delta * unit.speed;
                } else {
                    if(root) {
                        events.push({WorldEvent::COMMAND, unit.number, UnitCommand(UnitCommand::NONE)});
                    }
                    unit.command.type = UnitCommand::NONE;
                }
            } else if (unit.command.type == UnitCommand::MAKE_WORKER) {
                if (unit.type == Unit::BARRACKS) {
                    unit.load_time -= delta;
                    if (unit.load_time <= 0) {
                        unit.command.type = UnitCommand::NONE;
                        local_create_worker(unit.team, unit.position + glm::vec2(unit.radius), unit.direction);
                    }
                } else {
                    unit.command.type = UnitCommand::NONE;
                }
            } else if(unit.command.type == UnitCommand::MAKE_SOLDIER) {
                if(unit.type == Unit::BARRACKS) {
                    unit.load_time -= delta;
                    if(unit.load_time <= 0) {
                        unit.command = UnitCommand(UnitCommand::NONE);
                        local_create_soldier(unit.team, unit.position + glm::vec2(unit.radius), unit.direction);
                    }
                } else {
                    if(root) {
                        events.push({WorldEvent::COMMAND, unit.number, UnitCommand(UnitCommand::NONE)});
                    }
                    unit.command.type = UnitCommand::NONE;
                }
            } else if (unit.command.type == UnitCommand::WORKER_TRANSFORM) {
                if (unit.type == Unit::WORKER) {
                    unit.load_time -= delta;
                    if (unit.load_time <= 0) {
                        if(root) {
                            unit.alive = false;
                        }
                        events.push({WorldEvent::DEAD_UNIT, unit.number});
                        local_create_barracks(unit.team, unit.position);
                    }
                }
            } else if(unit.command.type == UnitCommand::ATTACK) {
                if(unit.type == Unit::SOLDIER) {
                    auto p = units.find(unit.command.targetID);
                    if(p == units.end() || !p->second.alive || unit.number == p->second.number) {
                        if(root) {
                            events.push({WorldEvent::COMMAND, unit.number, UnitCommand(UnitCommand::NONE)});
                        }
                        unit.command.type = UnitCommand::NONE;
                    } else {
                        auto& target = p->second;
                        glm::vec2 dir = target.position - unit.position;
                        if (glm::length(dir) <= unit.attack_range + unit.speed * delta) {
                            unit.direction = -atan2(dir.y, dir.x);
                            if(glm::length(dir) > unit.attack_range) {
                                unit.position += glm::normalize(dir) * delta * unit.speed;
                            }
                            if(unit.attack_timer <= 0) {
                                unit.attack_timer = 1.0f / unit.attack_speed;
                                events.push({WorldEvent::UNIT_UPDATE, unit.number});
                                local_shoot_projectile(unit.number, target.number, 15, 5);
                            }
                        } else {
                            unit.direction = -atan2(dir.y, dir.x);
                            unit.position += glm::normalize(dir) * delta * unit.speed;
                        }
                    }
                }
            } else if(unit.command.type == UnitCommand::ATTACK_MOVE) {
                if(unit.type == Unit::SOLDIER) {
                    bool foundTarget = false;
                    glm::vec2& dest = unit.command.destination;
                    int closestTarget;
                    float closestTargetDist;
                    glm::vec2 closestTargetDir;
                    for(auto p : units) {
                        auto& target = p.second;
                        glm::vec2 dir = target.position - unit.position;
                        if(target.alive && target.team != unit.team &&
                            target.number != unit.number && glm::length(dir) <= unit.attack_range) {
                            if(!foundTarget) {
                                closestTargetDist = glm::length(dir);
                                closestTargetDir = dir;
                                closestTarget = target.number;
                                foundTarget = true;
                            }
                            else if(glm::length(dir) < closestTargetDist) {
                                closestTarget = target.number;
                                closestTargetDist = glm::length(dir);
                                closestTargetDir = dir;
                            }
                        }
                    }
                    if(!foundTarget) {
                        glm::vec2 dir = dest - unit.position;
                        unit.position += glm::normalize(dir) * delta * unit.speed;
                        unit.direction = -atan2(dir.y, dir.x);
                        if (glm::length(dir) <= unit.speed * delta) {
                            if(root) {
                                events.push({WorldEvent::COMMAND, unit.number, UnitCommand(UnitCommand::NONE)});
                            }
                            unit.command.type = UnitCommand::NONE;
                            continue;
                        }
                    } else {
                        unit.direction = -atan2(closestTargetDir.y, closestTargetDir.x);
                        if(unit.attack_timer <= 0) {
                            unit.attack_timer = 1.0f / unit.attack_speed;
                            events.push({WorldEvent::UNIT_UPDATE, unit.number});
                            local_shoot_projectile(unit.number, closestTarget, 15, 5);
                        }
                    }
                }
            }
        }
    }
}
Unit& World::create_worker(int team, glm::vec2 position, float direction) {
    Unit unit(next_unit_identity());
    unit.type = Unit::WORKER;
    unit.team = team;
    unit.radius = 1.4f;
    unit.position = position;
    unit.direction = direction;
    unit.health = 20;
    unit.speed = 4.0f;
    unit.alive = true;
    unit.command = {UnitCommand::NONE};
    return units.insert({unit.number, unit}).first->second;
}
Unit& World::create_barracks(int team, glm::vec2 position) {
    Unit unit(next_unit_identity());
    unit.type = Unit::BARRACKS;
    unit.team = team;
    unit.radius = 1.4f * 4;
    unit.position = position;
    unit.direction = 3.14159265359f / 4.0f;
    unit.health = 200;
    unit.speed = 0.0f;
    unit.alive = true;
    unit.command = {UnitCommand::NONE};
    return units.insert({unit.number, unit}).first->second;
}
Unit& World::create_soldier(int team, glm::vec2 position, float direction) {
    Unit unit(next_unit_identity());
    unit.type = Unit::SOLDIER;
    unit.team = team;
    unit.radius = 1.4f * 1.4f;
    unit.position = position;
    unit.direction = direction;
    unit.health = 50;
    unit.speed = 5.0f;
    unit.alive = true;
    unit.command = {UnitCommand::NONE};
    unit.attack_range = 10.0f;
    unit.attack_speed = 1.0f;
    return units.insert({unit.number, unit}).first->second;
}
int World::shoot_projectile(int source_unit, int target_unit, float damage, float speed) {
    auto p1 = units.find(source_unit);
    auto p2 = units.find(target_unit);
    if(p1 == units.end() || p2 == units.end()) {
        return -1;
    }
    auto& source = p1->second, target = p2->second;
    Projectile projectile;
    projectile.source = source_unit;
    projectile.target = target_unit;
    projectile.number = next_projectile_identity();
    projectile.position = source.position;
    projectile.damage = 15;
    projectile.speed = speed;
    projectile.alive = true;
    return projectiles.insert({projectile.number, projectile}).first->second.number;
}
void World::command_units(std::vector<int> ids, UnitCommand command) {
    for (int i : ids) {
        auto& unit = units[i];
        unit.command = command;
        if(unit.command.type == UnitCommand::MAKE_WORKER) {
            unit.load_time = 5;
        } else if(unit.command.type == UnitCommand::WORKER_TRANSFORM) {
            unit.load_time = 5;
        } else if(unit.command.type == UnitCommand::MAKE_SOLDIER) {
            unit.load_time = 5;
        }
    }
}
int World::next_unit_identity() {
    return unit_counter++;
}
void World::server_command(std::string s) {
    std::string cmd = format_cmd(s);
    std::string args = format_args(s);
    if(cmd == "unit") {
        Unit unit(args);
        if(!units.insert({unit.number, unit}).second) {
            units[unit.number] = unit;
        }
    } else if(cmd == "command") {
        std::vector<int> ids;
        UnitCommand command1 = UnitCommand::parse(args, ids);
        command_units(ids, command1);
    } else if(cmd == "clear") {
        units = {};
    } else if(cmd == "projectile") {
        Projectile projectile(args);
        if(!projectiles.insert({projectile.number, projectile}).second) {
            projectiles[projectile.number] = projectile;
        }
    }
}
void World::local_create_worker(int team, glm::vec2 position, float direction) {
    if(root) {
        Unit& u = create_worker(team, position, direction);
        events.push({WorldEvent::UNIT_UPDATE, u.number});
    } else {
        //events.push_back({WorldEvent::NEW_UNIT});
    }
}
void World::local_create_barracks(int team, glm::vec2 position) {
    if(root) {
        Unit &u = create_barracks(team, position);
        events.push({WorldEvent::UNIT_UPDATE, u.number});
    } else {
        //events.push_back({WorldEvent::NEW_UNIT});
    }
}
void World::local_create_soldier(int team, glm::vec2 position, float direction) {
    if(root) {
        Unit &u = create_soldier(team, position, direction);
        events.push({WorldEvent::UNIT_UPDATE, u.number});
    } else {
        //events.push_back({WorldEvent::NEW_UNIT});
    }
}
void World::local_shoot_projectile(int source, int target, float damage, float speed) {
    if(root) {
        int id = shoot_projectile(source, target, damage, speed);
        events.push({WorldEvent::PROJECTILE, id});
    }
}
void World::local_command_units(int team, std::vector<int> ids, UnitCommand command) {
    if(root) {
        std::vector<int> filtered = filter_ids(ids, team, command);
        std::vector<int> filtered_2;
        // do extra checks for if a command can be done
        if(command.type == UnitCommand::WORKER_TRANSFORM) {
            for(int i = 0; i < filtered.size(); ++i) {
                auto& unit = units[filtered[i]];
                bool too_close = false;
                for(auto& unit2 : units) {
                    if(unit2.second.number != unit.number && unit2.second.alive) {
                        if(glm::length(unit2.second.position - unit.position) < unit2.second.radius + 6) {
                            too_close = true;
                        }
                    }
                }
                if(!too_close) {
                    filtered_2.push_back(filtered[i]);
                }
            }
        } else {
            filtered_2 = filtered;
        }
        command_units(filtered_2, command);
        events.push({WorldEvent::COMMAND, 0, command, filtered_2});
    } else {
        events.push({WorldEvent::COMMAND, 0, command, ids});
    }
}
Unit::Unit(std::string args) {
    std::vector<std::string> arr = format_args_arr(args);
    if(arr.size() < 14) {
        return;
    }
    number = std::atoi(arr[0].c_str());
    type = (Unit::Type) std::atoi(arr[1].c_str());
    team = std::atoi(arr[2].c_str());
    radius = std::atof(arr[3].c_str());
    position = glm::vec2(std::atof(arr[4].c_str()), std::atof(arr[5].c_str()));
    direction = std::atof(arr[6].c_str());
    health = std::atof(arr[7].c_str());
    speed = std::atof(arr[8].c_str());
    alive = arr[9] == "1";
    load_time = std::atof(arr[10].c_str());
    attack_range = std::atof(arr[11].c_str());
    std::string cmd_str = arr[12];
    for(int i = 13; i < arr.size(); ++i) {
        cmd_str += " " + arr[i];
    }
    command = UnitCommand(cmd_str);
}
std::string Unit::toString() {
    if(abs(position.x) >= 10000 || abs(position.y) >= 10000) {
        position.x = 0;
        position.y = 0;
        std::cout << "Unit " << number << " has a position that is too big!";
    }
    return std::to_string(number) + " " +
        std::to_string(type) + " " +
        std::to_string(team) + " " +
        std::to_string(radius).substr(0, 5) + " " +
        std::to_string(position.x).substr(0, 5) + " " +
        std::to_string(position.y).substr(0, 5) + " " +
        std::to_string(direction).substr(0, 5) + " " +
        std::to_string(health).substr(0, 5) + " " +
        std::to_string(speed).substr(0, 5) + " " +
        std::to_string((int) alive) + " " +
        std::to_string(load_time) + " " +
        std::to_string(attack_range) + " " +
            command.toString();
}
UnitCommand::UnitCommand(std::string args) {
    std::vector<std::string> arr = format_args_arr(args);
    if(arr.size() < 4) {
        type = NONE;
        return;
    }
    type = (UnitCommand::Type) std::atoi(arr[0].c_str());
    destination.x = std::atof(arr[1].c_str());
    destination.y = std::atof(arr[2].c_str());
    targetID = std::atof(arr[3].c_str());
}
UnitCommand UnitCommand::parse(std::string args, std::vector<int> &ids) {
    std::vector<std::string> arr = format_args_arr(args);
    if(arr.size() < 5) {
        return {UnitCommand::NONE};
    }
    for(int i = 4; i < arr.size(); ++i) {
        ids.push_back(std::atoi(arr[i].c_str()));
    }
    return UnitCommand(args);
}
std::string UnitCommand::toString() {
    return std::to_string((int) type) + " " +
           std::to_string(destination.x) + " " +
           std::to_string(destination.y) + " " +
           std::to_string(targetID);
}
std::string World::unit_command_string(UnitCommand cmd, std::vector<int> ids) {
    if(ids.size() == 0) {
        return "";
    }
    std::string out = cmd.toString();
    for(int i = 0; i < ids.size(); ++i) {
        out += " " + std::to_string(ids[i]);
    }
    return out;
}
Projectile::Projectile(std::string args) {
    std::vector<std::string> args_arr = format_args_arr(args);
    if(args_arr.size() < 7) {
        std::cerr << "Bad projectile! Not enough arguments!";
        return;
    }
    number = std::atoi(args_arr[0].c_str());
    source = std::atoi(args_arr[1].c_str());
    target = std::atoi(args_arr[2].c_str());
    damage = std::atof(args_arr[3].c_str());
    position.x = std::atof(args_arr[4].c_str());
    position.y = std::atof(args_arr[5].c_str());
    speed = std::atof(args_arr[6].c_str());
    alive = args_arr[7] == "1";
}
std::string Projectile::toString() {
    return std::to_string(number) + " " +
        std::to_string(source) + " " +
        std::to_string(target) + " " +
        std::to_string(damage) + " " +
        std::to_string(position.x) + " " +
        std::to_string(position.y) + " " +
        std::to_string(speed) + " " +
        std::to_string((int) alive);
}
bool World::has_next_event() {
    return events.size() > 0;
}
WorldEvent World::next_event() {
    if(events.size() <= 0) {
        return {WorldEvent::NONE};
    }
    WorldEvent event = events.front();
    events.pop();
    return event;
}
std::vector<int> World::filter_ids(const std::vector<int>& ids, int team, const UnitCommand& command) {
    const auto a = unitsThatUseCommand.find(command.type);
    if(a == unitsThatUseCommand.end()) {
        std::cerr << "Unit command not in permissions array: " << command.type << std::endl;
        return {};
    }
    const auto& unitTypes = a->second;
    std::vector<int> good = {};
    for(auto& id : ids) {
        auto p = units.find(id);
        if(p != units.end()) {
            auto& unit = p->second;
            if((team == -1 || unit.team == team) && unitTypes.find(unit.type) != unitTypes.end()) {
                good.push_back(id);
            }
        }
    }
    return good;
}