// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "iPlayer.h"

#include "Constants.h"
#include <algorithm>
#include <assert.h>

class iController;
class Entity;

class Player : public iPlayer {
public:
    // NOTE: we take ownership of the controller
    explicit Player(iController* pControl, bool bNorth);
    virtual ~Player();

    virtual bool isNorth() const { return m_bNorth; }

    virtual float getElixir() const { return (float)m_Elixir; }
    virtual const std::vector<iEntityStats::MobType>& GetAvailableMobTypes() const { return m_AvailableMobs; }
    virtual PlacementResult placeMob(iEntityStats::MobType type, const Vec2& pos);

    void tick(float deltaTSec);

    const std::vector<Entity*>& getBuildings() const { return m_Buildings; }
    const std::vector<Entity*>& getMobs() const { return m_Mobs; }

    virtual unsigned int getNumBuildings() const { return m_Buildings.size(); }
    virtual EntityData getBuilding(unsigned int i) const;

    virtual unsigned int getNumMobs() const { return m_Mobs.size(); }
    virtual EntityData getMob(unsigned int i) const;

    virtual unsigned int getNumOpponentBuildings() const { return GetOpponent().getNumBuildings(); }
    virtual EntityData getOpponentBuilding(unsigned int i) const;

    virtual unsigned int getNumOpponentMobs() const { return GetOpponent().getNumMobs(); }
    virtual EntityData getOpponentMob(unsigned int i) const;

    //turns units to castle or castle upgrade if possible at given location 
    virtual void unitToCastle(Vec2 gridPos);
    // builds castle at given location 
    virtual void buildCastle(Vec2 hexPos);
    // turns castle level into a unit
    virtual void castleLevelToUnit(Vec2 gridPos);

    // adds given path to given number of mobs in starting location of path
    // if -1 is given will add to all mobs
    virtual void addPath(std::vector<Vec2> hexPath, int num_mobs = -1);

    virtual const Vec2 getHexCenter(Vec2 hexpos);

    // returns the hex postion of given grid pos
    virtual const Vec2 getHexPos(Vec2 gridpos);

    // returns a path between start and end hexpostions using A*
    virtual std::vector<Vec2> getHexPath(Vec2 starthexpos, Vec2 endhexpos);

    // returns a  [x,y] values of valid neigbors of given location(loc) that are not outside the bounds of max x and y and zero
    // valid neighbors change based on the y in a hex grid
    virtual std::vector<Vec2> getValidNeighbors(Vec2 loc);

    // checks if given hexlocation is valid 
    virtual bool locationValid(Vec2 hexloc);

private:
    void placeStartingMobs();

    const Player& GetOpponent() const;

    float capElixir(float e) const { return std::max(e, MAX_ELIXIR); }

private:
    iController* m_pControl;                // owned, may be NULL

    bool m_bNorth;
    float m_Elixir;

    std::vector<iEntityStats::MobType> m_AvailableMobs;

    std::vector<Entity*> m_Buildings;       // owned
    std::vector<Entity*> m_Mobs;            // owned

    // When mobs die, we move them to this vector.  For now we just hang on to 
    // them forever - we never delete them - so as to avoid memory issues.
    std::vector<Entity*> m_DeadMobs;        // owned

};
