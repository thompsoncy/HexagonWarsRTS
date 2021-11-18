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

// Final Project:  A player is one of the two players in the game - either the 
// north or the south player.  This interface provides the functions you use to 
// query into the state of your player and the opposing player, and to control 
// your player.

#include "EntityStats.h"
#include "Vec2.h"
#include <vector>
#include <stack>

class iEntity;
class iOpposingEntity;

class iPlayer 
{
public:
    iPlayer() {}
    virtual ~iPlayer() {}

    // Final Project: What side are you playing?  If true, you're the top
    // player (i.e. red).  You'll need to account for this when doing things
    // like placing units, so that you place them on the right side.
    virtual bool isNorth() const = 0;

    // Final Project: Call this to find out how much elixir you currently have.
    // You can look in Constants.h to see how much you start with, what the 
    // maximum is and how fast it accumulates.
    virtual float getElixir() const = 0;

    // Final Project: This vector tells you what types of mobs are currently 
    // available to be placed.
    // TODO: Implement this feature - right now it's all of them!
    virtual const std::vector<iEntityStats::MobType>& GetAvailableMobTypes() const = 0;

    // Final Project: Your AI will call this function to place its mobs.  The
    // return value will tell you whether it succeeded and why it failed.
    enum PlacementResult
    {
        Success = 0,
        InsufficientElixir,
        InvalidX,
        InvalidY,
        MobTypeUnavailable,
    };
    virtual PlacementResult placeMob(iEntityStats::MobType type, const Vec2& pos) = 0;

    //turns units to castle or castle upgrade if possible at given location 
    virtual void unitToCastle(Vec2 gridPos) = 0;
    // builds castle at given location 
    virtual void buildCastle(Vec2 hexPos) = 0;
    // turns castle level into a unit
    virtual void castleLevelToUnit(Vec2 gridPos) = 0;

    // adds given path to given number of mobs in starting location of path
    // if -1 is given will add to all mobs
    virtual void addPath(std::vector<Vec2> hexPath, int num_mobs = -1) = 0;

    // get center location of a hex from hex postion to grid postion 
    virtual const Vec2 getHexCenter(Vec2 hexpos) = 0;

    // returns the hex postion of given grid pos
    virtual const Vec2 getHexPos(Vec2 gridpos) = 0;

    // returns a path between start and end grid postions using A*
    virtual std::vector<Vec2> getHexPath(Vec2 starthexpos, Vec2 endhexpos) = 0;

    // returns a  [x,y] values of valid neigbors of given location(loc) that are not outside the bounds of max x and y and zero
    // valid neighbors change based on the y in a hex grid
    virtual std::vector<Vec2> getValidNeighbors(Vec2 loc) = 0;

    // checks if given hexlocation is valid 
    virtual bool locationValid(Vec2 hexloc) = 0;


    struct EntityData
    {
        const iEntityStats& m_Stats;
        int& m_Health;
        const Vec2& m_Position;
        int& m_level;
        std::stack<Vec2>& hexpath;

        EntityData();
        EntityData(const iEntityStats& stats, int& health, const Vec2& pos,int& m_level, std::stack<Vec2>& hexpath);
        EntityData(const EntityData& rhs);
    };

    virtual unsigned int getNumBuildings() const = 0;
    virtual EntityData getBuilding(unsigned int i) const = 0;
    virtual unsigned int getNumMobs() const = 0;
    virtual EntityData getMob(unsigned int i) const = 0;

    virtual unsigned int getNumOpponentBuildings() const = 0;
    virtual EntityData getOpponentBuilding(unsigned int i) const = 0;
    virtual unsigned int getNumOpponentMobs() const = 0;
    virtual EntityData getOpponentMob(unsigned int i) const = 0;

private:
    // DELIBERATELY UNDEFINED
    iPlayer(const iPlayer& rhs);
    iPlayer& operator=(const iPlayer& rhs);
    bool operator==(const iPlayer& rhs) const;
    bool operator<(const iPlayer& rhs) const;
};



