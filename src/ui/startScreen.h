#pragma once
#include "../data.h"
#include "../render/renderer2d.h"
#include "../render/textRenderer.h"
#include "../ui/inputManager.h"
#include <string>
#include <vector>
#include "uiHelper.h"
#include "../ai/racketAI.h"

struct Settings { std::wstring title; std::vector<std::wstring> choices; int selectedIndex = 0; };

struct StartScreenResult
{
    int trajectoryMode = 0; //0 -> Disattivata | 1 -> Più pericolosa | 2 -> Tutte
    RacketAIMode racketAIMode = RacketAIMode::Off;
    bool isLightBallMode = false;
};

class StartScreen
{
public:
    
    StartScreen() = default;
    ~StartScreen() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    StartScreen(const StartScreen&) = delete;
    StartScreen& operator=(const StartScreen&) = delete;
    StartScreen(StartScreen&&) = delete;
    StartScreen& operator=(StartScreen&&) = delete;

    void Init(TextRenderer* tr, Renderer2D* r2d);

    bool HandleInput(const InputManager& input);    //True quando si preme su conferma

    void Render(const InputManager& input) const;

    StartScreenResult GetResult() const;

private:
    void DrawTitle() const;
    void DrawOptionRow(int index, bool selected) const;

    //< -> posizionato all'inizio della metà destra del pannello
    //> -> posizionato alla fine della metà destra del pannello
    UI::ButtonRect LeftArrowRect(int index) const { return { SS_BUTTON_LEFTARROW_X, SS_BUTTON_ARROW_Y(index), SS_BUTTON_ARROW_SIZE, SS_BUTTON_ARROW_SIZE }; }
    UI::ButtonRect RightArrowRect(int index) const { return { SS_BUTTON_RIGHTARROW_X, SS_BUTTON_ARROW_Y(index), SS_BUTTON_ARROW_SIZE, SS_BUTTON_ARROW_SIZE }; }
    UI::ButtonRect ConfirmRect() const { return { SS_BUTTON_CONFIRM_X, SS_BUTTON_CONFIRM_Y, SS_BUTTON_CONFIRM_W, SS_BUTTON_CONFIRM_H }; }

    std::vector<Settings> settings;
    int selectedSetting = 0;

    //Per evitare ripetizioni dovute alla pressione del tasto
    bool prevLeft = false, prevRight = false, prevUp = false, prevDown = false, prevEnter = false;

    TextRenderer* textRenderer = nullptr;
    Renderer2D* renderer2D = nullptr;
};