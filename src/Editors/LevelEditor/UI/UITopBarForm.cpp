#include "stdafx.h"
#include "UITopBarForm.h"
#include <shellapi.h>

UITopBarForm::UITopBarForm()
{
    m_tUndo                  = EDevice->Resources->_CreateTexture("ed\\bar\\Undo");
    m_timeUndo               = 0;
    m_tRedo                  = EDevice->Resources->_CreateTexture("ed\\bar\\Redo");
    m_timeRedo               = 0;
    m_tNew                   = EDevice->Resources->_CreateTexture("ed\\bar\\new");
    m_tOpen                  = EDevice->Resources->_CreateTexture("ed\\bar\\open");
    m_tSave                  = EDevice->Resources->_CreateTexture("ed\\bar\\save");
    m_tCForm                 = EDevice->Resources->_CreateTexture("ed\\bar\\CForm");
    m_tAIMap                 = EDevice->Resources->_CreateTexture("ed\\bar\\AIMap");
    m_tGGraph                = EDevice->Resources->_CreateTexture("ed\\bar\\GGraph");
    m_tPlayInEditor          = EDevice->Resources->_CreateTexture("ed\\bar\\play_in_editor");
    m_tPlayPC                = EDevice->Resources->_CreateTexture("ed\\bar\\play_pc");
    m_tBuildAndMake          = EDevice->Resources->_CreateTexture("ed\\bar\\build_all");
    m_tPlayCleanGame         = EDevice->Resources->_CreateTexture("ed\\bar\\play_clean_game");
    m_tTerminated            = EDevice->Resources->_CreateTexture("ed\\bar\\terminated");

    m_tReloadConfigs         = EDevice->Resources->_CreateTexture("ed\\bar\\reload_configs");
    m_tOpenGameData          = EDevice->Resources->_CreateTexture("ed\\bar\\open_gamedata");
    m_VerifySpaceRestrictors = false;
}

UITopBarForm::~UITopBarForm() {}

#define IMGUI_HINT_BUTTON(Name, Ptr, Hint, Callback) \
			Ptr->Load(); \
			if (ImGui::ImageButton("##" Name, Ptr->pSurface, ImVec2(20, 20))) \
				Callback(); \
			if (ImGui::IsItemHovered()) \
			{ \
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand); \
				ImGui::SetTooltip(Hint); \
			} \
			ImGui::SameLine()

#define IMGUI_HINT_BUTTON_EX(Name, Ptr, Timer, Hint, Callback) \
			Ptr->Load(); \
			if (ImGui::ImageButton("##" Name, Ptr->pSurface, ImVec2(20, 20), ImVec2(Timer > EDevice->TimerAsync() ? 0.5 : 0, 0), ImVec2(m_timeUndo > EDevice->TimerAsync() ? 1 : 0.5, 1))) \
			{ \
				Callback(); \
				Timer = EDevice->TimerAsync() + 130;\
			} \
			if (ImGui::IsItemHovered()) \
			{ \
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand); \
				ImGui::SetTooltip(Hint); \
			} \
			ImGui::SameLine()

