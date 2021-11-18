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

#include "iPlayer.h"

const Vec2 ksInvalidPos;


iPlayer::EntityData::EntityData()
    : m_Stats(iEntityStats::getStats(iEntityStats::InvalidMobType))
    , m_Health(m_Health)
    , m_Position(ksInvalidPos)
    , m_level(m_level)
    , hexpath(hexpath)
{
}

iPlayer::EntityData::EntityData(const iEntityStats& stats, int& health, const Vec2& pos, int& m_level, std::stack<Vec2>& hexpath)

    : m_Stats(stats)
    , m_Health(health)
    , m_Position(pos)
    , m_level(m_level)
    , hexpath(hexpath)
{
}

iPlayer::EntityData::EntityData(const EntityData& rhs)
    : m_Stats(rhs.m_Stats)
    , m_Health(rhs.m_Health)
    , m_Position(rhs.m_Position)
    , m_level(rhs.m_level)
    , hexpath(rhs.hexpath)
{
}


