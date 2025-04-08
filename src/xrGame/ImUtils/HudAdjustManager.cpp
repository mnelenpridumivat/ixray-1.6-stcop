#include "stdafx.h"
#include "../Level.h"
#include "../Actor.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"

#include "../xrEngine/XR_IOConsole.h"
#include "../xrEngine/string_table.h"

#include "ai_space.h"

#include "ImUtils.h"



void RenderHUDAdjustManager()
{
	if (!Engine.External.EditorStates[static_cast<u8>(EditorUI::Game_HudAdjustManager)])
		return;

	if (!g_pGameLevel)
		return;

	if (!ai().get_alife())
		return;

	if (imgui_hud_adjust_manager.is_initialized == false)
		return;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, kGeneralAlphaLevelForImGuiWindows));

	if (ImGui::Begin("Hud Adjust", &Engine.External.EditorStates[static_cast<u8>(EditorUI::Game_HudAdjustManager)]))
	{
		if (ImGui::BeginTabBar("Header"))
		{
			if (ImGui::BeginTabItem("General"))
			{
				if (ImGui::CollapsingHeader("Hud"))
				{
					ImGui::SeparatorText("Position");



					ImGui::SeparatorText("Rotation");
				}

				if (ImGui::CollapsingHeader("Item"))
				{
					ImGui::SeparatorText("Position");

					ImGui::SeparatorText("Rotation");
				}
			}


			if (ImGui::BeginTabItem("Settings"))
			{
				int casted = imgui_hud_adjust_manager.settings.history_command_max_count;

				if (ImGui::SliderInt("max history command count", &casted, 0, 1000))
				{
					imgui_hud_adjust_manager.settings.history_command_max_count = static_cast<decltype(imgui_hud_adjust_manager.settings.history_command_max_count)>(casted);
				}

				if (ImGui::Button("Save"))
				{
					// todo: implement
				}
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}