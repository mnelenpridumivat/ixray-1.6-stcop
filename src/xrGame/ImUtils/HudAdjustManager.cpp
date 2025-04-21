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
	static ImVec2 circlePos(100.0f, 100.0f);
	static float circleRadius = 20.0f;
	const float squareSize = 200.0f;
	const float minRadius = 5.0f;
	const float maxRadius = 50.0f;
	const float gridStep = 8.0f;

	ImGui::BeginChild("SquareArea", ImVec2(squareSize, squareSize), true,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	{
		const ImVec2 squareMin = ImGui::GetWindowPos();
		const ImVec2 squareMax(squareMin.x + squareSize, squareMin.y + squareSize);
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImU32 gridColor = IM_COL32(100, 100, 100, 255); // Gray with 50% alpha
		for (float x = 0; x <= squareSize; x += gridStep) {
			ImVec2 start(squareMin.x + x, squareMin.y);
			ImVec2 end(squareMin.x + x, squareMax.y);
			drawList->AddLine(start, end, gridColor);
		}
		for (float y = 0; y <= squareSize; y += gridStep) {
			ImVec2 start(squareMin.x, squareMin.y + y);
			ImVec2 end(squareMax.x, squareMin.y + y);
			drawList->AddLine(start, end, gridColor);
		}

		// Draw square border
		drawList->AddRect(squareMin, squareMax, IM_COL32(255,255,255,100));
		drawList->AddRectFilled(squareMin, squareMax, IM_COL32(255,255,255,255));

		// Calculate circle position in screen space
		const ImVec2 circleCenter(squareMin.x + circlePos.x, squareMin.y + circlePos.y);

		// Create invisible button over the circle area
		ImGui::SetCursorScreenPos(ImVec2(circleCenter.x - circleRadius, circleCenter.y - circleRadius));
		ImGui::InvisibleButton("##CircleDrag", ImVec2(circleRadius * 2, circleRadius * 2));

		// Handle dragging only when clicking inside the circle
		if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
			circlePos.x = std::clamp(circlePos.x + mouseDelta.x,
				circleRadius, squareSize - circleRadius);
			circlePos.y = std::clamp(circlePos.y + mouseDelta.y,
				circleRadius, squareSize - circleRadius);
		}

		// Handle mouse wheel for radius adjustment
		if (ImGui::IsWindowHovered())
		{
			const float wheel = ImGui::GetIO().MouseWheel;
			if (wheel != 0.0f)
			{
				circleRadius = std::clamp(circleRadius + wheel * 2.0f, minRadius, maxRadius);
				circlePos.x = std::clamp(circlePos.x, circleRadius, squareSize - circleRadius);
				circlePos.y = std::clamp(circlePos.y, circleRadius, squareSize - circleRadius);
			}
		}

		// Draw the circle
		drawList->AddCircle(circleCenter, circleRadius, IM_COL32(255, 0, 0, 200));
	}
	ImGui::EndChild();
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