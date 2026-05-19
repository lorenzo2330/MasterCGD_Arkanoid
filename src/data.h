#pragma once
#include <cmath>


inline constexpr float SCREEN_WIDTH = 800.0f;
inline constexpr float SCREEN_HEIGHT = 600.0f;

//Ball
inline constexpr float BALL_DEVIATION = 0.85f;
inline constexpr float BALL_RADIUS = 7.0f;
inline constexpr float BALL_START_DISTANCE = BALL_RADIUS + 3.0f;
inline constexpr float BALL_START_SPEED = 300.0f;		//Default: 300

//BallPredictor (AI)
inline constexpr int PREDICTOR_MAX_SIM = 300;			//Limite sul numero totale di iterazioni nel loop di simulazione
inline constexpr float PREDICTOR_FIXED_STEP = 3.0f;		//Ogni quanti pixel eseguire la simulazione fisica della traiettoria
inline constexpr float PREDICTOR_LINE_THICKNESS = 5.0f;	//Spessore linea di predizione
inline constexpr float PREDICTOR_DASH_SPEED = 1.5f;		//Cicli di animazione al secondo
inline constexpr float PREDICTOR_DASH_ON = 12.0f;		//Pixel "accessi" per "pezzettino"
inline constexpr float PREDICTOR_DASH_OFF = 8.0f;		//Pixel "spenti" per "pezzettino";
inline constexpr float PREDICTOR_DASH_PERIOD = PREDICTOR_DASH_ON + PREDICTOR_DASH_OFF;	//Ogni 20px si ripete il "pezzettino"
inline constexpr int PREDICTOR_MAX_BOUNCE = 5;

inline float PREDICTOR_END_PULSE(float dashTimer) { return 0.5f + 0.5f * std::sin(dashTimer * 5.0f); }
inline float PREDICTOR_END_RADIUS(float pulse) { return 5.0f + pulse * 2.0f; }	//Raggio del punto di destinazione della pallina
inline float PREDICTOR_END_Y(float pulse) { return SCREEN_HEIGHT - PREDICTOR_END_RADIUS(pulse); }

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
inline constexpr float SPEED_INCREASER = 0.1f;

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
inline constexpr int N_MAX_COLONNE = 10;
inline constexpr int N_MAX_RIGHE = 5;
inline constexpr float GRID_TOP_MARGIN = 55.0f;     
inline constexpr float GRID_LEFT_MARGIN = 36.0f;


//Racchetta
inline constexpr float RACKET_BASE_WIDTH = 120.0f;
inline constexpr float RACKET_MAX_WIDTH = RACKET_BASE_WIDTH * 2.0f;
inline constexpr float RACKET_POSY = SCREEN_HEIGHT - 50.0f;
inline constexpr float RACKET_HEIGHT = 16.0f;
inline constexpr float RACKET_SPEED = 400.0f;
inline constexpr float RACKET_BONUS_WIDTH_MULTIPLIER = 2.0f;

//StartScreen
// ── Costanti di layout (derivate da SCREEN_WIDTH / SCREEN_HEIGHT) ─────────

inline constexpr float ROW_H = 56.0f;    // Altezza di ogni riga opzione


inline constexpr float OPTIONS_START_Y = 130.0f;   // Y relativa al (top del) pannello dove iniziano le opzioni

inline constexpr float SS_PANEL_W = 520.0f;
inline constexpr float SS_PANEL_H = 460.0f;

inline constexpr float SS_PANEL_LEFT = (SCREEN_WIDTH - SS_PANEL_W) * 0.5f;
inline constexpr float SS_PANEL_TOP = (SCREEN_HEIGHT - SS_PANEL_H) * 0.5f;
inline constexpr float SS_PANEL_CENTERX = SCREEN_WIDTH * 0.5f;

