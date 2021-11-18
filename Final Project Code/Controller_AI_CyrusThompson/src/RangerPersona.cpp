#include "MindfulPersona.cpp"

// Ranger mindful persona that will play a game outside it's castles
class RangerPersona : public MindfulPersona {
public:
    RangerPersona() : MindfulPersona() {
        persona_name = "Ranger";
        outputVoiceLine("Range far Range fast");
    }
    ~RangerPersona() {
    }
    // do defensive evaluation
    int getDefensiveEvaluation(HexTileValues hex_tile) {
        // if there is fight support it
        if ((hex_tile.enemy_castle_level > 0 || hex_tile.enemy_troop_number > 0)
            && (hex_tile.friendly_castle_level > 0 || hex_tile.friendly_troop_number > 0)) {
            return -100 - (hex_tile.friendly_castle_level + hex_tile.friendly_troop_number);
        }
        // empty tiles are the best(these are rangers)
        return hex_tile.enemy_castle_level * 10 + hex_tile.enemy_troop_number + hex_tile.friendly_castle_level;
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
        if (offensive_mindset) {
            // if we are outnumbered time to defend
            return m_pPlayer->getNumOpponentMobs() * .5 + 2 > m_pPlayer->getNumMobs();
        }
        else {
            // if we have more forces than them time to attack
            return m_pPlayer->getNumOpponentMobs() * 1.75 + 2 <= m_pPlayer->getNumMobs();
        }
    }
    void doOffensiveStrat() {
        sendMobs({ offensive_queue.top().hexx, offensive_queue.top().hexy }, m_pPlayer->getNumMobs());
    }
    // will send all units to a front line castle
    void doDefensiveStrat() {
        buildWherePossible();
        spawnUnits(2);
        // if there is a fight go there
        if (defensive_queue.top().val < -90) {
            sendMobs({ defensive_queue.top().hexx,defensive_queue.top().hexy }, m_pPlayer->getNumMobs());
        }
        else {
            // if there is no fight
            Vec2 start_hex;
            if (m_pPlayer->getNumBuildings() > 0) {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getBuilding(0).m_Position);
            }
            else {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getMob(m_pPlayer->getNumMobs() - 1).m_Position);
            }
            // spread out and range
            vector<Vec2> locs = getBuildLocations(start_hex, m_pPlayer->getNumBuildings() + 1);
            for (unsigned int i = 0; i < locs.size(); i++) {
                sendMobs(locs[locs.size() - 1 - i], 1);
            }

        }
    }

    void doDefeat() {
        outputVoiceLine("The Wilds Are Our Graves");
    }
    void doVictory() {
        outputVoiceLine("Return to the earth");
    }
};