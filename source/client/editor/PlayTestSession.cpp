module;

#include <cstdint>
#include <optional>
export module Editor.PlayTestSession;

import Extern.Glm;

import Application.Window;
import MapEditor;
import ClientState;

import Shared.Core.IWorld;
import Shared.Core.Map.RuntimeMap;
import Shared.Core.Map.MapDocument;
import Shared.Core.State.StateManager;

export namespace Soldank
{
class PlayTestSession
{
public:
    bool IsActive() const { return is_active_; }

    void Start(ClientState& client_state, IWorld& world, Window& window, MapEditor& map_editor)
    {
        if (is_active_) {
            return;
        }

        editable_document_ = map_editor.SnapshotActiveDocumentForPlayTest();

        if (!client_state.client_soldier_id.has_value() ||
            !world.GetStateManager()->GetSoldier(*client_state.client_soldier_id).active) {
            const auto& soldier = world.CreateSoldier();
            client_state.client_soldier_id = soldier.id;
        }

        std::uint8_t client_soldier_id = *client_state.client_soldier_id;
        if (world.GetStateManager()->GetSoldier(client_soldier_id).dead_meat) {
            world.SpawnSoldier(client_soldier_id);
        }

        client_state.camera.view.ResetZoom();
        world.GetStateManager()->UnPauseGame();
        runtime_offset_ = BuildRuntimeMapAndMoveSoldiers(world, *editable_document_);
        world.GetStateManager()->GetMap().GenerateSectors();
        window.SetCursorMode(CursorMode::Locked);
        map_editor.Lock();
        is_active_ = true;
    }

    void Stop(ClientState& /*client_state*/, IWorld& world, Window& window, MapEditor& map_editor)
    {
        if (!is_active_) {
            return;
        }

        world.GetStateManager()->PauseGame();
        window.SetCursorMode(CursorMode::Normal);
        map_editor.RestoreActiveDocumentAfterPlayTest(*editable_document_, -*runtime_offset_);
        editable_document_.reset();
        runtime_offset_.reset();
        map_editor.Unlock();
        is_active_ = false;
    }

private:
    static glm::vec2 BuildRuntimeMapAndMoveSoldiers(IWorld& world, const MapDocument& document)
    {
        RuntimeMap runtime_map = RuntimeMap::BuildFromDocument(document);
        glm::vec2 move_offset = runtime_map.GetDocumentToRuntimeOffset();
        world.GetStateManager()->ApplyRuntimeMap(runtime_map);

        world.GetStateManager()->TransformSoldiers(
          [&](auto& soldier) { world.GetStateManager()->MoveSoldier(soldier.id, move_offset); });
        return move_offset;
    }

    bool is_active_ = false;
    std::optional<MapDocument> editable_document_;
    std::optional<glm::vec2> runtime_offset_;
};
} // namespace Soldank
