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

#include "Mob.h"

#include "Constants.h"
#include "Game.h"
#include "Player.h"

#include <algorithm>
#include <vector>
// returns the hex postion of given grid pos
const Vec2 getMobHexPos(Vec2 gridpos) {
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
const Vec2 getHexCenterMob(Vec2 hexpos) {
    float pixelX = (HEXB + (HEXB * 2 * hexpos.x)) * HEXSCALE;
    if (int(floor(hexpos.y)) % 2 == 1) {
        pixelX += HEXB * HEXSCALE;
    }
    float pixelY = ((HEXA + (HEXSIDE / 2.f)) + ((HEXA + HEXSIDE) * hexpos.y)) * HEXSCALE;
    return Vec2(pixelX / PIXELS_PER_METER, pixelY / PIXELS_PER_METER);
}


Mob::Mob(const iEntityStats& stats, const Vec2& pos, bool isNorth)
    : Entity(stats, pos, isNorth)
    , m_pWaypoint({ -1,-1 })
{
    assert(dynamic_cast<const iEntityStats_Mob*>(&stats) != NULL);
}

void Mob::tick(float deltaTSec)
{
    // Tick the entity first.  This will pick our target, and attack it if it's in range.
    Entity::tick(deltaTSec);

    // if our target isn't in range, move towards it.
    if (!targetInRange())
    {
        move(deltaTSec);
    }
}

void Mob::move(float deltaTSec)
{
    // If we have a target and it's on the same side of the river, we move towards it.
    //  Otherwise, we move toward the bridge.ss
    bool bMoveToTarget = false;
    Vec2 currenthexpos = getMobHexPos(m_Pos);
    if (!!m_pTarget)
    {    
        bool imTop = m_Pos.y < (GAME_GRID_HEIGHT / 2);
        bool otherTop = m_pTarget->getPosition().y < (GAME_GRID_HEIGHT / 2);
        Vec2 targethexpos = getMobHexPos(m_pTarget->getPosition());
        Vec2 currenthexpos = getMobHexPos(m_Pos);
        if (int(floor(targethexpos.x)) == int(floor(currenthexpos.x)) && int(floor(targethexpos.y)) == int(floor(currenthexpos.y)))
        {
            bMoveToTarget = true;
        }
    }
    

    Vec2 destPos;
    if (bMoveToTarget)
    { 
        destPos = m_pTarget->getPosition();
    }
    else
    {
        if (m_pWaypoint.x == -1)
        {
            if (!hexpath.empty()) {
                Vec2 newWayPoint = getHexCenterMob(hexpath.top());
                m_pWaypoint = getHexCenterMob(hexpath.top());
                // might be an issue
                hexpath.pop();
            }
            else {
                return;
            }
        }
        destPos = m_pWaypoint;
    }

    // Actually do the moving
    Vec2 moveVec = destPos - m_Pos;
    float distRemaining = moveVec.normalize();
    float moveDist = m_Stats.getSpeed() * deltaTSec;

    // if we're moving to m_pTarget, don't move into it
    if (bMoveToTarget)
    {
        assert(m_pTarget);
        distRemaining -= (m_Stats.getSize() + m_pTarget->getStats().getSize()) / 2.f;
        distRemaining = std::max(0.f, distRemaining);
    }

    if (moveDist <= distRemaining)
    {
        m_Pos += moveVec * moveDist;
    }
    else
    {
        m_Pos += moveVec * distRemaining;

        // if the destination was a waypoint, find the next one and continue movement
        if (m_pWaypoint.x != -1)
        {
            if (hexpath.empty()) {
                m_pWaypoint = { -1,-1 };
            }
            else {
                Vec2 newWayPoint = getHexCenterMob(hexpath.top());
                m_pWaypoint = getHexCenterMob(hexpath.top());
                hexpath.pop();
                destPos = m_pWaypoint;
                moveVec = destPos - m_Pos;
                moveVec.normalize();
                m_Pos += moveVec * distRemaining;
            }
        }
    }

    // PROJECT 2: This is where your collision code will be called from
    Mob* otherMob = checkCollision();
    if (otherMob) {
        processCollision(otherMob, deltaTSec);
    }
}

const Vec2* Mob::pickWaypoint()
{
    float smallestDistSq = FLT_MAX;
    const Vec2* pClosest = NULL;

    for (const Vec2& pt : Game::get().getWaypoints())
    {
        // Filter out any waypoints that are behind (or barely in front of) us.
        // NOTE: (0, 0) is the top left corner of the screen
        float yOffset = pt.y - m_Pos.y;
        if ((m_bNorth && (yOffset < 1.f)) ||
            (!m_bNorth && (yOffset > -1.f)))
        {
            continue;
        }

        float distSq = m_Pos.distSqr(pt);
        if (distSq < smallestDistSq) {
            smallestDistSq = distSq;
            pClosest = &pt;
        }
    }

    return pClosest;
}

// PROJECT 2: 
//  1) return a vector of mobs that we're colliding with
//  2) handle collision with towers & river 
Mob* Mob::checkCollision() 
{
    const Player& northPlayer = Game::get().getPlayer(true);
    for (const Entity* pOtherMob : northPlayer.getMobs())
    {
        if (this == pOtherMob) 
        {
            continue;
        }

        // PROJECT 2: YOUR CODE CHECKING FOR A COLLISION GOES HERE
    }

    const Player& southPlayer = Game::get().getPlayer(false);
    for (const Entity* pOtherMob : southPlayer.getMobs())
    {
        if (this == pOtherMob)
        {
            continue;
        }

        // PROJECT 2: YOUR CODE CHECKING FOR A COLLISION GOES HERE
    }

    return NULL;
}

void Mob::processCollision(Mob* otherMob, float deltaTSec) 
{
    // PROJECT 2: YOUR COLLISION HANDLING CODE GOES HERE
}

