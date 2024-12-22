# Game Enhancement Tool

This project has enhanced item data management, overlay improvements, and various player enhancement features.

## Downloads
[v3.2](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47966)
~~[v3.1](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47870)~~
~~[v3.0](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47761)~~
~~[v2.9](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47725)~~
~~[v2.8](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47693)~~
~~[v2.7](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47597)~~
~~[v2.5](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47504)~~
~~[v2.3](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47384)~~
~~[v2.0](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47333)~~
~~[v1.0](https://www.unknowncheats.me/forum/downloads.php?do=file&id=47283)~~

## Features
- **ESP**
   - Player ESP (employee / dissident)
   - Weapon ESP (guns, knife, c4, etc.)
   - Primary Object ESP (vent filters, etc.)
   - Secondary Object ESP (screw driver, etc.)
   - Task ESP (deliveries, alimentations, vents, pressure) ONLY WORKS IF HOSTING!
   - Inventory ESP
   - Weapon Case ESP
- **ESP Details**
   - Distance between player and object/player
   - Item state (gun ammo, fuse color, etc.)
   - Task state (pressure bottle color, etc.)
- **Player Hacks**
   - Speed Hack
   - God Mode
   - Infinite Stamina
   - Self Revive (invisible)
- **Weapon Hacks**
   - Fast Melee
   - Infinite Melee Range
   - Auto Fire
   - Rapid Fire
   - No Recoil
   - Max Damage
   - Infinite Ammo
- **Inventory Hacks**
   - Change Hand Item
   - Change Hand Item State (battery charge, container color, etc.)
   - Change Bag Item
   - Change Bag Item State (battery charge, container color, etc.)
   - Toggle Items "can inventory" (rice, pizzushi, etc.)
- **Aimbot**
   - Target priority (head, neck, body)
   - Target filter (employee, dissident, all)
   - Dynamic FOV
   - Smoothing
- **Default Hotkeys:**
   - `F1` : Open/Close Menu
   - `F2` : Toggle ESP
   - `F3` : Toggle Speedhack
   - `F4` : Toggle God Mode
   - `F5` : Toggle Infinite Stamina
   - `F6` : Toggle Fast Melee
   - `F7` : Toggle Infinite Melee Range
   - `F8` : Toggle Auto Fire
   - `F9` : Toggle Rapid Fire
   - `Insert` : Toggle No Recoil
   - `Del` : Toggle Max Damage
   - `PageUp` : Toggle Aimbot
   - `PageDown` : Toggle Player List
   - `LAlt` : Aimbot Hold Key

## File Structure
- `main.cpp`: Core logic and main game loop with memory and overlay management.
- `ItemProperties.h`: Defines item properties structure and initializes game item data for quick lookups.
- `overlay/`: Contains code for rendering the in-game overlay.
- `util.hpp`: Utility functions for string manipulation, memory, and more.

## Setup and Usage
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/psZachary/lockdown-protocol-external.git
   cd lockdown-protocol-external
   ```

2. **Project Configuration**:
   - Ensure paths to "Header Files" are correctly set if using Visual Studio.
   - Add the project’s include directories to the IDE’s include path.

3. **Build and Run**:
   - Compile and build the project in your IDE.
   - Run the executable with the target game open.
   - You must have [medal.tv](https://medal.tv/) installed and running.

## Example Usage
- Toggle features like infinite ammo or god mode using function keys (e.g., `F2` for player ESP, `F8` for infinite stamina).

## Code Example
```cpp
// Accessing item properties
ItemProperties knifeProps = GetItemProperties("KNIFE");
std::cout << "KNIFE cast time: " << knifeProps.melee_cast_time << std::endl;
```

## Contributing
Feel free to submit issues, fork the repository, and make pull requests to enhance features, fix bugs, or improve performance.
