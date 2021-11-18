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

#include "Player.h"

#include "Building.h"
#include "Constants.h"
#include "iController.h"
#include "Game.h"
#include "Mob.h"
#include <stack>
#include <array>
#include <queue>
using namespace std;

// returns the hex postion of given grid pos
const Vec2 Player::getHexPos(Vec2 gridpos) {
    int pixelX = int(floor(gridpos.x * PIXELS_PER_METER));
    int pixelY = int(floor(gridpos.y * PIXELS_PER_METER));
    int hex_y = HEXA * HEXSCALE;
    int count_y = -1;
    while (hex_y < pixelY) {
        hex_y += HEXSIDE * HEXSCALE;
        count_y += 1;
        if (hex_y > pixelY) {
            break;
        }
        hex_y += HEXA * HEXSCALE;
        if (hex_y > pixelY) {
            count_y = -1;
            break;
        }
    }
    int hex_x = HEXEDGE;
    if (count_y % 2 == 1) {
        hex_x += HEXB * HEXSCALE;
    }
    int count_x = -1;
    while (hex_x < pixelX) {
        hex_x += ((HEXB + HEXB) * HEXSCALE) - HEXEDGE * 2;
        count_x += 1;
        if (hex_x > pixelX) {
            break;
        }
        hex_x += HEXEDGE * 2;
        if (hex_x > pixelX) {
            count_x = -1;
            break;
        }
    }
    return Vec2(count_x, count_y);
}

// get center location of a hex from hex postion to grid postion 
const Vec2 Player::getHexCenter(Vec2 hexpos) {
    float pixelX = (HEXB + (HEXB * 2 * hexpos.x)) * HEXSCALE;
    if (int(floor(hexpos.y)) % 2 == 1) {
        pixelX += HEXB * HEXSCALE;
    }
    float pixelY = ((HEXA + (HEXSIDE / 2.f)) + ((HEXA + HEXSIDE)*hexpos.y)) * HEXSCALE;
    return Vec2(pixelX / PIXELS_PER_METER, pixelY / PIXELS_PER_METER);
}

Player::Player(iController* pControl, bool bNorth)
    : m_pControl(pControl)
    , m_bNorth(bNorth)
    , m_Elixir(capElixir(STARTING_ELIXIR))
{

    // for now, all mob types are available.
    for (size_t i = 0; i < iEntityStats::numMobTypes; ++i)
    {
        m_AvailableMobs.push_back((iEntityStats::MobType)i);
    }
    placeStartingMobs();

    if (m_pControl)
        m_pControl->setPlayer(*this);
}

Player::~Player()
{
    delete m_pControl;      // it's safe to delete NULL
    for (Entity* pBuilding : m_Buildings) delete pBuilding;
    for (Entity* pMob : m_Mobs) delete pMob;
    for (Entity* pMob : m_DeadMobs) delete pMob;
}

// checks if given hexlocation is valid 
bool Player::locationValid(Vec2 hexloc) {
    return hexloc.x >= 0 && hexloc.y >= 0 && hexloc.x < (HEX_GRID_WIDTH + 1) - (int(floor(hexloc.y)) % 2) && hexloc.y < (HEX_GRID_HEIGHT + 1);
}

iPlayer::PlacementResult Player::placeMob(iEntityStats::MobType type, const Vec2& pos)
{
    // Adjust the position to be a tile center.  Tiles are 1 unit wide.
    // TODO: move the code for converting to tile position somewhere shared

    Vec2 tilePos(pos.x, pos.y);
    Vec2 hexpos = getHexPos(pos);

    // Validate the position
    // TODO: move this functionality somewhere shared.
    if (!locationValid(hexpos))
    {
        std::cout << "Invalid Location (X): (" << tilePos.x << ", " <<
            tilePos.y << ")\n";
        return InvalidX;
    }


    // Validate that we have enough elixir
    const iEntityStats& stats = iEntityStats::getStats(type);
    Mob* pMob = new Mob(stats, tilePos, m_bNorth);
    m_Mobs.push_back(pMob);
    return Success;
}

