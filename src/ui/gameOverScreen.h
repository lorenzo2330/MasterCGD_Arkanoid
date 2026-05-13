#pragma once
#include "../colors.h"
#include "../render/textRenderer.h"
#include "inputManager.h"
#include "../render/renderer2d.h"
#include <string>


class GameOverScreen
{
public:
    enum class Action { None, Restart, Quit };

    GameOverScreen() = default;
    ~GameOverScreen() = default;

    void Init(TextRenderer* tr, Renderer2D* r2d);           //Calcola le dimensioni di pannello e pulsanti

    void SetFinalScore(int score) { finalScore = score; }

    Action HandleInput(const InputManager& input) const;

    void Render(const InputManager& input) const;

private:

    void DrawButton(const ButtonRect& btn, const std::wstring& label, bool hovered) const;

    TextRenderer* textRenderer = nullptr;
    Renderer2D* renderer2D = nullptr;

    ButtonRect buttonRestart{};
    ButtonRect buttonQuit{};

    int finalScore = 0;
    float panelX = 0.0f, panelY = 0.0f, panelW = 0.0f, panelH = 0.0f;

    Color GetButtonColor(bool hovered) const;
    Color GetButtonTextColor(bool hovered) const; 
};