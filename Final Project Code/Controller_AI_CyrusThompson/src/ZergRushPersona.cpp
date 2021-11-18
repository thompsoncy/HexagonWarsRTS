#include "Persona.cpp"

// A Zerg Rush persona 
// Strait out of star craft this persona wil Zerg Rush always
class ZergRushPersona : public Persona {
public:
    ZergRushPersona() : Persona() {
        persona_name = "ZERG RUSH";
        outputVoiceLine("THE SWARM AAAAAAAAAAAAAAAAAAAA");
    }
    ~ZergRushPersona() {
    }
    // this Strategy is simply attacking any enemy thing on the hex board 
    void doStrategy() {
        Persona::doStrategy();
        // kill buildings
        if (m_pPlayer->getNumOpponentBuildings() > 0 && m_pPlayer->getOpponentBuilding(0).m_level > 1) {
            sendMobs(m_pPlayer->getHexPos(m_pPlayer->getOpponentBuilding(0).m_Position), m_pPlayer->getNumMobs());
        }
        // kill mobs
        if (m_pPlayer->getNumMobs() > 0 && m_pPlayer->getNumOpponentBuildings() == 0 && m_pPlayer->getNumOpponentMobs() < 5) {

            sendMobs(m_pPlayer->getHexPos(m_pPlayer->getOpponentMob(0).m_Position), m_pPlayer->getNumMobs());
        }
    }
    void doDefeat() {
        outputVoiceLine("ZERG RUSH TO DEATH");
    }
    void doVictory() {
        outputVoiceLine("THE HIVE IS ALL");
    }
};
