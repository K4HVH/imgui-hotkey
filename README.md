# imgui-hotkey
A flexible and user-friendly hotkey system for ImGui applications that supports multiple activation modes and both keyboard and mouse inputs. This system provides a clean, thread-safe way to handle keyboard and mouse inputs with various activation modes.

## Features
- Multiple hotkey activation modes:
  - Off: Hotkey is disabled
  - Hold: Active only while the key is held down
  - Toggle: Toggles between active/inactive states on key press
  - Hold Off: Inverse of Hold - active when key is not pressed
  - Always: Always active regardless of key state
- Support for all standard keyboard keys and mouse buttons (Mouse 1-5)
- Thread-safe implementation using atomic operations
- Seamless ImGui integration with context menus
- Clean modal popup for key binding
- Descriptive key names for all common inputs
- Support for special keys (Function keys, Media keys, etc.)

## Installation
1. Copy `hotkeys.hpp` to your project
2. Include the required headers:
```cpp
#include <atomic>
#include <stdio.h>
#include "imgui.h"
```

## Dependencies
- ImGui
- Windows.h (for virtual key handling)
- Standard C++ libraries (atomic, stdio.h)

## Quick Start

### 1. Define Your Hotkeys
First, customize the `HotkeyIndex` enum with your desired hotkeys:

```cpp
enum class HotkeyIndex {
    MyCustomHotkey1, 
    MyCustomHotkey2, 
    MyCustomHotkey3,
    COUNT  // Always keep this last
};
```

### 2. Create and Update the System
```cpp
class MyApplication {
private:
    HotkeySystem hotkeySystem;

public:
    void Update() {
        // Update hotkey states
        hotkeySystem.Update();
        
        // Check hotkey states
        if (hotkeySystem.IsActive(HotkeyIndex::MyCustomHotkey1)) {
            // Handle hotkey activation
        }
    }
};
```

### 3. Render the UI
```cpp
void RenderUI() {
    if (ImGui::Begin("Hotkeys")) {
        // Render hotkey controls
        hotkeySystem.RenderHotkey("Action 1", HotkeyIndex::MyCustomHotkey1);
        hotkeySystem.RenderHotkey("Action 2", HotkeyIndex::MyCustomHotkey2);
        hotkeySystem.RenderHotkey("Action 3", HotkeyIndex::MyCustomHotkey3);
        
        ImGui::End();
    }
}
```

## Usage Guide

### Checking Hotkey States
```cpp
// Check if a hotkey is currently active
if (hotkeySystem.IsActive(HotkeyIndex::MyCustomHotkey1)) {
    // Perform action
}

// Get the virtual key code for a specific hotkey
int vkCode = hotkeySystem.GetHotkeyVK(HotkeyIndex::MyCustomHotkey1);
```

### User Interface Features
The system provides several UI interaction methods:
1. Click the key button to bind a new key
2. Right-click the control to open a context menu for mode selection
3. Visual feedback with green text when the hotkey is active
4. Clear key binding by pressing Escape in the binding popup

### Supported Keys
- Mouse buttons (Mouse 1-5)
- All standard keyboard keys
- Function keys (F1-F12)
- Numpad keys
- Media keys
- Special keys (Ctrl, Alt, Shift, Win)
- System keys (Print Screen, Scroll Lock, etc.)

## Advanced Features

### Thread Safety
The system uses `std::atomic` for all state variables, making it safe to use in multi-threaded applications:
```cpp
struct Hotkey {
    std::atomic<InputType> type{ InputType::None };
    std::atomic<int> vKey{ 0 };
    std::atomic<HotkeyMode> mode{ HotkeyMode::Hold };
    std::atomic<bool> state{ false };
    std::atomic<bool> isPressed{ false };
};
```

### Custom UI Styling
```cpp
void CustomStyleExample() {
    // Style the hotkey button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    
    hotkeySystem.RenderHotkey("Styled Hotkey", HotkeyIndex::MyCustomHotkey1);
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}
```

### Complex State Management
```cpp
void ComplexExample() {
    // Combining multiple hotkey states
    bool condition1 = hotkeySystem.IsActive(HotkeyIndex::MyCustomHotkey1);
    bool condition2 = hotkeySystem.IsActive(HotkeyIndex::MyCustomHotkey2);
    
    if (condition1 && !condition2) {
        // Handle mutually exclusive actions
    }
}
```

## Example Implementation

```cpp
class GameController {
private:
    HotkeySystem hotkeys;
    
public:
    void Update() {
        hotkeys.Update();
        
        // Handle game actions based on hotkeys
        if (hotkeys.IsActive(HotkeyIndex::MyCustomHotkey1)) {
            PerformAction1();
        }
        
        if (hotkeys.IsActive(HotkeyIndex::MyCustomHotkey2)) {
            PerformAction2();
        }
    }
    
    void RenderSettings() {
        if (ImGui::Begin("Controls")) {
            ImGui::Text("Game Controls");
            ImGui::Separator();
            
            hotkeys.RenderHotkey("Primary Action", HotkeyIndex::MyCustomHotkey1);
            hotkeys.RenderHotkey("Secondary Action", HotkeyIndex::MyCustomHotkey2);
            hotkeys.RenderHotkey("Special Action", HotkeyIndex::MyCustomHotkey3);
            
            ImGui::End();
        }
    }
};
```

## License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes. All contributions will be licensed under GPL-3.0.

## Notes
- The system is currently Windows-specific due to the use of Virtual Key codes
- All key states are handled using Windows' GetAsyncKeyState function
- The system supports real-time key binding changes
- Thread-safe design allows for use in multi-threaded applications