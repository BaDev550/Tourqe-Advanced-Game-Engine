# Tourqe Advanced Game Engine

**TAGE** is a modular, lightweight, and efficient game engine developed in C++ for Windows platforms. Designed with simplicity and flexibility in mind, TAGE provides a solid foundation for developing 3D games.

<img width="1919" height="1031" alt="Screenshot 2025-09-12 194206" src="https://github.com/user-attachments/assets/40cd3f33-5434-4cdf-bcb3-f0227658bb9f" />

## Features

* **Cross-Platform Architecture**: While currently tailored for Windows, the engine's modular design facilitates potential cross-platform support in the future.
* **Build System**: Utilizes [Premake5](https://premake.github.io/) for generating project files, simplifying the build process.

## Getting Started

### Prerequisites

* **Operating System**: Windows 10 or later
* **Development Environment**: [Visual Studio 2019](https://visualstudio.microsoft.com/) or later
* **Build System**: [Premake5](https://premake.github.io/)

### Building the Engine

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/BaDev550/Tourqe-Advanced-Game-Engine.git
   cd TAGE
   ```

2. **Generate Project Files**:

   Run the provided batch file to generate Visual Studio project files:

   ```bash
   Build.bat
   ```

3. **Open the Solution**:

   Open `TAGE.sln` in Visual Studio.

4. **Build the Solution**:

   Build the solution in Visual Studio to compile the engine and sample projects.

## Directory Structure

* `Engine/TAGE/`: Core engine source code.
* `Engine/TARE/`: render engine engine source code.

* `Editor/`: Sample project demonstrating engine usage.

* `premake5.lua`: Premake5 configuration script.
* `Build.bat`: Batch script to generate project files.
* `TAGE.sln`: Visual Studio solution file.

## Licens

<img width="1913" height="1021" alt="image" src="https://github.com/user-attachments/assets/12079e8b-b99f-4bc5-8402-bcab5f025b13" />

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Developed by [Baran Keklik](mailto:keklikbaran91@gmail.com)
