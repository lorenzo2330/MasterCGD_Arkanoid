#pragma once
#include "../render/renderer2d.h"
#include "../render/textRenderer.h"
#include "inputManager.h"
#include "uiHelper.h"

class GameOverScreen
{
public:
    GameOverScreen() = default;
    ~GameOverScreen() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    GameOverScreen(const GameOverScreen&) = delete;
    GameOverScreen& operator=(const GameOverScreen&) = delete;
    GameOverScreen(GameOverScreen&&) = delete;
    GameOverScreen& operator=(GameOverScreen&&) = delete;

    void Init(TextRenderer* tr, Renderer2D* r2d);           //Calcola le dimensioni di pannello e pulsanti

    void SetFinalScore(int score) { finalScore = score; }

    InputManager::Action HandleInput(const InputManager& input) const;

    void Render(const InputManager& input) const;

private:

    TextRenderer* textRenderer = nullptr;
    Renderer2D* renderer2D = nullptr;

    UI::ButtonRect buttonQuickRestart{};
    UI::ButtonRect buttonRestart{};
    UI::ButtonRect buttonQuit{};

    int finalScore = 0;
    float panelX = 0.0f, panelY = 0.0f, panelW = 0.0f, panelH = 0.0f;

};