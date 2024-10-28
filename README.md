# Game Enhancement Tool (Fork of Lockdown Protocol External)

This project is a fork of the [Lockdown Protocol External](https://github.com/psZachary/lockdown-protocol-external) by [psZachary](https://github.com/psZachary). This fork extends the original functionality by adding enhanced item data management, overlay improvements, and various player enhancement features.

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

## Original Project
This project is a fork of [psZachary's Lockdown Protocol External](https://github.com/psZachary/lockdown-protocol-external), with additional enhancements focused on item properties and gameplay customization.

## Contributing
Feel free to submit issues, fork the repository, and make pull requests to enhance features, fix bugs, or improve performance.
