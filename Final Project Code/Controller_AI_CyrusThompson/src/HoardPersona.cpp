#include "MindfulPersona.cpp"

// class Hoard mindfulpersona that will play a wandering Hoard offensive game 
class HoardPersona : public MindfulPersona {
public:
    HoardPersona() : MindfulPersona() {
        persona_name = "Hoard";
        outputVoiceLine("FOR THE HOARD");
    }
    ~HoardPersona() {
    }
    // do defensive evaluation
    int getDefensiveEvaluation(HexTileValues hex_tile) {
        // if there is fight support it
        if ((hex_tile.enemy_castle_level > 0 || hex_tile.enemy_troop_number > 0)
            && (hex_tile.friendly_castle_level > 0 || hex_tile.friendly_troop_number > 0)) {
            return -100 - (hex_tile.friendly_castle_level + hex_tile.friendly_troop_number);
        }
        // attack any enemy units not in castle first
        if (hex_tile.enemy_castle_level == 0 && hex_tile.enemy_troop_number > 0) {
            return -90;
        }
        return 0;
    }
    // do offensive evaluation
    int getOffensiveEvaluation(HexTileValues hex_tile) {
        // if there is fight support it
        if ((hex_tile.enemy_castle_level > 0 || hex_tile.enemy_troop_number > 0)
            && (hex_tile.friendly_castle_level > 0 || hex_tile.friendly_troop_number > 0)) {
            return -100 - (hex_tile.friendly_castle_level + hex_tile.friendly_troop_number);
        }
        // attack any enemy units not in castle first
        if (hex_tile.enemy_castle_level == 0 && hex_tile.enemy_troop_number > 0) {
            return -90;
        }
        // attack the weakest castle otherwise
        if (hex_tile.enemy_castle_level > 0) {
            if (hex_tile.enemy_power_vicinity >= 80) {
                return -89;
            }
            return int(floor(-1 * (hex_tile.enemy_power_vicinity))) - 10 + hex_tile.enemy_castle_level;
        }
        return 0;
    }
    // detemine if the mindset should be changed
    bool ChangeMindset() {
        // if we are already offensive
        if (offensive_mindset && m_pPlayer->getNumOpponentMobs() * .5 + 2 > m_pPlayer->getNumMobs()) {
            // if we are outnumbered time to defend
            outputVoiceLine("HOARD BACK");
            return true;
        }
        else if (!offensive_mindset && m_pPlayer->getNumOpponentMobs() * 1.75 + 3 <= m_pPlayer->getNumMobs()) {
            // if we have more forces than them time to attack
            outputVoiceLine("WAAAAHHHHGGGG ATTACK");
            return true;
        }
        return false;
    }

    void doOffensiveStrat() {
        sendMobs({ offensive_queue.top().hexx, offensive_queue.top().hexy }, m_pPlayer->getNumMobs());
    }
    // will send all units to a front line castle
    void doDefensiveStrat() {
        buildWherePossible(m_pPlayer->getNumBuildings() / 3);
        spawnUnits(2);
        // if there is a fight go there
        if (defensive_queue.top().val < -90) {
            sendMobs({ defensive_queue.top().hexx,defensive_queue.top().hexy }, m_pPlayer->getNumMobs());
        }
        // if they are out in the open and we can win attack
        else if (defensive_queue.top().val == -90 &&
            unsigned int(hex_game_state[defensive_queue.top().hexx][defensive_queue.top().hexy].enemy_troop_number) < m_pPlayer->getNumMobs()) {
            sendMobs({ defensive_queue.top().hexx,defensive_queue.top().hexy }, m_pPlayer->getNumMobs());
        }
        else if (m_pPlayer->getNumMobs() > 0) {
            // move the hoard to a new empty tile
            Vec2 start_hex;
            start_hex = m_pPlayer->getHexPos(m_pPlayer->getMob(m_pPlayer->getNumMobs() - 1).m_Position);
            vector<Vec2> validNeighbors = m_pPlayer->getValidNeighbors(start_hex);
            int start_index = validNeighbors.size();
            for (unsigned int i = rand() % validNeighbors.size(); i < validNeighbors.size(); i++) {
                if (hex_game_state[int(floor(validNeighbors[i].x))][int(floor(validNeighbors[i].y))].empty(true)) {
                    sendMobs(validNeighbors[i], m_pPlayer->getNumMobs());
                    break;
                }
            }

        }

    }

    void doDefeat() {
        outputVoiceLine("NOOOOOOO");
    }
    void doVictory() {
        outputVoiceLine("AZZEROTH IS OURS");
    }
};