#pragma once
#include "../render/renderer2d.h"
#include "../render/textRenderer.h"
#include "inputManager.h"
#include "uiHelper.h"

class PauseScreen
{
public:
    enum class Action { None, Continue, Restart, QuickRestart, Quit };

    PauseScreen() = default;
    ~PauseScreen() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    PauseScreen(const PauseScreen&) = delete;
    PauseScreen& operator=(const PauseScreen&) = delete;
    PauseScreen(PauseScreen&&) = delete;
    PauseScreen& operator=(PauseScreen&&) = delete;

    void Init(TextRenderer* tr, Renderer2D* r2d);

    void SetGameInfo(int s, int l) { score = s; level = l; }

    Action HandleInput(const InputManager& input) const;

    void Render(const InputManager& input) const;

private:
    TextRenderer* textRenderer = nullptr;
    Renderer2D* renderer2D = nullptr;

    UI::ButtonRect buttonContinue{};
    UI::ButtonRect buttonRestart{};
    UI::ButtonRect buttonQuickRestart{};
    UI::ButtonRect buttonQuit{};

    int score = 0;
    int level = 0;

    float panelX = 0.0f, panelY = 0.0f, panelW = 0.0f, panelH = 0.0f;
};