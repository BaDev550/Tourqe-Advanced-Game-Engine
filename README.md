# Tourqe Advanced Game Engine

**TAGE** is a modular, lightweight, and efficient game engine developed in C++ for Windows platforms. Designed with simplicity and flexibility in mind, TAGE provides a solid foundation for developing 3D games, 
making it an excellent choice for indie developers and hobbyists.

![Screenshot 2025-06-26 001631](https://github.com/user-attachments/assets/91a0f626-8426-45a2-9887-040654bb44dc)

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
   git clone https://github.com/BaDev550/TAGE.git
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

![Screenshot 2025-06-19 143531](https://github.com/user-attachments/assets/15cbd6df-31ed-4f6e-8f7c-1cf636e79acc)


This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Developed by [Baran Keklik](mailto:keklikbaran91@gmail.com)
