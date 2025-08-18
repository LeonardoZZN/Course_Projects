# 💣 Minesweeper (Java)

A simple Java implementation of the classic **Minesweeper** game.  
Recreate the nostalgic puzzle experience directly from your terminal or GUI, with customizable grid sizes and mine counts.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Java](https://img.shields.io/badge/java-8%2B-orange)

---

## 📚 Table of Contents
- [About](#-about)
- [Features](#-features)
- [Installation](#-installation)
- [Usage](#-usage)
- [Configuration](#-configuration)
- [Project Structure](#-project-structure)
- [Contributing](#-contributing)
- [License](#-license)

---

## 📖 About
This project reproduces the famous **Minesweeper** game entirely in Java.  
It includes:
- Logic to generate a random minefield
- Tracking of revealed and flagged tiles
- Win/loss detection
- Easy extensibility for different grid sizes and difficulty levels

---

## ✨ Features
- ✅ Classic Minesweeper rules
- ✅ Randomized mine placement each game
- ✅ Automatic tile revealing for empty cells
- ✅ Flagging suspected mines
- ✅ Win/lose detection logic

---

## 🛠 Installation

**Prerequisites**
- Java 8 or later

**Steps**
```bash
# Clone the repository
git clone https://github.com/LeonardoZZN/Course_Projects.git

# Move into the project directory
cd ./java_Minesweeper/src

# Compile all Java files
javac *.java
```

## 🚀 Usage
Run the game:
```bash
java MineSweeper
```

## ⚙ Configuration
You can modify difficulty or board size by editing parameters in MineSweeper.java:
```java
private static int SIDE_LENGTH = 9; // number of rows (columns) for a square game board
private static int NUM_MINES = 10;
```
## 📂 Project Structure
```bash
.
├── MineSweeper.java         # Main game class
├── MineSweeperFixed.java    # Main logic variant with fixed locations of mines
├── MineField.java           # Minefield generation and mine placement
├── VisibleField.java        # Player-visible board state
├── GameBoardPanel.java      # View and controller of the game (displays the board grids)
```

## 🤝 Contributing
1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

## 📜 License
Distributed under the MIT License. See LICENSE for more information.