void Player::tick(float deltaTSec)
{
    m_Elixir += deltaTSec * ELIXIR_PER_SECOND;

    if (m_pControl)
        m_pControl->tick(deltaTSec);

    for (Entity* pBuilding : m_Buildings) {
        if (!pBuilding->isDead()) {
            pBuilding->tick(deltaTSec);
        }
    }

    for (Entity* m : m_Mobs) {
        if (!m->isDead()) {
            m->tick(deltaTSec);
        }
    }

    // Move any mobs that died this tick into m_DeadMobs
    size_t newIndex = 0;
    for (size_t oldIndex = 0; oldIndex < m_Mobs.size(); ++oldIndex)
    {
        Entity* pMob = m_Mobs[oldIndex];
        if (!pMob->isDead())
        {
            Entity* pTemp = m_Mobs[newIndex];
            m_Mobs[newIndex] = m_Mobs[oldIndex];
            m_Mobs[oldIndex] = pTemp;
            ++newIndex;
        }
        else
        {
            m_DeadMobs.push_back(m_Mobs[oldIndex]);
        }
    }

    assert(newIndex <= m_Mobs.size());
    m_Mobs.resize(newIndex);

    // Move any buildings that died this tick into m_DeadMobs
    newIndex = 0;
    for (size_t oldIndex = 0; oldIndex < m_Buildings.size(); ++oldIndex)
    {
        Entity* pBuildings = m_Buildings[oldIndex];
        if (!pBuildings->isDead())
        {
            Entity* pTemp = m_Buildings[newIndex];
            m_Buildings[newIndex] = m_Buildings[oldIndex];
            m_Buildings[oldIndex] = pTemp;
            ++newIndex;
        }
        else
        {
            m_DeadMobs.push_back(m_Buildings[oldIndex]);
        }
    }

    assert(newIndex <= m_Buildings.size());
    m_Buildings.resize(newIndex);
}

iPlayer::EntityData Player::getBuilding(unsigned int i) const
{
    if (i < m_Buildings.size())
    {
        return m_Buildings[i]->getData();
    }

    return EntityData();
}

iPlayer::EntityData Player::getMob(unsigned int i) const
{
    if (i < m_Mobs.size())
    {
        return m_Mobs[i]->getData();
    }

    return EntityData();
}

iPlayer::EntityData Player::getOpponentBuilding(unsigned int i) const
{
    if (i < GetOpponent().getBuildings().size())
    {
        return GetOpponent().getBuildings()[i]->getData();
    }

    return EntityData();
}

iPlayer::EntityData Player::getOpponentMob(unsigned int i) const
{
    if (i < GetOpponent().getMobs().size())
    {
        return GetOpponent().getMobs()[i]->getData();
    }

    return EntityData();
}

void Player::placeStartingMobs()
{

    if (m_bNorth)
    {
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, 0)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, 0)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, 0)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, 0)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, 0)));
    }
    else
    {
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, HEX_GRID_HEIGHT)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, HEX_GRID_HEIGHT)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, HEX_GRID_HEIGHT)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, HEX_GRID_HEIGHT)));
        placeMob(iEntityStats::Swordsman, getHexCenter(Vec2(HEX_GRID_WIDTH / 2, HEX_GRID_HEIGHT)));
    }
}

