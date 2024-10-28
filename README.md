# Game Enhancement Tool

## Features
- **Player Enhancements**: Toggle options for god mode, infinite ammo, infinite stamina, and more.
- **Overlay and ESP**: Visual overlay that displays information about players and items in the game.
- **Fast Melee and Infinite Melee Range**: Modify melee properties for faster actions or extended range.

## File Structure
- `main.cpp`: Core logic and main game loop with memory and overlay management.
- `ItemProperties.h`: Defines item properties structure and initializes game item data for quick lookups.
- `overlay/`: Contains code for rendering the in-game overlay.
- `util.hpp`: Utility functions for string manipulation, memory, and more.

## Setup and Usage
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/OniSensei/lockdown-protocol-external.git
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
