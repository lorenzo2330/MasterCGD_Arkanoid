#pragma once
#include "../colors.h"
#include "../render/renderer2d.h"
#include "../render/textRenderer.h"
#include <string>

namespace UI
{
    struct ButtonRect { float x, y, w, h; };

    struct ButtonStyle {
        Color bgColor, borderColor, textColor; 
        float fontSize = 16.0f, textOffsetY = 13.0f, borderSize = 2.0f;
    };

    inline ButtonStyle DefaultButton() { return { COLOR_GO_BUTTON_INSIDE, COLOR_GO_BUTTON_BACKGROUND, COLOR_GO_BUTTON_TEXT }; }

    inline ButtonStyle HoveredButton() { return { COLOR_GO_BUTTON_INSIDE_HOVERED, COLOR_GO_BUTTON_BACKGROUND, COLOR_GO_BUTTON_TEXT_HOVERED }; }

    inline ButtonStyle ArrowButton() { return { COLOR_GO_BUTTON_INSIDE, COLOR_GO_BUTTON_BACKGROUND, COLOR_GO_BUTTON_TEXT, 13.0f, 4.0f }; }

    inline ButtonStyle ArrowButtonHovered() { return { COLOR_GO_BUTTON_INSIDE_HOVERED, COLOR_GO_BUTTON_BACKGROUND, COLOR_GO_BUTTON_TEXT_HOVERED, 13.0f, 4.0f }; }

    inline void DrawButton(Renderer2D& r2d, TextRenderer& tr, const ButtonRect& btn, const std::wstring& label, const ButtonStyle& style)
    {
        float b = style.borderSize;

        //Bordo del pulsante
        r2d.DrawRect(btn.x - b, btn.y - b, btn.w + b * 2.0f, btn.h + b * 2.0f, style.borderColor);

        //Interno del pulsante
        r2d.DrawRect(btn.x, btn.y, btn.w, btn.h, style.bgColor);

        //Testo centrato
        tr.BeginDraw();
        tr.DrawCenteredText(label, btn.x, btn.y + style.textOffsetY, btn.w, style.fontSize, style.textColor);
        tr.EndDraw();
    }

    inline void DrawButton(
        Renderer2D& r2d, TextRenderer& tr, const ButtonRect& btn, const std::wstring& label, 
        bool hovered, const ButtonStyle& normal = DefaultButton(), const ButtonStyle& hov = HoveredButton())
    {
        DrawButton(r2d, tr, btn, label, hovered ? hov : normal);
    }

    struct TextStyle { float x, y, w, fontSize; Color c; };

    inline void HDrawCenteredText(TextRenderer& tr, const std::wstring& text, TextStyle ts) {
        tr.BeginDraw();
        tr.DrawCenteredText(text, ts.x, ts.y, ts.w, ts.fontSize, ts.c);
        tr.EndDraw();
    }
}