void Player::unitToCastle(Vec2 gridpos)
{
    Vec2 hexPos = getHexPos(gridpos);

    // check if enemy castle is here if so stop
    for (unsigned int i = 0; i < getNumOpponentBuildings(); i++) {
        Vec2 oppCastleHexPos = getHexPos(getOpponentBuilding(i).m_Position);
        if (int(floor(oppCastleHexPos.x)) == int(floor(hexPos.x)) && int(floor(oppCastleHexPos.y)) == int(floor(hexPos.y))) {
            std::cout << "cant build ontop of enemy castle" << std::endl;
            return;
        }
    }
    int freindlyCastleindex = -1;
    // check if friendly castle is here 
    for (unsigned int i = 0; i < getNumBuildings(); i++) {
        Vec2 freindlyCastleHexPos = getHexPos(getBuilding(i).m_Position);
        if (int(floor(freindlyCastleHexPos.x)) == int(floor(hexPos.x)) && int(floor(freindlyCastleHexPos.y)) == int(floor(hexPos.y))) {
             freindlyCastleindex = i;
            break;
        }
    }
    // check if freindly castle is at max level stop if so stop
    if (freindlyCastleindex != -1 && getBuilding(freindlyCastleindex).m_level > 4) {
        std::cout << "castle already at max level" << std::endl;
        return;
    }

    // check there are enough units to build or upgrade castle if not stop
    std::stack<int>mobIndexStack = {};
    for (unsigned int i = 0; i < getNumMobs(); i++) {
        Vec2 freindlyMobHexPos = getHexPos(getMob(i).m_Position);
        if (int(floor(freindlyMobHexPos.x)) == int(floor(hexPos.x)) && int(floor(freindlyMobHexPos.y)) == int(floor(hexPos.y))) {
            mobIndexStack.push(i);
        }
    }
    // build castle and kill units
    if (freindlyCastleindex == -1 && mobIndexStack.size() >= CASTLEBUILDCOST) {
        // build castle 
        buildCastle(hexPos);
        // kill mobs
        for (int i = 0; i < CASTLEBUILDCOST; i++) {
            getMob(mobIndexStack.top()).m_Health = -1000;
            mobIndexStack.pop();
        }
    }
    // upgrade castle kill units
    if (freindlyCastleindex != -1) {
        int mobsToKill = 0;
        if (getBuilding(freindlyCastleindex).m_level == 1 && mobIndexStack.size() >= CASTLEUPGRADELEVELONECOST) {
            mobsToKill = CASTLEUPGRADELEVELONECOST;
            getBuilding(freindlyCastleindex).m_level += 1;
        }
        else if (getBuilding(freindlyCastleindex).m_level == 2 && mobIndexStack.size() >= CASTLEUPGRADELEVELTWOCOST) {
            mobsToKill = CASTLEUPGRADELEVELTWOCOST;
            getBuilding(freindlyCastleindex).m_level += 1;
        }
        else if (getBuilding(freindlyCastleindex).m_level == 3 && mobIndexStack.size() >= CASTLEUPGRADELEVELTHREECOST) {
            mobsToKill = CASTLEUPGRADELEVELTHREECOST;
            getBuilding(freindlyCastleindex).m_level += 1;
        }
        for (int i = 0; i < mobsToKill; i++) {
            getMob(mobIndexStack.top()).m_Health = -1000;
            mobIndexStack.pop();
        }
    }

}

void Player::buildCastle(Vec2 hexpos)
{
    const iEntityStats& princessStats = iEntityStats::getBuildingStats(iEntityStats::Princess);
    m_Buildings.push_back(new Building(princessStats, getHexCenter(hexpos), m_bNorth));
}

void Player::castleLevelToUnit(Vec2 gridPos)
{
    Vec2 hexPos = getHexPos(gridPos);
    int freindlyCastleindex = -1;
    // check if friendly castle is here 
    for (unsigned int i = 0; i < getNumBuildings(); i++) {
        Vec2 freindlyCastleHexPos = getHexPos(getBuilding(i).m_Position);
        if (int(floor(freindlyCastleHexPos.x)) == int(floor(hexPos.x)) && int(floor(freindlyCastleHexPos.y)) == int(floor(hexPos.y))) {
            freindlyCastleindex = i;
            break;
        }
    }
    if (freindlyCastleindex != -1 && getBuilding(freindlyCastleindex).m_level > 1) {
        getBuilding(freindlyCastleindex).m_level += -1;
        placeMob(iEntityStats::Swordsman,gridPos);
    }
}

// adds given path to given number of mobs in starting location of path
// if -1 is given will add to all mobs
void Player::addPath(std::vector<Vec2> hexPath, int num_mobs )
{
    if (hexPath.empty()) {
        return;
    }
    std::stack<int>mobIndexStack = {};
    int startx = int(floor(hexPath[0].x));
    int starty = int(floor(hexPath[0].y));
    for (unsigned int i = 0; i < getNumMobs(); i++) {
        if (num_mobs == 0) {
            return;
        }
        Vec2 freindlyMobHexPos = getHexPos(getMob(i).m_Position);
        if (int(floor(freindlyMobHexPos.x)) == startx && int(floor(freindlyMobHexPos.y)) == starty) {
            while (!getMob(i).hexpath.empty()) {
                getMob(i).hexpath.pop();
            }
            for (unsigned int b = 0; b < hexPath.size() - 1; b++) {
                getMob(i).hexpath.push(hexPath[hexPath.size() - 1 - b]);
            }
            num_mobs -= 1;
        }
    }
}



// index(index) of the item with  value(val)
// index refers to index in touched_locs
struct ValToIndexStruct
{
    float val;
    int index;

    ValToIndexStruct(float n1, int n2) : val(n1), index(n2)
    {
    }

    bool operator<(const struct ValToIndexStruct& other) const
    {
        //bigger is worse
        return val > other.val;
    }
};

