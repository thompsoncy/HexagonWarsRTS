#include "Controller_AI_CyrusThompson.h"


// This is an AI controller that controls a Persona AI at every tick
void Controller_AI_CyrusThompson::tick(float deltaTSec)
{
    // if the game is over do no more operations
    if (gameover == true) {
        return;
    }
    assert(m_pPlayer);
    // now that we know the player is not null make sure that is true for the persona as well
    if (persona->isPlayerNotSet()) {
        persona->setPlayer(m_pPlayer);
    }
    // check for victory 
    if (checkVictory()) {
        persona->doVictory();
        gameover = true;
        return;
    }
    // check for defeat
    if (checkDefeat()) {
        persona->doDefeat();
        gameover = true;
        return;
    }

    // do the personas strategy
    persona->doStrategy();
}

