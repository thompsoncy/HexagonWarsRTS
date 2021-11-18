#include "../../Interface/src/iPlayer.h"
#include "../../Interface/src/Constants.h"
#include <array>
#include <queue>
using namespace std;
// This file is used to create the Abstract Base AI Class Persona
// along with assosicated data structs HexTileValues, HexTileEvalStruct


// the stuct holds the strategically important values
// of an associated hex tile
struct HexTileValues
{
    // The level of enemy/friendly castle within this hex
    int enemy_castle_level;
    int friendly_castle_level;

    // number of enemy/freindly troops in this current hex
    int enemy_troop_number;
    int friendly_troop_number;

    // strength of the vicinity 
    // this is determined by the persona being used
    float enemy_power_vicinity;

    // defensive importance
    // this is determined by the persona being used
    int defensive_evaluation;

    // offensive importance
    // this is determined by the persona being used
    int offensive_evaluation;

    // default constructor sends all values to zero
    HexTileValues() {
        resetValues();
    }

    // sets all values to zero
    void resetValues() {
        enemy_castle_level = 0;
        friendly_castle_level = 0;
        enemy_troop_number = 0;
        friendly_troop_number = 0;
        enemy_power_vicinity = 0.f;
        defensive_evaluation = 0;
        offensive_evaluation = 0;
    }

    // checks if anything is in this tile
    // may or may not ignore friendly troops
    bool empty(bool ignore_friendly_troops = false) {
        return enemy_castle_level == 0 && friendly_castle_level == 0 && enemy_troop_number == 0 && (friendly_troop_number == 0 || ignore_friendly_troops);
    }
};

// this stuct holds a x, y, and an evaluation value
// This is used to make used to create a sorted list of hextiles
struct HexTileEvalStruct
{
    // value used for sorting
    int val;
    // hex tile x value
    int hexx;
    // hex tile y value
    int hexy;


    HexTileEvalStruct(int x, int y, int eval) : val(eval), hexx(x), hexy(y)
    {
    }

    bool operator<(const HexTileEvalStruct& other) const
    {
        //bigger is worse
        return val > other.val;
    }
};

// Base AI class that can be inherited to represent diffrent AI personas
// Persona is used to create a outline what a AI must do
// Including outputting voice lines at specific spots and playing the game itself
class Persona
{
public:
    Persona() {
        for (int x = 0; x < HEX_GRID_WIDTH; x++) {
            for (int y = 0; y < HEX_GRID_HEIGHT; y++) {
                hex_game_state[x][y] = HexTileValues();
            }
        }
        defensive_queue = priority_queue<HexTileEvalStruct>();
        offensive_queue = priority_queue<HexTileEvalStruct>();
    }

