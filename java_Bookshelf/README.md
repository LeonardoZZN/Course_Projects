# 📚 Bookshelf Keeper Program
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
The **Bookshelf Keeper Program** simulates a bookshelf system where books of varying positive heights are maintained in **non-decreasing order**.  
It is built around three main classes:  

- `Bookshelf` → represents the collection of books.  
- `BookshelfKeeper` → manages insertion and removal while maintaining order.  
- `BookshelfKeeperProg` → the main driver program for running simulations.  

The program ensures that books remain upright and properly ordered when picked or placed.  

---

## ✨ Features
- Create a `Bookshelf` with any number of books (positive heights only).  
- Automatically maintain books in **non-decreasing height order**.  
- Pick a book from the shelf while preserving order.  
- Insert a new book into the correct position.  
- Prevents books from “falling down” by enforcing ordering rules.  

---

## ⚙️ Installation
### Prerequisites
- [Java 8+](https://www.oracle.com/java/technologies/javase-downloads.html)

### Steps
```bash
# Clone the repository
git clone https://github.com/LeonardoZZN/Course_Projects.git

# Move into the project directory
cd ./java_Bookshelf

# Compile all Java files
javac *.java
```

---

## 🚀 Usage
Run the program:
```bash
java BookshelfKeeperProg
```
When you run the program, you can:
- Add books by specifying their heights.
- Remove books by index.
- Watch how the bookshelf automatically keeps books in order.

Example session:
```less
Initial Bookshelf: [1, 3, 4, 7]
Pick book at position 2
Bookshelf after: [1, 3, 7]

Put book of height 5
Bookshelf after: [1, 3, 5, 7]
```

---

## ⚙ Configuration
This program runs directly from the command line and does not require additional configuration.
If desired, you can modify the initial bookshelf contents by editing the code in `BookshelfKeeperProg.java`.

---

## 📂 Project Structure
```bash
.
├── Bookshelf.java            # Defines the Bookshelf data structure
├── BookshelfKeeper.java      # Manages ordered operations on the bookshelf
└── BookshelfKeeperProg.java  # Main driver program
```

---

## 🤝 Contributing
1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📜 License
Distributed under the MIT License. See LICENSE for more information.
