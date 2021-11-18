
#pragma once
#include <time.h>  
#include <stdlib.h> 
#include <array>
#include <queue>
#include "../../Interface/src/iController.h"
#include "../../Interface/src/iPlayer.h"
#include "../../Interface/src/Constants.h"
#include "Persona.cpp"
#include "ZergRushPersona.cpp"
#include "FatBastardPersona.cpp"
#include "RangerPersona.cpp"
#include "AlliancePersona.cpp"
#include "HoardPersona.cpp"
#include "FatGodPersona.cpp"
using namespace std;

// This is an AI controller that controls a Persona AI at every tick
class Controller_AI_CyrusThompson : public iController
{
public:
    // The game is starting up
    Controller_AI_CyrusThompson() {
        // The game is starting up wait a bit
        srand((unsigned)time(0));
        gameover = false;
        // have player pick an enemy AI
        cout << "0 - Zerg Rush - Prepare to be rushed" << endl;
        cout << "1 - Fat Bastard - He is hungry and dumb" << endl;
        cout << "2 - Ranger - They are smart and they will face you on the open field" << endl;
        cout << "3 - Alliance - Get ready for a seige" << endl;
        cout << "4 - Hoard - MEET THE WANDERING ORC HOARD WAAAHHHHG(My recommendation)" << endl;
        cout << "5 - FAT GOD _ YOU WILL BE EATEN" << endl;
        cout << "Please enter what number Persona you would like to play they are ordered based on difficulty" << endl;
        int persona_choice;
        bool all_good = false;
        while (!all_good)
        {
            try {
                cin >> persona_choice;
                if (!cin || persona_choice < 0 || persona_choice > 5)
                    throw "";
                all_good = true;
            }
            catch (...)
            {
                cout << "Please enter a valid value" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        // set persona
        switch (persona_choice) {
        case 0:
            persona = new ZergRushPersona();
            break;
        case 1:
            persona = new FatBastardPersona();
            break;
        case 2:
            persona = new RangerPersona();
            break;
        case 3:
            persona = new AlliancePersona();
            break;
        case 4:
            persona = new HoardPersona();
            break;
        case 5:
            persona = new FatGodPersona();
            break;
        }
    }
    virtual ~Controller_AI_CyrusThompson() {
        delete persona;
    }
    void tick(float deltaTSec);
    //check if AI has lost
    virtual bool checkDefeat() {
        return m_pPlayer->getNumBuildings() == 0 && m_pPlayer->getNumMobs() == 0;
    }
    //check if the AI has won
    virtual bool checkVictory() {
        return m_pPlayer->getNumOpponentBuildings() == 0 && m_pPlayer->getNumOpponentMobs() == 0;
    }
protected:
    Persona* persona;
    string persona_name;
    bool gameover;
};