    virtual ~Persona() {}
    // do this personas Strategy for one tick
    // do defeat line 
    virtual void doDefeat() = 0;
    // do victory line 
    virtual void doVictory() = 0;
    // do defensive evaluation
    virtual int getDefensiveEvaluation(HexTileValues hex_tile) {
        return 0;
    }
    // do defensive evaluation
    virtual int getOffensiveEvaluation(HexTileValues hex_tile) {
        return 0;
    }
    // outputs voiceline to cout
    virtual void setPlayer(iPlayer* m_pPlayer) {
        this->m_pPlayer = m_pPlayer;
    }
    // checks if the player is set and the game can be played
    virtual bool isPlayerNotSet() {
        return m_pPlayer == 0;
    }
    // Outputs a voiceline to command line 
    virtual void outputVoiceLine(std::string voice_line) const {
        std::cout << persona_name << " says-" << std::endl;
        std::cout << voice_line << std::endl;
    }
    // places mob at specified location 
    virtual void aiPlaceMob(iEntityStats::MobType mob_type, Vec2 kspos) const {
        bool isNorth = m_pPlayer->isNorth();
        Vec2 pos_game = kspos.Player2Game(isNorth);
        m_pPlayer->placeMob(mob_type, pos_game);
    }
    // updates the gamestate 
    // this is done so that inheriting personas can use this information
    // to interact with the game intelligently
    virtual void updateGameState() {
        // resets all values
        for (int x = 0; x < HEX_GRID_WIDTH + 1; x++) {
            for (int y = 0; y < HEX_GRID_HEIGHT + 1; y++) {
                hex_game_state[x][y].resetValues();
            }
        }
        // adds current location of friendly mobs to gamestate
        for (unsigned int mob_index = 0; mob_index < m_pPlayer->getNumMobs(); mob_index++) {
            Vec2 mob_hex_location = m_pPlayer->getHexPos(m_pPlayer->getMob(mob_index).m_Position);
            if (mob_hex_location.x >= 0 and mob_hex_location.y >= 0) {
                hex_game_state[int(floor(mob_hex_location.x))][int(floor(mob_hex_location.y))].friendly_troop_number += 1;
            }
        }
        // adds current location of enemy mobs to gamestate
        for (unsigned int mob_index = 0; mob_index < m_pPlayer->getNumOpponentMobs(); mob_index++) {
            Vec2 mob_hex_location = m_pPlayer->getHexPos(m_pPlayer->getOpponentMob(mob_index).m_Position);
            if (mob_hex_location.x >= 0 && mob_hex_location.y >= 0) {
                hex_game_state[int(floor(mob_hex_location.x))][int(floor(mob_hex_location.y))].enemy_troop_number += 1;
            }
        }
        // adds friendly castles to gamestate
        for (unsigned int castle_index = 0; castle_index < m_pPlayer->getNumBuildings(); castle_index++) {
            Vec2 castle_hex_location = m_pPlayer->getHexPos(m_pPlayer->getBuilding(castle_index).m_Position);
            int castle_level = m_pPlayer->getBuilding(castle_index).m_level;
            if (castle_hex_location.x >= 0 and castle_hex_location.y >= 0) {
                hex_game_state[int(floor(castle_hex_location.x))][int(floor(castle_hex_location.y))].friendly_castle_level = castle_level;
            }
        }
        // adds enemy castles to gamestate
        for (unsigned int castle_index = 0; castle_index < m_pPlayer->getNumOpponentBuildings(); castle_index++) {
            Vec2 castle_hex_location = m_pPlayer->getHexPos(m_pPlayer->getOpponentBuilding(castle_index).m_Position);
            int castle_level = m_pPlayer->getOpponentBuilding(castle_index).m_level;
            if (castle_hex_location.x >= 0 && castle_hex_location.y >= 0) {
                hex_game_state[int(floor(castle_hex_location.x))][int(floor(castle_hex_location.y))].enemy_castle_level = castle_level;
            }
        }
        // add enemy power in the vicinity 
        for (int i = 1; i < 5; i++) {
            for (int x = 0; x < HEX_GRID_WIDTH + 1; x++) {
                for (int y = 0; y < HEX_GRID_HEIGHT + 1; y++) {
                    vector<Vec2> neighbors = m_pPlayer->getValidNeighbors({ x,y });
                    for (unsigned int neighbor_index = 0; neighbor_index < neighbors.size(); neighbor_index++) {
                        hex_game_state[x][y].enemy_power_vicinity += hex_game_state[int(floor(neighbors[neighbor_index].x))][int(floor(neighbors[neighbor_index].y))].enemy_troop_number + 0.f;
                    }
                }
            }
        }
        // fill the queuse that has the most important evaluations first
        defensive_queue = priority_queue<HexTileEvalStruct>();
        offensive_queue = priority_queue<HexTileEvalStruct>();
        //get offensive and defensive evaluations
        for (int x = 0; x < HEX_GRID_WIDTH + 1; x++) {
            for (int y = 0; y < HEX_GRID_HEIGHT + 1; y++) {
                hex_game_state[x][y].defensive_evaluation = getDefensiveEvaluation(hex_game_state[x][y]);
                defensive_queue.push(HexTileEvalStruct(hex_game_state[x][y].defensive_evaluation, x, y));
                hex_game_state[x][y].offensive_evaluation = getOffensiveEvaluation(hex_game_state[x][y]);
                offensive_queue.push(HexTileEvalStruct(hex_game_state[x][y].offensive_evaluation, x, y));
            }
        }
    }

