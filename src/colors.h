#pragma once

struct Color {
	float r, g, b, a;
	Color() : r(0), g(0), b(0), a(1.0f) {}
	Color(float red, float green, float blue, float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha) {}
};

//Ball
inline const Color COLOR_BALL(1.0f, 1.0f, 1.0f);

//BallPredictor (AI)
inline const Color COLOR_PREDICTOR_LINE(float progress) { return Color(0.4f, 0.85f, 1.0f, (0.6f * (1.0f - progress * 0.8f))); }
inline const Color COLOR_PREDICTOR_END(float pulse) { return Color(0.4f, 0.85f, 1.0f, (0.35f + 0.35f * pulse)); }

//Bonus
inline const Color COLOR_BONUS_RACKET(1.0f, 0.85f, 0.0f);				//Default / Racket
inline const Color COLOR_BONUS_BALL(0.0f, 0.9f, 1.0f);					//Bonus pallina doppia
inline const Color COLOR_BONUS_SPEED(1.0f, 0.3f, 0.8f);					//Bonus velocità

//Brick
inline const Color COLOR_BRICK_RED(1.0f, 0.15f, 0.15f);					//Mattoncino rosso (hp == 2)
inline const Color COLOR_BRICK_ORANGE(1.0f, 0.55f, 0.0f);				//Mattoncino rosso (hp == 1)
inline const Color COLOR_BRICK_GREEN(0.1f, 0.9f, 0.2f);					//Mattoncino verde
inline const Color COLOR_BRICK_BLUE(0.2f, 0.4f, 1.0f);					//Mattoncino blu

//Game
inline const Color COLOR_GAME_BACKGROUND(0.05f, 0.05f, 0.1f);			//Sfondo del game

//GameOverScreen
inline const Color COLOR_GO_BACKGROUND(0.0f, 0.0f, 0.0f, 1.0f);			//Sfondo
inline const Color COLOR_GO_PANEL_BACKGROUND(0.85f, 0.1f, 0.1f, 1.0f);	//Pannello (sfondo rosso)
inline const Color COLOR_GO_PANEL_INSIDE(0.08f, 0.08f, 0.14f, 1.0f);	//Pannello (interno nero)
inline const Color COLOR_GO_TEXT_TITLE(1.0f, 0.25f, 0.25f);				//Testo "GAME OVER"
inline const Color COLOR_GO_TEXT_SCORE(1.0f, 1.0f, 1.0f);				//Testo punteggio
inline const Color COLOR_GO_TEXT_INFO(0.55f, 0.55f, 0.65f);				//Testo info "R / ESC"
inline const Color COLOR_GO_BUTTON_BACKGROUND(0.7f, 0.7f, 0.9f);		//Sfondo pulsante
inline const Color COLOR_GO_BUTTON_INSIDE(0.15f, 0.15f, 0.25f);			//Interno pulsante
inline const Color COLOR_GO_BUTTON_INSIDE_HOVERED(0.28f, 0.28f, 0.38f);	//Interno pulsante (hovered)
inline const Color COLOR_GO_BUTTON_TEXT(0.85f, 0.85f, 0.85f);			//Testo pulsante
inline const Color COLOR_GO_BUTTON_TEXT_HOVERED(1.0f, 1.0f, 1.0f);		//Testo pulsante (hovered)

//HUD
inline const Color COLOR_HUD_TEXT(0.8f, 0.9f, 1.0f);

//Racket
inline const Color COLOR_RACKET(0.7f, 0.7f, 0.7f);

