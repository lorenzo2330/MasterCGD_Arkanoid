#define NOMINMAX 
#include "../data.h"
#include "racket.h"
#include <windows.h>
#include <algorithm>

Racket::Racket()
{
	this->w = RACKET_BASE_WIDTH;
    this->posX = (SCREEN_WIDTH - w) * 0.5f;  //Centra orizzontalmente
    this->posY = RACKET_POSY;
	this->h = RACKET_HEIGHT;
	this->speed = RACKET_SPEED;
}

void Racket::Update(float deltaTime, InputManager input)
{
	//In alternativa, si può interrogare direttamente l'hw (mouse / tastiera) -> GetAsyncKeyState('A') & 0x8000)
    bool sinistra = (input.IsKeyDown(VK_LEFT)) || (input.IsKeyDown('A'));
	bool destra = (input.IsKeyDown(VK_RIGHT)) || (input.IsKeyDown('D'));
    float movimento = speed * deltaTime;

	if (sinistra) posX -= movimento;
	if (destra) posX += movimento;

    //Controllo per non uscire dallo schermo
    posX = std::max(0.0f, std::min(posX, SCREEN_WIDTH - w));
}

//Raddoppia dimensioni della racchetta (BonusType::Racket)
void Racket::WidthBonus() { w = std::min(w * RACKET_BONUS_WIDTH_MULTIPLIER, RACKET_MAX_WIDTH); }

//Resetta dimensioni racchetta
void Racket::Reset() { w = RACKET_BASE_WIDTH; posX = (SCREEN_WIDTH - w) * 0.5f; }