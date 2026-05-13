#pragma once

inline constexpr float SCREEN_WIDTH = 800.0f;
inline constexpr float SCREEN_HEIGHT = 600.0f;

//Ball
inline constexpr float BALL_START_SPEED = 300.0f;
inline constexpr float BALL_START_DISTANCE = 30.0f;
inline constexpr float BALL_RADIUS = 7.0f;
inline constexpr float BALL_DEVIATION = 0.85f;

//Bonus
inline constexpr float BONUS_WIDTH = 20.0f;				//Larghezza del bonus
inline constexpr float BONUS_HEIGHT = 20.0f;			//Altezza del bonus
inline constexpr float BONUS_SPEED = 150.0f;			//Velocità di caduta del bonus
inline constexpr float BONUS_SPEED_MULTIPLIER = 1.3f;	//Multiplier della speed una volta ottenuto il bonus

//Brick
inline constexpr float BRICK_WIDTH = 64.0f;
inline constexpr float BRICK_HEIGHT = 22.0f;
inline constexpr float BRICK_PADDING_X = 8.0f;
inline constexpr float BRICK_PADDING_Y = 6.0f;

//Game
inline constexpr int SCORE_BLUE = 10;
inline constexpr int SCORE_RED = 15;
inline constexpr int SCORE_GREEN = 20;

//GameOverScreen
inline constexpr float GO_BUTTON_WIDTH = 200.0f;
inline constexpr float GO_BUTTON_HEIGHT = 46.0f;
inline constexpr float GO_BUTTON_GAP = 18.0f;
inline constexpr float GO_BUTTON_START_Y = 170.0f;
inline constexpr float GO_FONTSIZE_TITLE = 38.0f;
inline constexpr float GO_FONTSIZE_SCORE = 22.0f;
inline constexpr float GO_FONTSIZE_INFO = 13.0f;

//HUD
inline constexpr float HUD_SCORE_X = 10.0f;
inline constexpr float HUD_SCORE_Y = 12.0f;
inline constexpr float HUD_SCORE_WIDTH = 180.0f;
inline constexpr float HUD_SCORE_HEIGHT = 20.0f;
inline constexpr float HUD_LEVEL_X = SCREEN_WIDTH - 200.0f;
inline constexpr float HUD_LEVEL_Y = 12.0f;
inline constexpr float HUD_LEVEL_WIDTH = 180.0f;
inline constexpr float HUD_LEVEL_HEIGHT = 20.0f;

//Level
inline constexpr int N_COLONNE = 10;
inline constexpr int N_RIGHE = 5;
inline constexpr float GRID_TOP_MARGIN = 55.0f;     
inline constexpr float GRID_LEFT_MARGIN = 36.0f;


//Racchetta
inline constexpr float RACKET_BASE_WIDTH = 120.0f;
inline constexpr float RACKET_MAX_WIDTH = RACKET_BASE_WIDTH * 2.0f;
inline constexpr float RACKET_POSY = SCREEN_HEIGHT - 50.0f;
inline constexpr float RACKET_HEIGHT = 16.0f;
inline constexpr float RACKET_SPEED = 400.0f;
inline constexpr float RACKET_BONUS_WIDTH_MULTIPLIER = 2.0f;