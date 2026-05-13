#include "ball.h"
#include "../data.h"
#include <cstdlib>

Ball::Ball(float startX, float startY, float speed){
	this->posX = startX;
	this->posY = startY;
    this->velX = 0;
	this->velY = -speed;    //Velocità negativa (quindi verso l'alto (re:mind: y cresce verso il basso))
	this->r = BALL_RADIUS;
    this->on = true;
}

void Ball::Update(float deltaTime, bool& hitBottom)
{
    //Muove la pallina
    posX += velX * deltaTime;
    posY += velY * deltaTime;

    //Re:mind: il punto (0, 0) è in alto a sinistra, y cresce verso il basso

    //Gestisce eventuali rimbalzi
    if (posY - r < 0.0f) { posY = r; velY = std::abs(velY); }                           //Rimbalzo bordo sopra
    if (posX - r < 0.0f) { posX = r; velX = std::abs(velX); }                           //Rimbalzo bordo sinistro
	if (posX + r > SCREEN_WIDTH) { posX = SCREEN_WIDTH - r;  velX = -std::abs(velX); }  //Rimbalzo bordo destro
    
    //Pallina tocca il bordo inferiore -> game over
    if (posY - r > SCREEN_HEIGHT) { on = false; hitBottom = true; }                   
}

void Ball::Bounce(bool onX) { if (onX) { velX = -velX; } else { velY = -velY; } }