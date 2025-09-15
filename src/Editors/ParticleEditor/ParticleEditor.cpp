// ParticleEditor.cpp : Определяет точку входа для приложения.
//
#include "stdafx.h"
#include "../../xrEngine/xr_input.h"

#include "xrECore/Splash.h"

extern ECORE_API xr_token2* actions_token;
extern xr_token2 actions_token_impl[];

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS))
	{
		Msg("! SDL_Init Error: %s", SDL_GetError());
		return 0;
	}
    
    PROF_START_CAPTURE();

	splash::show(IDB_PE);

    START_PROFILE("Initializing Debugger")
	splash::update(5, "Initializing Debugger");

	if (!IsDebuggerPresent()) Debug._initialize(false);
    STOP_PROFILE

    START_PROFILE("Initializing Debugger")
    splash::update(10, "Initializing COM Library");

    CoInitialize(nullptr);
    STOP_PROFILE

    START_PROFILE("Core Initialization")
    splash::update(20, "Core Initialization");

    const char* FSName = "fs.ltx";
    Core._initialize("Particle", ELogCallback, 1, FSName);

    psDeviceFlags.set(rsFullscreen, false);
    STOP_PROFILE

    START_PROFILE("Initializing Particle Tools")
    splash::update(35, "Initializing Particle Tools");
    actions_token = actions_token_impl;
    Tools = new CParticleTool();
    PTools = (CParticleTool*)Tools;
    STOP_PROFILE

    START_PROFILE("Registering UI Commands")
    splash::update(55, "Registering UI Commands");

    UI = new CParticleMain();
    UI->RegisterCommands();
    STOP_PROFILE
    
    START_PROFILE("Registering UI Commands")
    splash::update(75, "Creating Main UI Form");

    UIMainForm* MainForm = new UIMainForm();
    ::MainForm = MainForm;
    UI->Push(MainForm, false);
    STOP_PROFILE

    START_PROFILE("Registering UI Commands")
    splash::update(100, "Finalizing");
    splash::hide();
    STOP_PROFILE

    PROF_STOP_CAPTURE();
    PROF_SAVE_CAPTURE("programm-capture-startup.opt");

	//MainForm->Frame();
	bool NeedExit = false;
	while (!NeedExit)
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
			case SDL_EVENT_WINDOW_MAXIMIZED:
				EDevice->MaximizedWindow();
				break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				SDL_WindowID MainWndID = SDL_GetWindowID(g_AppInfo.Window);
				if (Event.window.windowID == MainWndID)
				{
					EPrefs->SaveConfig();
					NeedExit = true;
				}
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{
				SDL_WindowID MainWndID = SDL_GetWindowID(g_AppInfo.Window);
				if (UI && REDevice && Event.window.windowID == MainWndID)
				{
					UI->Resize(Event.window.data1, Event.window.data2, true);
					EPrefs->SaveConfig();
				}
				break;
			}
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
					UI->KeyDown(Event.key.scancode, UI->GetShiftState());
					UI->ApplyShortCutInput(Event.key.scancode);
				}
				break;
			case SDL_EVENT_KEY_UP:
				if (UI)UI->KeyUp(Event.key.scancode, UI->GetShiftState());
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
