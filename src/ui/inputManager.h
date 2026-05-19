#pragma once
#include <windows.h>
#include <unordered_set>
#include "uiHelper.h"

class InputManager
{
public:
    float mouseX = 0.0f, mouseY = 0.0f;

	//Gestione movimento mouse
    //LOWORD e HIWORD sono macro per ottenenere le coordinate del mouse
    void OnMouseMove(LPARAM lParam) { mouseX = static_cast<float>(LOWORD(lParam)); mouseY = static_cast<float>(HIWORD(lParam)); }

    void OnMouseButtonDown() { leftButtonPressed = true; } 

    //Gestione pressione tasti
    void OnKeyDown(WPARAM key) { keysDown.insert(static_cast<int>(key)); }
    void OnKeyUp(WPARAM key) { keysDown.erase(static_cast<int>(key)); }
    bool IsKeyDown(int vk) const { return keysDown.count(vk) > 0; }
    
    void EndFrame() { leftButtonPressed = false; }

    //IsHover e IsClicked permettono di sopperire alla mancanza di pulsanti con una vera e propria logica

    //Controlla se il mouse è sopra al rettangolo dato
    bool IsHover(UI::ButtonRect b) const { return mouseX >= b.x && mouseX <= b.x + b.w && mouseY >= b.y && mouseY <= b.y + b.h; }

    //Controlla se si clicca nel rettangolo
    bool IsClicked(UI::ButtonRect b) const { return leftButtonPressed && IsHover(b); }

private:
    bool leftButtonPressed = false;
    std::unordered_set<int> keysDown;

};