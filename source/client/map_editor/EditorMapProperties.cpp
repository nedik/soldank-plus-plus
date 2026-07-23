module;

#include <string>

export module MapEditor.EditorMapProperties;

import MapEditorState;
import MapEditor.EditorDocumentTabs;

import Shared.Core.State.StateManager;
import Shared.Core.Map.PMSEnums;
import Shared.Core.Map.PMSStructs;

export namespace Soldank
{
class EditorMapProperties
{
public:
    EditorMapProperties(MapEditorState& map_editor_state,
                        StateManager& game_state_manager,
                        EditorDocumentTabs& document_tabs)
    {
        const auto mark_changed = [&map_editor_state, &game_state_manager, &document_tabs]() {
            document_tabs.StoreActiveMap(game_state_manager);
            document_tabs.SetActiveDirty(true);
            map_editor_state.is_map_changed = true;
        };
        map_editor_state.event_save_map.AddObserver(
          [&map_editor_state, &game_state_manager, &document_tabs](const std::string& map_name) {
              game_state_manager.GetMap().SaveMap(map_name);
              document_tabs.StoreActiveMap(game_state_manager);
              document_tabs.SetActiveDirty(false);
              map_editor_state.is_map_changed = false;
          });
        map_editor_state.event_set_map_name.AddObserver(
          [&game_state_manager, mark_changed](const std::string& map_name) {
              game_state_manager.GetMap().SetName(map_name);
              mark_changed();
          });
        map_editor_state.event_set_map_description.AddObserver(
          [&game_state_manager, mark_changed](const std::string& description) {
              game_state_manager.GetMap().SetDescription(description);
              mark_changed();
          });
        map_editor_state.event_set_map_weather_type.AddObserver(
          [&game_state_manager, mark_changed](PMSWeatherType weather_type) {
              game_state_manager.GetMap().SetWeatherType(weather_type);
              mark_changed();
          });
        map_editor_state.event_set_map_step_type.AddObserver(
          [&game_state_manager, mark_changed](PMSStepType step_type) {
              game_state_manager.GetMap().SetStepType(step_type);
              mark_changed();
          });
        map_editor_state.event_set_map_grenades_count.AddObserver(
          [&game_state_manager, mark_changed](unsigned char grenades_count) {
              game_state_manager.GetMap().SetGrenadesCount(grenades_count);
              mark_changed();
          });
        map_editor_state.event_set_map_medikits_count.AddObserver(
          [&game_state_manager, mark_changed](unsigned char medikits_count) {
              game_state_manager.GetMap().SetMedikitsCount(medikits_count);
              mark_changed();
          });
        map_editor_state.event_set_map_jet_count.AddObserver(
          [&game_state_manager, mark_changed](int jet_count) {
              game_state_manager.GetMap().SetJetCount(jet_count);
              mark_changed();
          });
        map_editor_state.event_set_map_background_top_color.AddObserver(
          [&game_state_manager, mark_changed](const PMSColor& background_top_color) {
              game_state_manager.GetMap().SetBackgroundTopColor(background_top_color);
              mark_changed();
          });
        map_editor_state.event_set_map_background_bottom_color.AddObserver(
          [&game_state_manager, mark_changed](const PMSColor& background_bottom_color) {
              game_state_manager.GetMap().SetBackgroundBottomColor(background_bottom_color);
              mark_changed();
          });
        map_editor_state.event_set_map_texture_name.AddObserver(
          [&game_state_manager, mark_changed](const std::string& texture_name) {
              game_state_manager.GetMap().SetTextureName(texture_name);
              mark_changed();
          });
    }
};
} // namespace Soldank
