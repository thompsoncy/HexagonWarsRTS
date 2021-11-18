#include "Graphics.h"

#include "Constants.h"
#include <algorithm>
#include <stack>
#include <tuple>
Graphics* Singleton<Graphics>::s_Obj = NULL;

Graphics::Graphics() {
	gWindow = SDL_CreateWindow("Crash Loyal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		gRenderer = NULL;
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}
	else {
		//Create renderer for window
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		if (gRenderer == NULL) {
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		}
		else {
			//Initialize renderer color
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags)) {
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			}
		}
	}

    // init the text libraries
//     if (TTF_Init() < 0) {
//         printf("Text library TTF could not be Initialized correctly.\n");
//     }

//     // Load in the font 
//     sans = TTF_OpenFont("fonts/abelregular.ttf", 36);
//     if (!sans) { printf("TTF_OpenFont: %s\n", TTF_GetError()); }
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
}

void Graphics::render() {
    SDL_RenderPresent(gRenderer);
}

void Graphics::resetFrame() {
    drawBG();
    drawUI();
}

void Graphics::drawMob(Entity* m) {
	int alpha = healthToAlpha(m);

	if (m->isNorth())
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, alpha);
	else
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, alpha);

	float centerX = m->getPosition().x * PIXELS_PER_METER;
	float centerY = m->getPosition().y * PIXELS_PER_METER;
	float squareSize = m->getStats().getSize() * PIXELS_PER_METER;

	drawSquare(centerX, centerY, squareSize);

	SDL_Rect stringRect = {
		(int)(centerX - (squareSize / 2.f)),
		(int)(centerY - (squareSize / 2.f)),
		(int)squareSize,
		(int)squareSize
	};
	SDL_Color stringColor = { 0, 0, 0, 255 };
	drawText(m->getStats().getDisplayLetter(), stringRect, stringColor);
}


void Graphics::drawSquare(float centerX, float centerY, float size) {
    // Draws a square at the given pixel coordinate
    SDL_Rect rect = {
        (int)(centerX - (size / 2.f)),
        (int)(centerY - (size / 2.f)),
        (int)(size),
        (int)(size)
    };
    SDL_RenderFillRect(gRenderer, &rect);
}

int Graphics::healthToAlpha(const Entity* e)
{
    float health = std::max(0.f, (float)e->getHealth());
    float maxHealth = (float)e->getStats().getMaxHealth();
    return (int)(((health / maxHealth) * 200.f) + 55.f);
}

void Graphics::drawBuilding(Entity* b) {
    int alpha = healthToAlpha(b);

    if (b->isNorth())
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, alpha);
    else
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, alpha);

    drawSquare(b->getPosition().x * PIXELS_PER_METER,
        b->getPosition().y * PIXELS_PER_METER,
        b->getStats().getSize() * PIXELS_PER_METER);
    if (b->getLevel() == 1) {
        SDL_SetRenderDrawColor(gRenderer, 160, 82, 45, alpha);
    } 
    if (b->getLevel() == 2) {
        SDL_SetRenderDrawColor(gRenderer, 0,0,0, alpha);
    }
    if (b->getLevel() == 3) {
        SDL_SetRenderDrawColor(gRenderer, 192, 192, 192, alpha);
    }
    if (b->getLevel() == 4) {
        SDL_SetRenderDrawColor(gRenderer, 255, 215, 0, alpha);
    }
    drawSquare(b->getPosition().x * PIXELS_PER_METER,
        b->getPosition().y * PIXELS_PER_METER,
        b->getStats().getSize() * PIXELS_PER_METER * .5f);
}

void Graphics::drawText(const char* textToDraw, SDL_Rect messageRect, SDL_Color color) {
    // Draws the given text in a box with the specified position and dimension

//     SDL_Surface* surfaceMessage = TTF_RenderText_Solid(sans, textToDraw, color); // TODO Make this print something other than m
//     if (!surfaceMessage) { printf("TTF_OpenFont: %s\n", TTF_GetError()); }
//     SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
//     if (!message) { printf("Error 2\n"); }
//     //SDL_Rect messageRect = {
//     //    topLeftXPix,
//     //    topLeftYPix,
//     //    boxWidth,
//     //    boxHeight
//     //    //(int)(centerX - (squareSize / 2.f)),
//     //    //(int)(centerY - (squareSize / 2.f)),
//     //    //(int)squareSize,
//     //    //(int)squareSize
//     //};
//     SDL_RenderCopy(gRenderer, message, NULL, &messageRect);
//     SDL_FreeSurface(surfaceMessage);
//     SDL_DestroyTexture(message);
}

