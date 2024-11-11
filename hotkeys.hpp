#include <atomic>
#include <stdio.h>

#include "imgui.h"

enum class HotkeyMode {
    Off,
    Hold,
    Toggle,
    HoldOff,
    Always
};

enum class InputType {
    None,
    VirtualKey,
};

struct Hotkey {
    std::atomic<InputType> type{ InputType::None };
    std::atomic<int> vKey{ 0 };
    std::atomic<HotkeyMode> mode{ HotkeyMode::Hold };
    std::atomic<bool> state{ false };
    std::atomic<bool> isPressed{ false };
};

enum class HotkeyIndex {
    MyCustomHotkey1, 
    MyCustomHotkey2, 
    MyCustomHotkey3,
    COUNT
};

class HotkeySystem {
private:
    friend class Settings;
    Hotkey hotkeys[static_cast<size_t>(HotkeyIndex::COUNT)];

    bool IsKeyPressed(const Hotkey& hotkey) {
        if (hotkey.type == InputType::VirtualKey) {
            return (GetAsyncKeyState(hotkey.vKey.load()) & 0x8000) && !(GetAsyncKeyState(hotkey.vKey.load()) & 0x4000);
        }
        return false;
    }

    bool IsKeyDown(const Hotkey& hotkey) {
        if (hotkey.type == InputType::VirtualKey) {
            return GetAsyncKeyState(hotkey.vKey.load()) & 0x8000;
        }
        return false;
    }

    // Get descriptive name for virtual key codes
    const char* GetKeyName(const Hotkey& hotkey) {
        static char keyName[256];
        if (hotkey.type == InputType::None) {
            return "None";
        }

        int vKey = hotkey.vKey.load();

        switch (vKey) {
            // Mouse buttons
        case VK_LBUTTON: return "Mouse 1";
        case VK_RBUTTON: return "Mouse 2";
        case VK_MBUTTON: return "Mouse 3";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";

            // Special keys
        case VK_BACK: "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_PAUSE: return "Pause";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Escape";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left";
        case VK_UP: return "Up";
        case VK_RIGHT: return "Right";
        case VK_DOWN: return "Down";
        case VK_SNAPSHOT: return "Print Screen";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";

            // Windows keys
        case VK_LWIN: "Left Win";
        case VK_RWIN: return "Right Win";
        case VK_APPS: return "Menu";

            // Numpad specific
        case VK_NUMPAD0: return "Num 0";
        case VK_NUMPAD1: return "Num 1";
        case VK_NUMPAD2: return "Num 2";
        case VK_NUMPAD3: return "Num 3";
        case VK_NUMPAD4: return "Num 4";
        case VK_NUMPAD5: return "Num 5";
        case VK_NUMPAD6: return "Num 6";
        case VK_NUMPAD7: return "Num 7";
        case VK_NUMPAD8: return "Num 8";
        case VK_NUMPAD9: return "Num 9";
        case VK_MULTIPLY: return "Num *";
        case VK_ADD: return "Num +";
        case VK_SEPARATOR: return "Separator";
        case VK_SUBTRACT: return "Num -";
        case VK_DECIMAL: return "Num .";
        case VK_DIVIDE: return "Num /";
        case VK_NUMLOCK: return "Num Lock";

            // Function keys
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";

            // Modifier
        case VK_LSHIFT: return "Left Shift";
        case VK_RSHIFT: return "Right Shift";
        case VK_LCONTROL: return "Left Ctrl";
        case VK_RCONTROL: return "Right Ctrl";
        case VK_LMENU: return "Left Alt";
        case VK_RMENU: return "Right Alt";

            // Media keys
        case VK_VOLUME_MUTE: return "Vol Mute";
        case VK_VOLUME_DOWN: return "Vol Down";
        case VK_VOLUME_UP: return "Vol Up";
        case VK_MEDIA_NEXT_TRACK: return "Next Track";
        case VK_MEDIA_PREV_TRACK: return "Prev Track";
        case VK_MEDIA_STOP: return "Media Stop";
        case VK_MEDIA_PLAY_PAUSE: return "Play/Pause";

        default:
            // For standard keys (letters, numbers, etc.)
            UINT scanCode = MapVirtualKey(vKey, MAPVK_VK_TO_VSC);
            if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName))) {
                return keyName;
            }

            // If we still don't have a name, return the hex value
            snprintf(keyName, sizeof(keyName), "Key 0x%X", vKey);
            return keyName;
        }
    }

    // Check if any key is pressed and return its virtual key code
    int GetPressedKey() {
        // Check mouse buttons first
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) return VK_LBUTTON;
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) return VK_RBUTTON;
        if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) return VK_MBUTTON;
        if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) return VK_XBUTTON1;
        if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) return VK_XBUTTON2;

        // Check all possible virtual key codes
        for (int i = 0; i < 256; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                return i;
            }
        }
        return 0;
    }

