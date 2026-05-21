#include "level.h"
#include "brick.h"
#include "../data.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>

void Level::GenerateRandomGrid(int currentLevel)
{
    int nColonne = N_MAX_COLONNE;   //Avanzamento progressivo in base al livello
    int nRighe = std::min(currentLevel, N_MAX_RIGHE);           //Avanzamento progressivo in base al livello

    bricks.clear();
    bricks.reserve(static_cast<std::vector<Brick, std::allocator<Brick>>::size_type>(nColonne) * nRighe);  //Unica allocazione statica (AdvC++ lezione 2)

    for (int r = 0; r < nRighe; r++) {
        for (int c = 0; c < nColonne; c++) {
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
	if (r >= 2 && r < 6) return BrickType::Red;     //50% rosso
	return BrickType::Blue;                         //30% blu
}