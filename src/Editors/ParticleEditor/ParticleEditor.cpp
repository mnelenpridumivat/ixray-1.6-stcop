// ParticleEditor.cpp : Определяет точку входа для приложения.
//
#include "stdafx.h"
#include "../../xrEngine/xr_input.h"

#include "xrECore/Splash.h"

void BeginRender()
{
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    RDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    RDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    RDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
    RDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

    RDevice->BeginScene();
}

void EndRender()
{
    RDevice->EndScene();
    RDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    splash::show(IDB_PE);

    splash::update(1, "");

    if (!IsDebuggerPresent()) Debug._initialize(false);
    const char* FSName = "fs.ltx";

    splash::update(5, "");

    CoInitialize(nullptr);

    splash::update(28, "");

    Core._initialize("Patricle", ELogCallback, 1, FSName);

    splash::update(45, "");

    psDeviceFlags.set(rsFullscreen, false);

    Tools = new CParticleTool();
    PTools = (CParticleTool*)Tools;

    splash::update(67, "");

    UI = new CParticleMain();
    UI->RegisterCommands();
    
    splash::update(86, "");

    UIMainForm* MainForm = new UIMainForm();
    ::MainForm = MainForm;
    UI->Push(MainForm, false);

    splash::update(100, "");
    splash::hide();

    //MainForm->Frame();
    bool NeedExit = false;
    while (!NeedExit)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            switch (Event.type)
            {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                EPrefs->SaveConfig();
                NeedExit = true;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                if (UI && REDevice)
                {
                    UI->Resize(Event.window.data1, Event.window.data2, true);
                    EPrefs->SaveConfig();
                }
                break;
            case SDL_EVENT_WINDOW_SHOWN:
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                Device.b_is_Active = true;
                //if (UI) UI->OnAppActivate();

                break;
            case SDL_EVENT_WINDOW_HIDDEN:
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                Device.b_is_Active = false;
                //if (UI)UI->OnAppDeactivate();
                break;

            case SDL_EVENT_KEY_DOWN:
                if (UI)
                {
                    UI->KeyDown(Event.key.keysym.scancode, UI->GetShiftState());
                    UI->ApplyShortCutInput(Event.key.keysym.scancode);
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (UI)UI->KeyUp(Event.key.keysym.scancode, UI->GetShiftState());
                break;

            case SDL_EVENT_MOUSE_MOTION:
                pInput->MouseMotion(Event.motion.xrel, Event.motion.yrel);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                pInput->MouseScroll(Event.wheel.y);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                int mouse_button = 0;
                if (Event.button.button == SDL_BUTTON_LEFT) { mouse_button = 0; }
                if (Event.button.button == SDL_BUTTON_RIGHT) { mouse_button = 1; }
                if (Event.button.button == SDL_BUTTON_MIDDLE) { mouse_button = 2; }
                if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    pInput->MousePressed(mouse_button);
                }
                else {
                    pInput->MouseReleased(mouse_button);
                }
            }
            break;
            }

            if (!UI->ProcessEvent(&Event))
                break;
        }
        MainForm->Frame();
    }

    xr_delete(MainForm);
    Core._destroy();
    return 0;
}
