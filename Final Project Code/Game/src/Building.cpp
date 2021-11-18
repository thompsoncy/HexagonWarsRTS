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

#include "Building.h"
#include "Constants.h"

Building::Building(const iEntityStats& stats, const Vec2& pos, bool isNorth)
    : Entity(stats, pos, isNorth)
{
    assert(dynamic_cast<const iEntityStats_Building*>(&stats) != NULL);

}

void Building::tick(float deltaTSec)
{
    // Tick the entity first.  This will pick our target, and attack it if it's in range.
    Entity::tick(deltaTSec);

    // this will upgrade the building over time
    if (m_level == 1 && m_TimeSinceLastUpgrade > CASTLEUPGRADELEVELONETIME)
    {
        setLevel(m_level + 1);
        m_TimeSinceLastUpgrade = 0.f;
    }
    if (m_level == 2 && m_TimeSinceLastUpgrade > CASTLEUPGRADELEVELTWOTIME)
    {
        setLevel(m_level + 1);
        m_TimeSinceLastUpgrade = 0.f;
    }
    if (m_level == 3 && m_TimeSinceLastUpgrade > CASTLEUPGRADELEVELTHREETIME)
    {
        setLevel(m_level + 1);
        m_TimeSinceLastUpgrade = 0.f;
    }
}