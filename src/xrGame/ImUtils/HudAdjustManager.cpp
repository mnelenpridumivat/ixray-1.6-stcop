#include "stdafx.h"
#include "../Level.h"
#include "../Actor.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"

#include "../xrEngine/XR_IOConsole.h"
#include "../xrEngine/string_table.h"

#include "ai_space.h"

#include "ImUtils.h"

void ImGui_Render2DWidget()
{
	static ImVec2 scrolling(0.0f, 0.0f);

	// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
//	canvas_p0.x += canvas_sz.x * 0.5f;
//	canvas_p0.y += canvas_sz.y * 0.5f;
	canvas_sz.x = (canvas_sz.x < canvas_sz.y ? canvas_sz.x : canvas_sz.y) * 0.5f;
	canvas_sz.y = canvas_sz.x;

	ImVec2 reg = ImGui::GetContentRegionAvail();

//	canvas_p0.x += reg.x - canvas_sz.x;
//	canvas_p0.y += reg.y - canvas_sz.y;

	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

	// Draw border and background color
	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	// This will catch our interactions
	ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held
	const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
	const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
}


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

					if (ImGui::BeginTable("Data##HUDP", 2))
					{
						ImGui::TableNextRow();

						ImGui::TableNextColumn();

						ImGui::SliderFloat("X##HUDP", &imgui_hud_adjust_manager.settings.hud_position.x, -1.0f, 1.0f);

						ImGui::SliderFloat("Y##HUDP", &imgui_hud_adjust_manager.settings.hud_position.y, -1.0f, 1.0f);

						ImGui::SliderFloat("Z##HUDP", &imgui_hud_adjust_manager.settings.hud_position.z, -1.0f, 1.0f);
						
						auto test = ImGui::GetContentRegionAvail();
						ImGui::TableNextColumn();
						ImGui_Render2DWidget();

						ImGui::EndTable();
					}






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