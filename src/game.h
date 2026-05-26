#pragma once
#include "ai/ballPredictor.h"
#include "game/ball.h"
#include "game/bonus.h"
#include "game/level.h"
#include "game/racket.h"
#include "render/renderer.h"
#include "render/renderer2d.h"
#include "render/textRenderer.h"
#include "ui/gameOverScreen.h"
#include "ui/hud.h"
#include "ui/inputManager.h"
#include "ui/startScreen.h"
#include <vector>
#include <windows.h>
#include "ai/racketAI.h"
#include "render/lightBallRenderer.h"
#include "ui/pauseScreen.h"

enum class GamePhase { StartScreen, Playing, Pause, GameOver };

class Game
{
public:
    Game() = default;
    ~Game() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

	bool Init(HWND hwnd);                           //Inizializzazione e avvio del gioco
	void Shutdown();                                //Pulizia risorse

    InputManager& GetInput() { return input; }      //Usato da WndProc nel main per gestire l'input

    void Update(float deltaTime);                          
    void Render();                                  

	bool IsRunning() const { return isRunning; }    //Per capire quando chiudere il gioco (WM_QUIT)

private:
    Renderer renderer;
    Renderer2D renderer2D;
    TextRenderer textRenderer;
    LightBallRenderer lightBallRenderer;

    Racket racket;
    std::vector<Ball> balls;
    Level level;
    std::vector<BonusItem> bonuses;

    HUD hud;
    GameOverScreen gameOverScreen;
    PauseScreen pauseScreen;
    StartScreen startScreen;
    InputManager input;

    BallPredictor ballPredictor;    //AI: predice la traiettoria della palla più pericolosa
    RacketAI racketAI;              //AI: muove la racchetta automaticamente
    BallLightData BuildLightData() const;

    GamePhase phase = GamePhase::StartScreen;

    bool  isRunning = true, levelHasToStart = true, lightBallMode = false;
	float speedMultiplier = 1.0f;                   
	int currentLevel = 0;

    void ApplySettings(const StartScreenResult& result);

    void SpawnBall();
    void NewLevel();

    void ApplyBonus(BonusType t);
    void BonusDuplicateBalls();
    void BonusIncreaseSpeed();
    void BonusLargerRacket();
    
    void UpdateCollisions();
    void HandlePauseInput();
    void HandleGameOverInput();

    void UpdateRacket(float deltaTime);
};