// returns a  [x,y] values of valid neigbors of given location(loc) that are not outside the bounds of max x and y and zero
// valid neighbors change based on the y in a hex grid
vector<Vec2> Player::getValidNeighbors(Vec2 loc)
{
    // standard neigbors
    Vec2 neighbor_a = { loc.x + 1, loc.y };
    Vec2 neighbor_b = { loc.x - 1, loc.y };
    Vec2 neighbor_c = { loc.x - 1, loc.y - 1 };
    Vec2 neighbor_d = { loc.x, loc.y - 1 };
    Vec2 neighbor_e = { loc.x - 1, loc.y + 1 };
    Vec2 neighbor_f = { loc.x, loc.y + 1 };
 // changes because of hexagon allignment
    if (int(floor(loc.y)) % 2 == 1) {
        neighbor_c.x += 1;
        neighbor_d.x += 1;
        neighbor_e.x += 1;
        neighbor_f.x += 1;
    }
    vector<Vec2> validNeighbors = {};
    vector<Vec2> neighbors = { neighbor_a , neighbor_b , neighbor_c , neighbor_d , neighbor_e , neighbor_f };
    for (unsigned int i = 0; i < neighbors.size(); i++) {
        if (locationValid(neighbors[signed int (i)])) {
            validNeighbors.push_back(neighbors[i]);
        }
    }
    return validNeighbors;
}



std::vector<Vec2> Player::getHexPath(Vec2 starthexpos, Vec2 endhexpos)
{
    std::vector<Vec2> finalhexpath = {};
    if (!(locationValid(starthexpos) && locationValid(endhexpos))) {
        cout << "invalid location" << endl;
        return finalhexpath;
    }
    // remmebers if given location has been seen
    bool* have_seen = new bool[(HEX_GRID_HEIGHT+1) * (HEX_GRID_WIDTH+1)]{ false };

    // current location 
    Vec2 current_loc = { 0,0 };
    current_loc.x = starthexpos.x;
    current_loc.y = starthexpos.y;
    int current_loc_index = 0;

    // vector of all touched locations. [x,y,previous_index, path cost] 
    //where the previous_index corrisponds to the index of the previous location on the path which is in this vector and the path cost of getting there
    std::vector<std::array<int, 4>> touched_locs = vector<array<int, 4>>();

    array<int, 4> loc_info_first{ int(floor(current_loc.x)),int(floor(current_loc.y)),current_loc_index,0 };
    touched_locs.push_back(loc_info_first);

    // gets the index(.index) of the item with the current highest value(.val)
    // index refers to index in touched_locs
    priority_queue<ValToIndexStruct>  value_to_loc_index = priority_queue<ValToIndexStruct>();

    while (int(floor(current_loc.x)) != int(floor(endhexpos.x)) || int(floor(current_loc.y)) != int(floor(endhexpos.y))) {
        vector<Vec2> valid_neighbors = getValidNeighbors(current_loc);
        //add neighbors
        for (unsigned int i = 0; i < valid_neighbors.size(); i++) {
            Vec2 cur_neighbor = valid_neighbors[i];
            if (!have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))]) {
                //add neighbor cost to index struct to priority_queue
                int path_cost = 1 + touched_locs[current_loc_index][3];
                // add heuristic cost 
                float cost = path_cost + abs(cur_neighbor.x - endhexpos.x) + abs(cur_neighbor.y - endhexpos.y);;

                value_to_loc_index.push(ValToIndexStruct(cost, touched_locs.size()));
                //add neighbor to touched locations
                array<int, 4> loc_info = { int(floor(cur_neighbor.x)),int(floor(cur_neighbor.y)),current_loc_index,path_cost };

                touched_locs.push_back(loc_info);
                have_seen[(int(floor(cur_neighbor.y)) * HEX_GRID_HEIGHT + int(floor(cur_neighbor.x)))] = true;
            }
        }
        if (value_to_loc_index.empty()) {
            return finalhexpath;
        }
        // new current location
        current_loc_index = value_to_loc_index.top().index;
        value_to_loc_index.pop();
        current_loc.x = float(touched_locs[current_loc_index][0]);
        current_loc.y = float(touched_locs[current_loc_index][1]);
    }
    finalhexpath.push_back({ endhexpos.x, endhexpos.y });
    while (current_loc_index != 0) {
        current_loc_index = touched_locs[current_loc_index][2];
        finalhexpath.insert(finalhexpath.begin(), { touched_locs[current_loc_index][0],touched_locs[current_loc_index][1] });
    }
    return finalhexpath;
}

const Player& Player::GetOpponent() const
{
    const Player& opPlayer = Game::get().getPlayer(!m_bNorth);
    assert(&opPlayer != this);
    return opPlayer;
}