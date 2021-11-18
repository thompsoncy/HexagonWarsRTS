#include "Persona.cpp"

// Fat Bastard persona will go wide get fat then attack when it has the advantage
class FatBastardPersona : public Persona {
public:
    FatBastardPersona() : Persona() {
        persona_name = "Fat Bastard";
        outputVoiceLine("I will come over there at some point");
    }
    ~FatBastardPersona() {
    }
    // the statagy is simply to build as many castles as possible then attack when it has the advantage 
    void doStrategy() {
        Persona::doStrategy();
        spawnUnits(2);
        // will attempt to build
        if (m_pPlayer->getNumBuildings() == 0) {
            buildWherePossible();
        }
        else {
            buildWherePossible(3);
        }
        // if we have overwelming numbers attack
        if (m_pPlayer->getNumMobs() > 0 && m_pPlayer->getNumMobs() >= 2 + m_pPlayer->getNumOpponentMobs() * 3) {
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
        outputVoiceLine("Of course, I'm not happy. Look at me, I'm a big fat slob. I've got bigger titties than you. I've got more chins than Chinese phone book. I've not seen my willy in two years which is long enough to declare I'm legally dead.");
    }
    void doVictory() {
        outputVoiceLine("NOW GET IN MY BELLY");
    }
};