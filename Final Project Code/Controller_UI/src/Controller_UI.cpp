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

#include "Controller_UI.h"

#include "Constants.h"
#include "EntityStats.h"
#include "iPlayer.h"
#include "Vec2.h"
#include <vector>
#include "SDL.h"

Controller_UI* Singleton<Controller_UI>::s_Obj = NULL;

Controller_UI::~Controller_UI()
{
    std::cout << "Controller_UI is being deleted. This probably means that "
        << "you made more than one." << std::endl;
}

void Controller_UI::tick(float deltaTSec) {
    while(!events.empty()) {
        SDL_Event e = events.front();
        events.pop();

        if ((e.type == SDL_MOUSEBUTTONUP) && (e.button.button == SDL_BUTTON_LEFT)) {
            int pixelX = -1;
            int pixelY = -1;
            SDL_GetMouseState(&pixelX, &pixelY);


            const Vec2 mousePos((pixelX / (0.f  + PIXELS_PER_METER)), (pixelY / (0.f + PIXELS_PER_METER)));
            iEntityStats::MobType mobType;

            assert(m_pPlayer);
            if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_U])
            {
                m_pPlayer->castleLevelToUnit(mousePos);
            }
            else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C])
            {
                m_pPlayer->unitToCastle(mousePos);
            }
            else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S])
            {
                startHex = m_pPlayer->getHexPos(mousePos);
                if (startHex.x >= 0 && startHex.y >= 0 && endHex.x >= 0 && endHex.y >= 0) {
                    std::vector<Vec2> path = m_pPlayer->getHexPath(startHex, endHex);
                    m_pPlayer->addPath(path, 1);
                }
            }
            else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_E])
            {
                endHex = m_pPlayer->getHexPos(mousePos);
                if (startHex.x >= 0 && startHex.y >= 0 && endHex.x >= 0 && endHex.y >= 0) {
                    std::vector<Vec2> path = m_pPlayer->getHexPath(startHex, endHex);
                    m_pPlayer->addPath(path, 1);
                }
            }
            else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A])
            {
                if (startHex.x >= 0 && startHex.y >= 0 && endHex.x >= 0 && endHex.y >= 0) {
                    std::vector<Vec2> path = m_pPlayer->getHexPath(startHex, endHex);
                    m_pPlayer->addPath(path, 10000);
                }
            }
        }
    }
}

void Controller_UI::loadEvent(SDL_Event e) {
    events.push(e);
}