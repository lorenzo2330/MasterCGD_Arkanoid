#include "brick.h"
#include "bonus.h"
#include "../colors.h"
#include "../data.h"
#include <cstdlib>

Brick::Brick(float x, float y, BrickType t)
{
	this->posX = x;
	this->posY = y;
	this->w = BRICK_WIDTH;
	this->h = BRICK_HEIGHT;
	this->type = t;
	this->on = true;
    
	if (type == BrickType::Red) { hp = 2; } else { hp = 1; }
	if (type == BrickType::Green) { bonusDrop = GetRandomBonusType(); } else { bonusDrop = BonusType::None; }
}


BonusType Brick::Hit() { if (--hp <= 0) { on = false; return bonusDrop; } return BonusType::None; }

Color Brick::GetColor() const
{
	if (type == BrickType::Red) { if (hp == 1) { return COLOR_BRICK_ORANGE; } else { return COLOR_BRICK_RED; } }
    if (type == BrickType::Green) { return COLOR_BRICK_GREEN; }
	return COLOR_BRICK_BLUE;
}

BonusType Brick::GetRandomBonusType() {
    int r = rand() % 10;
    if (r >= 0 && r < 2) return BonusType::Racket;  //20% racket
    if (r >= 2 && r < 7) return BonusType::Ball;	//50% ball
	return BonusType::Speed;						//30% speed
}