void UITopBarForm::Draw()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + UI->GetMenuBarHeight()));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, UIToolBarSize));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2( 2,0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-2, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

	if (ImGui::Begin("TOOLBAR", NULL, window_flags))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6);

		if (ImGui::BeginTable("##ToolbarTable", 8, ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Hideable))
		{
			ImGui::TableSetupColumn("Actions");
			ImGui::TableSetupColumn("File");
			ImGui::TableSetupColumn("PIE Pre-Build");
			ImGui::TableSetupColumn("PIE Actions");
			ImGui::TableSetupColumn("Compile Actions");
			ImGui::TableSetupColumn("Engine");
			ImGui::TableSetupColumn("Directory Actions");
			ImGui::TableSetupColumn("Sound Preferences");

			if (ImGui::TableNextColumn())
			{
				IMGUI_HINT_BUTTON_EX("Undo", m_tUndo, m_timeUndo, "Undo the last action", ClickUndo);
				IMGUI_HINT_BUTTON_EX("Redo", m_tRedo, m_timeRedo, "Repeat the last action", ClickRedo);
			}


			if (ImGui::TableNextColumn())
			{
				IMGUI_HINT_BUTTON("New", m_tNew, "Clear/New Scene", ClickNew);
				IMGUI_HINT_BUTTON("Open", m_tOpen, "Open level", ClickOpen);
				IMGUI_HINT_BUTTON("Save", m_tSave, "Save level", ClickSave);
			}

			if (ImGui::TableNextColumn())
			{
				IMGUI_HINT_BUTTON("BuildCForm", m_tCForm, "Build CFORM", ClickCForm);
				IMGUI_HINT_BUTTON("BuildAIMap", m_tAIMap, "Build AI-Map", ClickAIMap);
				IMGUI_HINT_BUTTON("BuildGameGraph", m_tGGraph, "Build Game Graph", ClickGGraph);
			}

			if (ImGui::TableNextColumn())
			{
				if (LTools->IsCompilerRunning() || LTools->IsGameRunning())
				{
					IMGUI_HINT_BUTTON("StopPIE", m_tTerminated, "Stop Play in Editor", ClickTerminated);
				}
				else if (Scene->IsPlayInEditor())
				{
					IMGUI_HINT_BUTTON("StopPIE", m_tTerminated, "Stop Play in Editor", Scene->Stop);
				}
				else
				{
					IMGUI_HINT_BUTTON("StartPIE", m_tPlayInEditor, "Start Play in Editor", ClickPlayInEditor);
				}

				if (ImGui::ArrowButton("##PlaySettings", ImGuiDir_Down, ImVec2(ImGui::GetFrameHeight(), 20), 0))
				{
					ImGui::OpenPopup("test");
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					ImGui::SetTooltip("Play in Editor settings");
				}

				ImGui::SameLine();
				if (ImGui::BeginPopup("test"))
				{
					ImGui::Checkbox("Verify space restrictors", &m_VerifySpaceRestrictors);
					ImGui::Checkbox("Build artefact spawn positions", &((CLevelPreferences*)EPrefs)->PIEArtSpawnPos);
					ImGui::EndPopup();
				}
			}

			if (ImGui::TableNextColumn())
			{
				ImGui::BeginDisabled(LTools->IsCompilerRunning() || LTools->IsGameRunning());
				IMGUI_HINT_BUTTON("ReloadCfg", m_tReloadConfigs, "Reload Configs", ClickReloadConfigs);
				IMGUI_HINT_BUTTON("BuildAndMake", m_tBuildAndMake, "Build and Make", ClickBuildAndMake);
				ImGui::EndDisabled();
			}

			if (ImGui::TableNextColumn())
			{
				ImGui::BeginDisabled(LTools->IsCompilerRunning() || LTools->IsGameRunning());
				IMGUI_HINT_BUTTON("PlayPC", m_tPlayPC, "Play level", ClickPlayPC);
				IMGUI_HINT_BUTTON("PlayLIG", m_tPlayCleanGame, "Play level in game", ClickPlayCleanGame);
				ImGui::EndDisabled();
			}

			if (ImGui::TableNextColumn())
			{
				IMGUI_HINT_BUTTON("OpenGamedata", m_tOpenGameData, "Open 'gamedata' folder", ClickOpenGameData);
			}

			if (ImGui::TableNextColumn())
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
				ImGui::SetNextItemWidth(150);
				ImGui::SliderFloat("Volume", &psSoundVEffects, 0, 1, "%.2f");
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(6);
	
}

void UITopBarForm::ClickUndo()
{
	ExecCommand(COMMAND_UNDO);
}

void UITopBarForm::ClickRedo()
{
	ExecCommand(COMMAND_REDO);
}

void UITopBarForm::ClickNew()
{
	ExecCommand(COMMAND_CLEAR);
}
void UITopBarForm::ClickOpen()
{
	ExecCommand(COMMAND_LOAD);
}
void UITopBarForm::ClickSave()
{
	ExecCommand(COMMAND_SAVE, xr_string(LTools->m_LastFileName.c_str()));
}
void UITopBarForm::ClickReloadConfigs()
{
	xr_delete(pSettings);
	string_path 			si_name;
	FS.update_path(si_name, "$game_config$", "system.ltx");
	pSettings = new CInifile(si_name, TRUE);// FALSE,TRUE,TRUE);
	xr_delete(pGameIni);
	string_path					fname;
	FS.update_path(fname, "$game_config$", "game.ltx");
	pGameIni = new CInifile(fname, TRUE);
	g_SEFactoryManager->reload();
	g_pGamePersistent->OnAppEnd();
	g_pGamePersistent->OnAppStart();
	Tools->UpdateProperties();
}

void UITopBarForm::ClickOpenGameData()
{
	string_path GameDataPath;
	FS.update_path(GameDataPath, "$game_data$", "");
	ShellExecuteA(NULL, "open", GameDataPath, NULL, NULL, SW_SHOWDEFAULT);
}
void UITopBarForm::ClickCForm()
{
	Scene->BuildCForm();

}
void UITopBarForm::ClickAIMap()
{
	Scene->BuildAIMap();

}
void UITopBarForm::ClickGGraph()
{
	Scene->BuildGameGraph();

}
void UITopBarForm::ClickPlayInEditor()
{
	Scene->Play();
}
void UITopBarForm::ClickBuildAndMake()
{
	if (Builder.Compile(false,false))
	{
		LTools->RunXrLC();
	}
}
void UITopBarForm::ClickTerminated()
{
	LTools->Terminated();
}
void UITopBarForm::ClickPlayPC()
{
	if (!Scene->BuildForPCPlay())
		return;

	LTools->RunGame("-editor_scene -start server(editor/single/alife/new) client(localhost) -nointro -noprefetch");
}
void UITopBarForm::ClickPlayCleanGame()
{
	LTools->RunGame("");
}