inline constexpr float SS_TITLE_X = SS_PANEL_LEFT;
inline constexpr float SS_TITLE_Y = SS_PANEL_TOP + 20.0f;
inline constexpr float SS_TITLE_W = SS_PANEL_W;
inline constexpr float SS_TITLE_FONT = 36.0f;
inline constexpr float SS_SUBTITLE_X = SS_PANEL_LEFT;
inline constexpr float SS_SUBTITLE_Y = SS_PANEL_TOP + 68.0f;
inline constexpr float SS_SUBTITLE_W = SS_PANEL_W;
inline constexpr float SS_SUBTITLE_FONT = 13.0f;

inline constexpr float SS_ROW_SELECTED_X = SS_PANEL_LEFT + 10.0f;
inline constexpr float SS_ROW_TITLE_X = SS_PANEL_LEFT + 18.0f;
inline constexpr float SS_ROW_TITLE_W = SS_PANEL_W * 0.5f - 26.0f;
inline constexpr float SS_ROW_FONTSIZE = 13.0f;

inline float SS_ROW_CENTERY(int index) { return SS_PANEL_TOP + OPTIONS_START_Y + index * ROW_H + ROW_H * 0.5f; }
inline float SS_ROW_TOP(int index) { return SS_ROW_CENTERY(index) - ROW_H * 0.5f + 4.0f; };
inline float SS_ROW_TEXTY(int index) { return SS_ROW_TOP(index) + 15.0f; };

inline constexpr float SS_BUTTON_ARROW_SIZE = 28.0f;
inline constexpr float SS_BUTTON_LEFTARROW_X = SS_PANEL_LEFT + SS_PANEL_W * 0.5f + 6.0f;
inline float SS_BUTTON_ARROW_Y(int index) { return SS_ROW_CENTERY(index) - SS_BUTTON_ARROW_SIZE * 0.5f; }
inline constexpr float SS_BUTTON_RIGHTARROW_X = SS_PANEL_LEFT + SS_PANEL_W - 14.0f - SS_BUTTON_ARROW_SIZE;

inline constexpr float SS_BUTTON_CONFIRM_H = 46.0f;
inline constexpr float SS_BUTTON_CONFIRM_W = 200.0f;
inline constexpr float SS_BUTTON_CONFIRM_Y = SS_PANEL_TOP + SS_PANEL_H - SS_BUTTON_CONFIRM_H - 22.0f;
inline constexpr float SS_BUTTON_CONFIRM_X = SS_PANEL_CENTERX - SS_BUTTON_CONFIRM_W * 0.5f;

// ---------------------------------------------------------------------------
// RacketAI
// ---------------------------------------------------------------------------

//Pesi dei bonus: determinano quanto l'AI valuta ciascun tipo di bonus
//Usati in BonusPriority e Adaptive. Ball è il più prezioso (pallina extra).
inline constexpr float AI_BONUS_WEIGHT_BALL = 3.0f;
inline constexpr float AI_BONUS_WEIGHT_RACKET = 2.0f;
inline constexpr float AI_BONUS_WEIGHT_SPEED = 1.0f;

//Ritardo di reazione in secondi: l'AI non aggiorna il target ogni frame,
//ma aspetta questo intervallo per sembrare più "umana" e meno perfetta.
inline constexpr float AI_REACTION_DELAY = 0.08f;

//Range dell'errore intenzionale in pixel (modalità BallPriority).
//Il target viene spostato di un valore casuale in [-AI_ERROR_RANGE, +AI_ERROR_RANGE].
inline constexpr float AI_ERROR_RANGE = 12.0f;

//Distanza in pixel dalla racchetta entro cui la pallina viene considerata "urgente"
//in modalità Adaptive. Sotto questa distanza l'AI smette di inseguire bonus.
inline constexpr float AI_URGENCY_DISTANCE = 200.0f;

//Quanto peso dare al bonus vs pallina in modalità Adaptive quando l'urgenza è bassa.
//0 = mai inseguire bonus, 1 = ignorare completamente la pallina per il bonus.
inline constexpr float AI_ADAPTIVE_BONUS_BLEND = 0.55f;




