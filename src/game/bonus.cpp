#include "bonus.h"
#include "../data.h"
#include "../colors.h"

BonusItem::BonusItem(float brickX, float brickY, float brickW, BonusType t)
{
	this->w = BONUS_WIDTH;
    this->h = BONUS_HEIGHT;
    this->posX = brickX + (brickW - this->w) * 0.5f;
    this->posY = brickY;
	this->velY = BONUS_SPEED;
    this->type = t;
	on = true;
}

void BonusItem::Update(float deltaTime) { posY += velY * deltaTime; if (posY > SCREEN_HEIGHT) on = false; }

Color BonusItem::GetColor() const
{
	if (type == BonusType::Ball) { return COLOR_BONUS_BALL; }
	if (type == BonusType::Speed) { return COLOR_BONUS_SPEED; }
	return COLOR_BONUS_RACKET;
}