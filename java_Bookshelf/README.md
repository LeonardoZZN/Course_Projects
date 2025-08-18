# 📚 Bookshelf Keeper Program

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
Clone this repository and compile the project:
```bash
git clone <your-repo-url>
cd <your-repo-name>
javac Bookshelf.java BookshelfKeeper.java BookshelfKeeperProg.java
