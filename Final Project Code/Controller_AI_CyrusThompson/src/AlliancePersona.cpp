#include "MindfulPersona.cpp"

// Alliance mindfulpersona that will do a castle based Defensive game 
// specifcially will build castles and stay in them till they have the advantage and attack
class AlliancePersona : public MindfulPersona {
public:
    AlliancePersona() : MindfulPersona() {
        persona_name = "Alliance";
        outputVoiceLine("Come and get us");
    }
    ~AlliancePersona() {
    }
    // do defensive evaluation
    int getDefensiveEvaluation(HexTileValues hex_tile) {
        // if there is fight support it
        if ((hex_tile.enemy_castle_level > 0 || hex_tile.enemy_troop_number > 0)
            && (hex_tile.friendly_castle_level > 0 || hex_tile.friendly_troop_number > 0)) {
            return -100 - (hex_tile.friendly_castle_level + hex_tile.friendly_troop_number);
        }
        return 0;
    }
    // detemine if the mindset should be changed
    bool ChangeMindset() {
        // if we are already offensive
        if (offensive_mindset && 14 < m_pPlayer->getNumMobs() && 4 < m_pPlayer->getNumBuildings()) {
            // if we are outnumbered time to defend
            outputVoiceLine("STAY BEHIND STRONG WALLS");
            return true;
        }
        else if (!offensive_mindset && (10 > m_pPlayer->getNumMobs() || 5 > m_pPlayer->getNumBuildings())) {
            // if we have more forces than them time to attack
            outputVoiceLine("RAISE THE KEEPS MARSHAL THE MEN");
            return true;
        }
        return false;
    }

    // will attack if it has the advantage and the castles 
    void doOffensiveStrat() {
        spawnUnits(4);
        buildWherePossible();
        Vec2 start_hex;
        // will defend an attaked castle 
        if (defensive_queue.top().val < -90) {
            sendMobs({ defensive_queue.top().hexx,defensive_queue.top().hexy }, m_pPlayer->getNumMobs());
        }
        //otherwise attack
        else if (m_pPlayer->getNumBuildings() < 5) {
            if (m_pPlayer->getNumBuildings() > 0) {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getBuilding(0).m_Position);
            }
            else {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getMob(m_pPlayer->getNumMobs() - 1).m_Position);
            }
            sendMobs(getBuildLocations(start_hex).back(), m_pPlayer->getNumMobs());
        }
    }
    // will send all units to a front line castle
    void doDefensiveStrat() {
        if (defensive_queue.top().val < -90) {
            sendMobs({ defensive_queue.top().hexx,defensive_queue.top().hexy }, m_pPlayer->getNumMobs());
        }
    }

    void doDefeat() {
        outputVoiceLine("OUR WALLS WERE SO HIGH");
    }
    void doVictory() {
        outputVoiceLine("ATTRITION BEATS ALL");
    }
};