public:
    void Update() {
        for (auto& hotkey : hotkeys) {
            bool currentlyPressed = IsKeyDown(hotkey);
            bool wasPressed = hotkey.isPressed.exchange(currentlyPressed);

            switch (hotkey.mode.load()) {
            case HotkeyMode::Hold:
                hotkey.state.store(currentlyPressed);
                break;
            case HotkeyMode::Toggle:
                if (currentlyPressed && !wasPressed) {
                    hotkey.state.store(!hotkey.state.load());
                }
                break;
            case HotkeyMode::HoldOff:
                hotkey.state.store(!currentlyPressed);
                break;
            case HotkeyMode::Always:
                hotkey.state.store(true);
                break;
            default:
                hotkey.state.store(false);
                break;
            }
        }
    }

    int GetHotkeyVK(HotkeyIndex index) const {
        return hotkeys[static_cast<size_t>(index)].vKey;
    }

    bool IsActive(HotkeyIndex index) {
        return hotkeys[static_cast<size_t>(index)].state.load();
    }

    bool RenderHotkey(const char* label, HotkeyIndex index) {
        bool changed = false;
        Hotkey& hotkey = hotkeys[static_cast<size_t>(index)];
    
        ImGui::PushID(static_cast<int>(index));

        float buttonHeight = ImGui::GetFrameHeight();
        float textHeight = ImGui::GetTextLineHeight();
        float verticalOffset = (buttonHeight - textHeight) * 0.5f;

        ImGui::BeginGroup();
    
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

        if (hotkey.state.load()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        }

        ImGui::Text("%s", label);

        if (hotkey.state.load()) {
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - verticalOffset);

        if (ImGui::Button(GetKeyName(hotkey), ImVec2(100, buttonHeight))) {
            ImGui::OpenPopup("Change Hotkey");
        }

        ImGui::EndGroup();

        if (ImGui::BeginPopupContextItem("ModeMenu")) {
            const char* modeNames[] = { "Off", "Hold", "Toggle", "Hold Off", "Always"};
            HotkeyMode currentMode = hotkey.mode.load();
            for (int i = 0; i < IM_ARRAYSIZE(modeNames); i++) {
                if (ImGui::Selectable(modeNames[i], static_cast<int>(currentMode) == i)) {
                    hotkey.mode.store(static_cast<HotkeyMode>(i));
                    changed = true;
                }
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Change Hotkey", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Press any key or mouse button (Esc to clear)...");
        
            int pressedKey = GetPressedKey();
            if (pressedKey == VK_ESCAPE) {
                if (hotkey.type != InputType::None || hotkey.vKey != 0) {
                    hotkey.type = InputType::None;
                    hotkey.vKey = 0;
                    changed = true;
                }
                ImGui::CloseCurrentPopup();
            }
            else if (pressedKey != 0) {
                if (hotkey.type != InputType::VirtualKey || hotkey.vKey != pressedKey) {
                    hotkey.type = InputType::VirtualKey;
                    hotkey.vKey = pressedKey;
                    changed = true;
                }
                ImGui::CloseCurrentPopup();
            }
        
            ImGui::EndPopup();
        }

        ImGui::PopID();
        return changed;
    }
};