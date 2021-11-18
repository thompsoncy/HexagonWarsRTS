#include "Persona.cpp"
// This file is used to create a more Advanced Abstract Base AI Class MindfulPersona

// class mindful abstract persona that has a mindset mechanic that can be used to change mindsets 
// specifically to change from being on the offensive to the defence 
class MindfulPersona : public Persona {
public:
    MindfulPersona() : Persona() {
        offensive_mindset = false;
    }
    ~MindfulPersona() {
    }
    // check if the mindset should be changed and then act on current mindset
    virtual void doStrategy() {
        Persona::doStrategy();
        if (ChangeMindset()) {
            offensive_mindset = !offensive_mindset;
        }
        if (offensive_mindset) {
            doOffensiveStrat();
        }
        else {
            doDefensiveStrat();
        }
    }
    // determine if the persona should switch from an offensive to defensive mindset
    virtual bool ChangeMindset() = 0;
    // do an offensive/defensive stratagy
    virtual void doOffensiveStrat() = 0;
    virtual void doDefensiveStrat() = 0;
protected:
    bool offensive_mindset;
};