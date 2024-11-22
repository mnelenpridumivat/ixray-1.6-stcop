#include "stdafx.h"
#include "../Level.h"
#include "../Actor.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"

#include "../xrEngine/XR_IOConsole.h"
#include "../xrEngine/string_table.h"

#include "ai_space.h"

#include "ImUtils.h"

void RenderSearchManagerWindow()
{
	if (!Engine.External.EditorStates[static_cast<u8>(EditorUI::Game_SearchManager)])
		return;

	if (!g_pGameLevel)
		return;

	if (!ai().get_alife())
		return;

	if (g_pClsidManager == nullptr)
		return;

	if (imgui_search_manager.is_initialized == false)
		return;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, kGeneralAlphaLevelForImGuiWindows));
	if (ImGui::Begin("Search Manager"), &Engine.External.EditorStates[static_cast<u8>(EditorUI::Game_SearchManager)])
	{
		constexpr size_t kItemSize = sizeof(imgui_search_manager.combo_items) / sizeof(imgui_search_manager.combo_items[0]);
		ImGui::Combo("Category", &imgui_search_manager.selected_type, imgui_search_manager.combo_items, kItemSize);

		ImGui::SeparatorText("Stats");
		ImGui::Text("Current category: %s (%d)", imgui_search_manager.convertTypeToString(imgui_search_manager.selected_type), imgui_search_manager.selected_type);
		ImGui::Text("Level: %s", Level().name().c_str());

		ImGui::Text("All: %d", imgui_search_manager.counts[(eSelectedType::kSelectedType_All)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_SmartCover, imgui_search_manager.counts[(eSelectedType::kSelectedType_SmartCover)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_SmartTerrain, imgui_search_manager.counts[(eSelectedType::kSelectedType_SmartTerrain)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_Stalker, imgui_search_manager.counts[(eSelectedType::kSelectedType_Stalker)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_Car, imgui_search_manager.counts[(eSelectedType::kSelectedType_Car)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_LevelChanger, imgui_search_manager.counts[(eSelectedType::kSelectedType_LevelChanger)]);
		ImGui::Text("%s: %d", imgui_search_manager.pTranslatedLabel_Artefact, imgui_search_manager.counts[(eSelectedType::kSelectedType_Artefact)]);

		char colh_monsters[24]{};
		sprintf_s(colh_monsters, sizeof(colh_monsters), "Monsters: %d", imgui_search_manager.counts[eSelectedType::kSelectedType_Monster_All]);

		if (ImGui::CollapsingHeader(colh_monsters))
		{
			for (const auto& id : g_pClsidManager->get_monsters())
			{
				char monster_name[32]{};
				sprintf_s(monster_name, sizeof(monster_name), "%s: %d", g_pClsidManager->translateCLSID(id), imgui_search_manager.counts[imgui_search_manager.convertCLSIDToType(id)]);
				ImGui::Text(monster_name);
			}
		}

		char colh_weapons[24]{};
		sprintf_s(colh_weapons, sizeof(colh_weapons), "Weapons: %d", imgui_search_manager.counts[eSelectedType::kSelectedType_Weapon_All]);

		if (ImGui::CollapsingHeader(colh_weapons))
		{
			for (const auto& id : g_pClsidManager->get_weapons())
			{
				char weapon_name[32]{};
				sprintf_s(weapon_name, sizeof(weapon_name), "%s: %d", g_pClsidManager->translateCLSID(id), imgui_search_manager.counts[imgui_search_manager.convertCLSIDToType(id)]);
				ImGui::Text(weapon_name);
			}
		}

		ImGui::SeparatorText("Settings");
		ImGui::Checkbox("Alive", &imgui_search_manager.show_alive_creatures);
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Shows alive or not alive creature(if it is not creature this flag doesn't affect)");
			ImGui::EndTooltip();
		}

		ImGui::SeparatorText("Simulation");

		if (ImGui::BeginTabBar("##TB_InGameSearchManager"))
		{
			if (ImGui::BeginTabItem("Online##TB_Online_InGameSearchManager"))
			{
				memset(imgui_search_manager.counts, 0, sizeof(imgui_search_manager.counts));

				ImGui::InputText("##IT_InGameSeachManager", imgui_search_manager.search_string, sizeof(imgui_search_manager.search_string));

				char category_name_separator[64]{};
				const char* pTranslatedCategoryName = imgui_search_manager.convertTypeToString(imgui_search_manager.selected_type);
				size_t translate_str_len = strlen(pTranslatedCategoryName);
				memcpy_s(category_name_separator, sizeof(category_name_separator), pTranslatedCategoryName, translate_str_len);
				ImGui::SeparatorText(category_name_separator);

				auto size = Level().Objects.o_count();
				auto filter_string_size = strlen(imgui_search_manager.search_string);

				if (filter_string_size)
				{
					for (auto i = 0; i < size; ++i)
					{
						auto* pObject = Level().Objects.o_get_by_iterator(i);

						if (pObject && pObject->H_Parent() == nullptr)
						{
							// statistics for search manager must be refactored and counting when object adds or deletes from online/offline, later
						//	imgui_search_manager.count(pObject->CLS_ID);

							if (imgui_search_manager.valid(pObject->CLS_ID))
							{
								CGameObject* pCasted = smart_cast<CGameObject*>(pObject);
								bool passed_filter{ true };
								if (filter_string_size)
								{
									if (pCasted && pObject)
									{
										std::string_view cname = pObject->cName().c_str();
										std::string_view translate_name = Platform::ANSI_TO_UTF8(g_pStringTable->translate(pCasted->Name()).c_str()).c_str();

										if (cname.find(imgui_search_manager.search_string) == xr_string::npos && translate_name.find(imgui_search_manager.search_string) == xr_string::npos)
										{
											passed_filter = false;
										}
									}
								}

								if (pCasted)
								{
									if (imgui_search_manager.show_alive_creatures)
									{
										CEntity* pEntity = smart_cast<CEntity*>(pCasted);

										if (pEntity)
										{
											if (!pEntity->g_Alive())
											{
												passed_filter = false;
											}
										}
										else
										{
											passed_filter = false;
										}
									}
								}

								if (passed_filter)
								{
									xr_string name = pObject->cName().c_str();

									if (pCasted)
									{
										name += " ";
										name += "[";
										name += Platform::ANSI_TO_UTF8(g_pStringTable->translate(pCasted->Name()).c_str());
										name += "]";
									}
									name += "##InGame_SM_";
									name += std::to_string(i);

									if (ImGui::Button(name.c_str()))
									{
										CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

										if (pActor)
										{
											xr_string cmd;
											cmd = "set_actor_position ";
											cmd += cmd.ToString(pObject->Position().x);
											cmd += ",";
											cmd += cmd.ToString(pObject->Position().y);
											cmd += ",";
											cmd += cmd.ToString(pObject->Position().z);

											execute_console_command_deferred(Console, cmd.c_str());
										}
									}

									if (ImGui::BeginItemTooltip())
									{
										ImGui::Text("system name: [%s]", pObject->cName().c_str());
										ImGui::Text("section name: [%s]", pObject->cNameSect().c_str());
										ImGui::Text("translated name: [%s]", Platform::ANSI_TO_UTF8(g_pStringTable->translate(pCasted->Name()).c_str()).c_str());
										ImGui::Text("position: %f %f %f", pObject->Position().x, pObject->Position().y, pObject->Position().z);

										ImGui::EndTooltip();
									}
								}
							}
						}
					}
				}
				else
				{
					ImGuiListClipper clipper;
					clipper.Begin(size);

					while (clipper.Step())
					{
						int real_count = 0;
						int supposed_to_be_displayed = clipper.DisplayEnd - clipper.DisplayStart;

						for (size_t i = clipper.DisplayStart; i < size; ++i)
						{
							if (real_count >= supposed_to_be_displayed)
								break;

							auto* pObject = Level().Objects.o_get_by_iterator(i);

							if (pObject && pObject->H_Parent() == nullptr)
							{
								// statistics for search manager must be refactored and counting when object adds or deletes from online/offline, later
							//	imgui_search_manager.count(pObject->CLS_ID);

								if (imgui_search_manager.valid(pObject->CLS_ID))
								{
									CGameObject* pCasted = smart_cast<CGameObject*>(pObject);
									bool passed_filter = true;

									if (pCasted)
									{
										if (imgui_search_manager.show_alive_creatures)
										{
											CEntity* pEntity = smart_cast<CEntity*>(pCasted);

											if (pEntity)
											{
												if (!pEntity->g_Alive())
												{
													passed_filter = false;
												}
											}
											else
											{
												passed_filter = false;
											}
										}
									}

									if (passed_filter)
									{
										++real_count;
										xr_string name = pObject->cName().c_str();

										if (pCasted)
										{
											name += " ";
											name += "[";
											name += Platform::ANSI_TO_UTF8(g_pStringTable->translate(pCasted->Name()).c_str());
											name += "]";
										}
										name += "##InGame_SM_";
										name += std::to_string(i);

										if (ImGui::Button(name.c_str()))
										{
											CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

											if (pActor)
											{
												xr_string cmd;
												cmd = "set_actor_position ";
												cmd += cmd.ToString(pObject->Position().x);
												cmd += ",";
												cmd += cmd.ToString(pObject->Position().y);
												cmd += ",";
												cmd += cmd.ToString(pObject->Position().z);

												execute_console_command_deferred(Console, cmd.c_str());
											}
										}

										if (ImGui::BeginItemTooltip())
										{
											ImGui::Text("system name: [%s]", pObject->cName().c_str());
											ImGui::Text("section name: [%s]", pObject->cNameSect().c_str());
											ImGui::Text("translated name: [%s]", Platform::ANSI_TO_UTF8(g_pStringTable->translate(pCasted->Name()).c_str()).c_str());
											ImGui::Text("position: %f %f %f", pObject->Position().x, pObject->Position().y, pObject->Position().z);

											ImGui::EndTooltip();
										}
									}
								}
							}

						}
					}
				}


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Offline##TB_Offline_InGameSearchManager"))
			{
				memset(imgui_search_manager.counts, 0, sizeof(imgui_search_manager.counts));

				ImGui::InputText("##IT_InGameSearchManager", imgui_search_manager.search_string, sizeof(imgui_search_manager.search_string));

				char category_name_separator[64]{};
				const char* pTranslatedCategoryName = imgui_search_manager.convertTypeToString(imgui_search_manager.selected_type);
				size_t translate_str_len = strlen(pTranslatedCategoryName);
				memcpy_s(category_name_separator, sizeof(category_name_separator), pTranslatedCategoryName, translate_str_len);
				ImGui::SeparatorText(category_name_separator);

				const auto& objects = ai().alife().objects().objects_vec();
				size_t total_amount = objects.size();

				auto filter_string_size = strlen(imgui_search_manager.search_string);

				// todo: think about filtering for offline objects, because they can be REAL huge up to 32k...
				// filtering is slow because it is linear, possible variants for optimization: unordered_map for names and name_replace
				// possible suggestions: filter when button is pressed (but you need to remember the result and render only cache version (the result of filtering), not whole vector), create additional cache structures like filter by location and etc
				if (filter_string_size)
				{
					for (size_t i = 0; i < total_amount; ++i)
					{
						CSE_ALifeDynamicObject* pServerObject = objects[i];

						if (pServerObject)
						{
							if (pServerObject->ID_Parent == 0xffff)
							{
								if (imgui_search_manager.valid(pServerObject->m_tClassID))
								{
									bool passed_filter = true;

									CSE_Abstract* pAbstract = dynamic_cast<CSE_Abstract*>(pServerObject);

									if (pAbstract && pServerObject)
									{
										bool filter_by_cname = true;
										bool filter_by_s_name = true;
										if (pServerObject->name_replace())
										{
											std::string_view cname = pServerObject->name_replace();
											const xr_string& translated_by_cname = Platform::ANSI_TO_UTF8(g_pStringTable->translate(cname.data()).c_str());
											if (cname.find(imgui_search_manager.search_string) == std::string_view::npos && translated_by_cname.find(imgui_search_manager.search_string) == xr_string::npos)
											{
												filter_by_cname = false;
											}
										}
										else
										{
											filter_by_cname = false;
										}
										
										if (pAbstract->s_name.c_str())
										{
											std::string_view s_name = pAbstract->s_name.c_str();

											const xr_string& translated_by_s_name = Platform::ANSI_TO_UTF8(g_pStringTable->translate(s_name.data()).c_str());

											if (s_name.find(imgui_search_manager.search_string) == std::string_view::npos && translated_by_s_name.find(imgui_search_manager.search_string) == xr_string::npos)
											{
												filter_by_s_name = false;
											}
										}
										else
										{
											filter_by_s_name = false;
										}
										
										passed_filter = filter_by_cname || filter_by_s_name;
									}
									
									char button_name[128];
									sprintf_s(button_name, "%s [%s]", pServerObject->name_replace() ? pServerObject->name_replace() : "", Platform::ANSI_TO_UTF8(g_pStringTable->translate(pAbstract->s_name).c_str()).c_str());

									if (passed_filter)
									{
										if (ImGui::Button(button_name))
										{
											CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

											if (pActor)
											{
												xr_string cmd;
												cmd = "set_actor_position ";
												cmd += cmd.ToString(pServerObject->Position().x);
												cmd += ",";
												cmd += cmd.ToString(pServerObject->Position().y);
												cmd += ",";
												cmd += cmd.ToString(pServerObject->Position().z);

												execute_console_command_deferred(Console, cmd.c_str());
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					ImGuiListClipper clipper;
					clipper.Begin(total_amount);

					while (clipper.Step())
					{
						int real_count = 0;
						int supposed_to_be_displayed = clipper.DisplayEnd - clipper.DisplayStart;

						for (size_t i = clipper.DisplayStart; i < total_amount; ++i)
						{
							if (real_count >= supposed_to_be_displayed)
								break;

							auto* pServerObject = objects[i];

							if (pServerObject)
							{
								if (pServerObject->ID_Parent == 0xffff)
								{
									// statistics for search manager must be refactored and counting when object adds or deletes from online/offline, later
					//				imgui_search_manager.count(pServerObject->m_tClassID);

									if (imgui_search_manager.valid(pServerObject->m_tClassID))
									{
										xr_string name;

										name = pServerObject->name_replace() ? pServerObject->name_replace() : pServerObject->name();
										CSE_Abstract* pAbstract = smart_cast<CSE_Abstract*>(pServerObject);

										char button_name[128];
										sprintf_s(button_name, "%s [%s]", pServerObject->name_replace() ? pServerObject->name_replace() : "", Platform::ANSI_TO_UTF8(g_pStringTable->translate(pAbstract->s_name).c_str()).c_str());

										if (ImGui::Button(button_name))
										{
											CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

											if (pActor)
											{
												xr_string cmd;
												cmd = "set_actor_position ";
												cmd += cmd.ToString(pServerObject->Position().x);
												cmd += ",";
												cmd += cmd.ToString(pServerObject->Position().y);
												cmd += ",";
												cmd += cmd.ToString(pServerObject->Position().z);

												execute_console_command_deferred(Console, cmd.c_str());
											}
										}
										++real_count;
									}

								}
							}
						}
					}
				}


				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
	ImGui::PopStyleColor(1);
}

clsid_manager imgui_clsid_manager;

void InitImGuiCLSIDInGame()
{
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_bloodsucker);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_boar);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_dog);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_flesh);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_pseudodog);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_burer);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_cat);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_chimera);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_controller);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_izlom);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_poltergeist);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_pseudogigant);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_zombie);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_snork);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_tushkano);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_psydog);
	imgui_clsid_manager.add_monster(imgui_clsid_manager.monster_psydogphantom);

	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_binocular);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_knife);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_bm16);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_groza);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_svd);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_ak74);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_lr300);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_hpsa);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_pm);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_rg6);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_rpg7);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_shotgun);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_autoshotgun);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_svu);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_usp45);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_val);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_vintorez);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_walther);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_magazine);
	imgui_clsid_manager.add_weapon(imgui_clsid_manager.weapon_stationary_machine_gun);

	imgui_clsid_manager.add_item(imgui_clsid_manager.item_torch);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_detector_scientific);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_detector_elite);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_detector_advanced);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_detector_simple);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_d_pda);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_pda);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_medkit);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_bandage);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_antirad);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_food);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_bottle);
	imgui_clsid_manager.add_item(imgui_clsid_manager.item_ii_attch);

	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_base);
	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_vog25);
	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_og7b);
	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_m209);
	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_f1);
	imgui_clsid_manager.add_ammo(imgui_clsid_manager.ammo_rgd5);

	imgui_clsid_manager.add_outfit(imgui_clsid_manager.outfit);
	imgui_clsid_manager.add_outfit(imgui_clsid_manager.helmet);

	imgui_clsid_manager.add_addon(imgui_clsid_manager.addon_scope);
	imgui_clsid_manager.add_addon(imgui_clsid_manager.addon_silen);
	imgui_clsid_manager.add_addon(imgui_clsid_manager.addon_glaun);

	imgui_clsid_manager.add_artefact(imgui_clsid_manager.artefact);
	imgui_clsid_manager.add_artefact(imgui_clsid_manager.artefact_s);

	imgui_clsid_manager.add_vehicle(imgui_clsid_manager.car);

	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_helmet);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_out_exo);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_out_military);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_out_scientific);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_out_stalker);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_ak74);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_magazine_gl);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_binocular);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_bm16);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_fn2000);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_fort);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_groza);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_hpsa);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_knife);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_lr300);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_magazine);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_pm);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_rg6);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_rpg7);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_shotgun);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_svd);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_svu);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_usp45);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_val);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_vintorez);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_weapon_walther);

	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_ammo_base);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_ammo_og7b);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_ammo_m209);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_ammo_vog25);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_f1);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_rgd5);
	//imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_rpg7);

	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_mercury_ball);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_black_drops);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_needles);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_bast_artefact);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_gravi_black);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_dummy);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_zuda);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_thorn);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_faded_ball);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_electric_ball);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_rusty_hair);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_galantine);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_gravi);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_art_cta);

	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_addon_scope);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_addon_silen);
	imgui_clsid_manager.add_mp_stuff(imgui_clsid_manager.mp_addon_glaun);


	g_pClsidManager = &imgui_clsid_manager;
}


void InitImGuiSearchInGame()
{
	imgui_search_manager.init();
}