// if x and y location are valid will return true
bool validLocation(int x, int y) {
    return x >= 0 && y >= 0 
        && x <= SCREEN_WIDTH_PIXELS/ HEXSCALE
        && y <= SCREEN_HEIGHT_PIXELS / HEXSCALE;
}

// draws hexgonal grid
void Graphics::drawGrid() {

    // will draw grid in black
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

    //records what points we have seen
    bool seen_points_matrix[100][100] = { {false} };

    // {x location, y location, upward oriented}
    // upward oriented neigbors are not upward oriented and the converse is true as well
    std::stack < std::tuple<int, int, bool >> points;
    // starting point
    points.push({ HEXB, 0 , true });
    // This will go through and draw the hexagon grid by drawing lines between every point
    while (points.size() > 0) {
        std::tuple<int, int, bool> current_point = points.top();
        points.pop();
        std::stack < std::tuple<int, int, bool >> possible_points;
        // get neighbors
        if (std::get<2>(current_point)) {
            possible_points.push({ std::get<0>(current_point) + HEXB ,
                std::get<1>(current_point) + HEXA , false});
            possible_points.push({ std::get<0>(current_point) - HEXB,
                std::get<1>(current_point) + HEXA, false });
            possible_points.push({ std::get<0>(current_point),
                std::get<1>(current_point) - HEXSIDE, false });
        }
        else {
            possible_points.push({ std::get<0>(current_point) - HEXB ,
                std::get<1>(current_point) - HEXA , true });
            possible_points.push({ std::get<0>(current_point) + HEXB,
                std::get<1>(current_point) - HEXA, true });
            possible_points.push({ std::get<0>(current_point),
                std::get<1>(current_point) + HEXSIDE, true });
        }
        // draw lines and add points we have not seen and are valid to stack
        while (possible_points.size() > 0) {
            if(validLocation(std::get<0>(possible_points.top()), std::get<1>(possible_points.top()))){
                SDL_RenderDrawLine(gRenderer,
                    std::get<0>(current_point) * HEXSCALE,
                    std::get<1>(current_point) * HEXSCALE,
                    std::get<0>(possible_points.top()) * HEXSCALE,
                    std::get<1>(possible_points.top()) * HEXSCALE);
                if (!seen_points_matrix[std::get<0>(possible_points.top())][std::get<1>(possible_points.top())]) {
                    seen_points_matrix[std::get<0>(possible_points.top())][std::get<1>(possible_points.top())] = true;
                    points.push(possible_points.top());
                }
            }
            possible_points.pop();
        }
    }
}

void Graphics::drawBG() {
    SDL_Rect bgRect = {
        0,
        0,
        SCREEN_WIDTH_PIXELS,
        SCREEN_HEIGHT_PIXELS
    };
    SDL_SetRenderDrawColor(gRenderer, 79, 161, 0, 0xFF); // Dark green
    SDL_RenderFillRect(gRenderer, &bgRect);


    drawGrid();
}

void Graphics::drawWinScreen(int winningSide) {
    if (winningSide == 0) { return; }

    const char* msg = (winningSide > 0) ? "Game Over. North Wins!" : "Game Over. South Wins!";
    int topY = SCREEN_HEIGHT_PIXELS / 5;
    int leftX = SCREEN_WIDTH_PIXELS / 15;
    int height = SCREEN_HEIGHT_PIXELS / 3;
    int width = (SCREEN_WIDTH_PIXELS * 14) / 15;

    SDL_Rect stringRect = { leftX, topY, width, height };
    SDL_Color color = { 0, 0, 0, 255 };
    drawText(msg, stringRect, color);
}

void Graphics::drawUI() {
    // Draws the rectangle to the right of the play area that contains the UI

    SDL_Rect uiRect = {
        (int)(GAME_GRID_WIDTH * PIXELS_PER_METER),
        (int)0,
        (int)(UI_WIDTH * PIXELS_PER_METER),
        (int)(UI_HEIGHT * PIXELS_PER_METER),
    };
    SDL_SetRenderDrawColor(gRenderer, 0x50, 0x50, 0x50, 100);
    SDL_RenderFillRect(gRenderer, &uiRect);


}
