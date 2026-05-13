#include "level.h"
#include "brick.h"
#include "../data.h"
#include <cstdlib>

void Level::GenerateRandomGrid()
{
    bricks.clear();
    bricks.reserve(N_COLONNE * N_RIGHE);  //Unica allocazione statica (AdvC++ lezione 2)

    for (int r = 0; r < N_RIGHE; r++) {
        for (int c = 0; c < N_COLONNE; c++) {
            float bx = GRID_LEFT_MARGIN + c * (BRICK_WIDTH + BRICK_PADDING_X);
            float by = GRID_TOP_MARGIN + r * (BRICK_HEIGHT + BRICK_PADDING_Y);
            bricks.emplace_back(bx, by, RandomType());
        }
    }
}

bool Level::AllDestroyed() const { 
    for (const Brick& b : bricks) if (b.on) return false;
    return true;
}

BrickType Level::RandomType() const
{
    int r = rand() % 10;
    if (r >= 0 && r < 2) return BrickType::Green;   //20% verde
	if (r >= 2 && r < 6) return BrickType::Red;     //40% rosso
	return BrickType::Blue;                         //40% blu
}