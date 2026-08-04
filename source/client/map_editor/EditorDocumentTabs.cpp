module;

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

export module MapEditor.EditorDocumentTabs;

import MapEditor.EditorCommandHistory;

import Shared.Core.Map.MapDocument;
import Shared.Core.State.StateManager;

export namespace Soldank
{
struct EditorDocumentTabDisplay
{
    std::uint64_t id;
    std::string name;
    bool is_dirty;
};

class EditorDocumentTabs
{
public:
    void InitializeFromActiveMap(StateManager& game_state_manager)
    {
        InitializeFromActiveMap(game_state_manager, std::nullopt);
    }

    void InitializeFromActiveMap(StateManager& game_state_manager,
                                 std::optional<std::uint8_t> client_soldier_id)
    {
        tabs_.clear();
        active_tab_id_ = CreateTab(game_state_manager.CreateMapDocumentSnapshot(),
                                   game_state_manager.CreateSoldiersSnapshot(),
                                   client_soldier_id);
    }

    bool IsInitialized() const { return active_tab_id_.has_value(); }

    std::size_t GetTabCount() const { return tabs_.size(); }

    std::optional<std::uint64_t> GetActiveTabId() const { return active_tab_id_; }

    std::vector<EditorDocumentTabDisplay> GetTabDisplays() const
    {
        std::vector<EditorDocumentTabDisplay> displays;
        displays.reserve(tabs_.size());
        for (const auto& tab : tabs_) {
            displays.push_back(
              { tab.id, tab.document.GetMap().GetName().value_or("Untitled"), tab.is_dirty });
        }
        return displays;
    }

    std::uint64_t CreateEmptyAndSelect(StateManager& game_state_manager,
                                       std::optional<std::uint8_t>& client_soldier_id)
    {
        StoreActiveState(game_state_manager, client_soldier_id);

        MapDocument document;
        document.CreateEmptyMap();
        const std::uint64_t tab_id = CreateTab(
          std::move(document), game_state_manager.CreateEmptySoldiersSnapshot(), std::nullopt);
        ApplyTab(tab_id, game_state_manager, client_soldier_id);
        return tab_id;
    }

    std::uint64_t CreateEmptyAndSelect(StateManager& game_state_manager)
    {
        std::optional<std::uint8_t> client_soldier_id;
        return CreateEmptyAndSelect(game_state_manager, client_soldier_id);
    }

    bool Select(std::uint64_t tab_id,
                StateManager& game_state_manager,
                std::optional<std::uint8_t>& client_soldier_id)
    {
        if (!FindTab(tab_id) || active_tab_id_ == tab_id) {
            return active_tab_id_ == tab_id;
        }

        StoreActiveState(game_state_manager, client_soldier_id);
        ApplyTab(tab_id, game_state_manager, client_soldier_id);
        return true;
    }

    bool Select(std::uint64_t tab_id, StateManager& game_state_manager)
    {
        std::optional<std::uint8_t> client_soldier_id;
        return Select(tab_id, game_state_manager, client_soldier_id);
    }

    bool Reorder(std::uint64_t tab_id, std::size_t new_index)
    {
        const auto tab_iterator =
          std::find_if(tabs_.begin(), tabs_.end(), [tab_id](const EditorDocumentTab& tab) {
              return tab.id == tab_id;
          });
        if (tab_iterator == tabs_.end() || new_index >= tabs_.size()) {
            return false;
        }

        EditorDocumentTab tab = std::move(*tab_iterator);
        tabs_.erase(tab_iterator);
        tabs_.insert(tabs_.begin() + static_cast<std::ptrdiff_t>(new_index), std::move(tab));
        return true;
    }

    void StoreActiveMap(const StateManager& game_state_manager)
    {
        if (!active_tab_id_) {
            return;
        }

        FindTab(*active_tab_id_)->document = game_state_manager.CreateMapDocumentSnapshot();
    }

    void StoreActiveState(const StateManager& game_state_manager,
                          std::optional<std::uint8_t> client_soldier_id)
    {
        StoreActiveMap(game_state_manager);
        EditorDocumentTab* tab = FindTab(*active_tab_id_);
        tab->soldiers = game_state_manager.CreateSoldiersSnapshot();
        tab->client_soldier_id = client_soldier_id;
    }

    EditorCommandHistory& GetActiveCommandHistory()
    {
        return FindTab(*active_tab_id_)->command_history;
    }

    const EditorCommandHistory& GetActiveCommandHistory() const
    {
        return FindTab(*active_tab_id_)->command_history;
    }

    void SetActiveDirty(bool is_dirty) { FindTab(*active_tab_id_)->is_dirty = is_dirty; }

    bool IsActiveDirty() const { return FindTab(*active_tab_id_)->is_dirty; }

private:
    struct EditorDocumentTab
    {
        std::uint64_t id;
        MapDocument document;
        StateManager::SoldierStates soldiers;
        std::optional<std::uint8_t> client_soldier_id;
        EditorCommandHistory command_history;
        bool is_dirty = false;
    };

    std::uint64_t CreateTab(MapDocument document,
                            StateManager::SoldierStates soldiers,
                            std::optional<std::uint8_t> client_soldier_id)
    {
        const std::uint64_t tab_id = next_tab_id_++;
        tabs_.push_back({ tab_id,
                          std::move(document),
                          std::move(soldiers),
                          client_soldier_id,
                          EditorCommandHistory{},
                          false });
        return tab_id;
    }

    void ApplyTab(std::uint64_t tab_id,
                  StateManager& game_state_manager,
                  std::optional<std::uint8_t>& client_soldier_id)
    {
        const EditorDocumentTab* tab = FindTab(tab_id);
        game_state_manager.ApplyMapDocument(tab->document);
        game_state_manager.ApplySoldiersSnapshot(tab->soldiers);
        client_soldier_id = tab->client_soldier_id;
        active_tab_id_ = tab_id;
    }

    EditorDocumentTab* FindTab(std::uint64_t tab_id)
    {
        const auto tab_iterator =
          std::find_if(tabs_.begin(), tabs_.end(), [tab_id](const EditorDocumentTab& tab) {
              return tab.id == tab_id;
          });
        return tab_iterator == tabs_.end() ? nullptr : &*tab_iterator;
    }

    const EditorDocumentTab* FindTab(std::uint64_t tab_id) const
    {
        const auto tab_iterator =
          std::find_if(tabs_.begin(), tabs_.end(), [tab_id](const EditorDocumentTab& tab) {
              return tab.id == tab_id;
          });
        return tab_iterator == tabs_.end() ? nullptr : &*tab_iterator;
    }

    std::vector<EditorDocumentTab> tabs_;
    std::optional<std::uint64_t> active_tab_id_;
    std::uint64_t next_tab_id_ = 1;
};
} // namespace Soldank
