#include "Persona.cpp"

// Fat God Persona will go wide get fat then attack when you cant win
class FatGodPersona : public Persona {
public:
    FatGodPersona() : Persona() {
        persona_name = "Fat God";
        outputVoiceLine("I WILL EAT THE WORLD");
    }
    ~FatGodPersona() {
    }
    // the statagy is simply to build as many castles as possible then attack when it can not lose
    void doStrategy() {
        Persona::doStrategy();
        spawnUnits(2);
        buildWherePossible();
        // if we have very overwelming numbers attack
        if (m_pPlayer->getNumMobs() > 0 && m_pPlayer->getNumMobs() >= 3 + m_pPlayer->getNumOpponentMobs() * 4) {
            if (m_pPlayer->getNumOpponentBuildings() > 0) {
                sendMobs(m_pPlayer->getHexPos(m_pPlayer->getOpponentBuilding(0).m_Position), m_pPlayer->getNumMobs());
            }
            else {
                sendMobs(m_pPlayer->getHexPos(m_pPlayer->getOpponentMob(0).m_Position), m_pPlayer->getNumMobs());
            }
        }
        else if (m_pPlayer->getNumOpponentMobs() * 2 > m_pPlayer->getNumBuildings()) {

            Vec2 start_hex;
            if (m_pPlayer->getNumBuildings() > 0) {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getBuilding(0).m_Position);
            }
            else {
                start_hex = m_pPlayer->getHexPos(m_pPlayer->getMob(m_pPlayer->getNumMobs() - 1).m_Position);
            }
            sendMobs(getBuildLocations(start_hex).back(), m_pPlayer->getNumMobs());

        }
    }
    void doDefeat() {
        outputVoiceLine("EAT IT ALL");
    }
    void doVictory() {
        outputVoiceLine("NOM NOM NOM");
    }
};