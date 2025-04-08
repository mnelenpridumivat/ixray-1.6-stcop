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


					ImGui::SeparatorText("Position##HUD");

					if (ImGui::Button("Reset##HPosition"))
					{
						// todo: implement
					}

					ImGui::SliderFloat("X##HUDP", &imgui_hud_adjust_manager.settings.hud_position.x, -1.0f, 1.0f);

					ImGui::SliderFloat("Y##HUDP", &imgui_hud_adjust_manager.settings.hud_position.y, -1.0f, 1.0f);

					ImGui::SliderFloat("Z##HUDP", &imgui_hud_adjust_manager.settings.hud_position.z, -1.0f, 1.0f);





					ImGui::SeparatorText("Rotation##HUD");

					if (ImGui::Button("Reset##HRotation"))
					{
						// todo: implement
					}

					ImGui::SliderFloat("X##HUDR", &imgui_hud_adjust_manager.settings.hud_rotation.x, -1.0f, 1.0f);

					ImGui::SliderFloat("Y##HUDR", &imgui_hud_adjust_manager.settings.hud_rotation.y, -1.0f, 1.0f);

					ImGui::SliderFloat("Z##HUDR", &imgui_hud_adjust_manager.settings.hud_rotation.z, -1.0f, 1.0f);
				}

				if (ImGui::CollapsingHeader("Item"))
				{
					ImGui::SeparatorText("Position##Item");

					if (ImGui::Button("Item##IPosition"))
					{
						// todo: implement
					}

					ImGui::SeparatorText("Rotation##Item");

					if (ImGui::Button("Item##IRotation"))
					{
						// todo: implement
					}
				}

				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("Settings"))
			{
				int casted = imgui_hud_adjust_manager.settings.history_command_max_count;

				if (ImGui::Button("Save"))
				{
					// todo: implement
				}

				ImGui::SeparatorText("Params");

				if (ImGui::SliderInt("max history command count", &casted, 0, 1000))
				{
					imgui_hud_adjust_manager.settings.history_command_max_count = static_cast<decltype(imgui_hud_adjust_manager.settings.history_command_max_count)>(casted);
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	ImGui::PopStyleColor(1);
}