    // sends a given number of mobs to given hex location
    // with closest being sent first
    virtual void sendMobs(Vec2 hexPos, int num_mobs) const {
        // if you try to send more mobs than avalible you will just send max mobs
        if (num_mobs > int(m_pPlayer->getNumMobs())) {
            num_mobs = m_pPlayer->getNumMobs();
        }
        int mobs_being_sent = 0;
        vector<Vec2> neigbors;
        neigbors.push_back(hexPos);
        // remebers if given location has been seen
        bool* have_seen = new bool[(HEX_GRID_HEIGHT + 1) * (HEX_GRID_WIDTH + 1)]{ false };
        have_seen[int(floor(neigbors[0].y)) * HEX_GRID_HEIGHT + int(floor(neigbors[0].x))] = true;
        while (!neigbors.empty()) {
            Vec2 current_loc = neigbors[0];
            neigbors.erase(neigbors.begin());
            int mob_at_loc = hex_game_state[int(floor(current_loc.x))][int(floor(current_loc.y))].friendly_troop_number;
            mobs_being_sent += mob_at_loc;
            if (mobs_being_sent >= num_mobs) {
                m_pPlayer->addPath(m_pPlayer->getHexPath(current_loc, hexPos), mob_at_loc - (mobs_being_sent - num_mobs));
                return;
            }
            m_pPlayer->addPath(m_pPlayer->getHexPath(current_loc, hexPos));
            vector<Vec2> valid_neighbors = m_pPlayer->getValidNeighbors(current_loc);
            //add neighbors
            for (unsigned int i = 0; i < valid_neighbors.size(); i++) {
                Vec2 cur_neighbor = valid_neighbors[i];
                if (!have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))]) {
                    have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))] = true;
                    neigbors.push_back(cur_neighbor);
                }
            }
        }
    }

    //spawns given number of units if able with castles of given level or below
    // -1 for number of units spawns as many as possible
    virtual void spawnUnits(int maxlevel, int num_mob = -1) {
        for (unsigned int castle_index = 0; castle_index < m_pPlayer->getNumBuildings(); castle_index++) {

            Vec2 castle_hex_location = m_pPlayer->getHexPos(m_pPlayer->getBuilding(castle_index).m_Position);
            int castle_level = m_pPlayer->getBuilding(castle_index).m_level;
            if (castle_hex_location.x >= 0 && castle_hex_location.y >= 0 && castle_level <= maxlevel && castle_level > 1) {
                m_pPlayer->castleLevelToUnit(m_pPlayer->getBuilding(castle_index).m_Position);
                num_mob -= 1;
            }
            if (num_mob == 0) {
                return;
            }
        }
    }

    // gets the closest to given locations free hex build locations returns given num build locations
    virtual vector<Vec2> getBuildLocations(Vec2 start_hex, int num_loc = 1) {
        int mobs_being_sent = 0;
        vector<Vec2> neigbors;
        vector<Vec2> output = vector<Vec2>();
        neigbors.push_back(start_hex);
        // remmebers if given location has been seen
        bool* have_seen = new bool[(HEX_GRID_HEIGHT + 1) * (HEX_GRID_WIDTH + 1)]{ false };
        have_seen[int(floor(neigbors[0].y)) * HEX_GRID_HEIGHT + int(floor(neigbors[0].x))] = true;
        while (!neigbors.empty()) {
            Vec2 current_loc = neigbors[0];
            neigbors.erase(neigbors.begin());
            if (hex_game_state[int(floor(current_loc.x))][int(floor(current_loc.y))].empty(true)) {
                output.push_back(current_loc);
                num_loc -= 1;
            }
            if (num_loc < 1) {
                return output;
            }
            vector<Vec2> valid_neighbors = m_pPlayer->getValidNeighbors(current_loc);
            //add neighbors
            for (unsigned int i = 0; i < valid_neighbors.size(); i++) {
                Vec2 cur_neighbor = valid_neighbors[i];
                if (!have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))]) {
                    have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))] = true;
                    neigbors.push_back(cur_neighbor);
                }
            }
        }
        return output;
    }
    // will build where there there are offset + build cost number of mobs
    virtual void buildWherePossible(int offset = 0) {
        for (int x = 0; x < HEX_GRID_WIDTH + 1; x++) {
            for (int y = 0; y < HEX_GRID_HEIGHT + 1; y++) {
                if (hex_game_state[x][y].empty(true) && hex_game_state[x][y].friendly_troop_number >= CASTLEBUILDCOST + offset) {
                    m_pPlayer->unitToCastle(m_pPlayer->getHexCenter({ x,y }));
                }
            }
        }
    }

    // just updates the gamestate
    // The idea is before any inheriting persona does a stratagy the gamestate should be updated
    virtual void doStrategy() {
        updateGameState();
    }
protected:
    // this is the gamestate that represents the hex tile board and 
    // various persona specific evaluations of it 
    array<array<HexTileValues, HEX_GRID_HEIGHT + 1>, HEX_GRID_WIDTH + 1> hex_game_state;
    // name of this persona
    std::string persona_name;
    // pointer to the Player
    iPlayer* m_pPlayer;
    // queues that have the most important evaluations first
    // either offensive or defensive
    priority_queue<HexTileEvalStruct>  defensive_queue;
    priority_queue<HexTileEvalStruct>  offensive_queue;
};
