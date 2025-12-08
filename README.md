# 🎓 SRMS - Student Record Management System

A high-performance, native desktop application for managing student records, built entirely in **C++** using the raw **Win32 API**.

8
> **Why this project is unique:** Unlike typical console-based student projects, this application features a full Graphical User Interface (GUI) built from scratch without heavy third-party frameworks like Qt or wxWidgets. It demonstrates manual memory management, custom UI painting (Owner Draw), and efficient file handling.

---

## ✨ Key Features

### 🖥️ Advanced User Interface
* **Native Win32 GUI:** Built using the low-level Windows API for maximum performance and minimal memory footprint.
* **Custom Theming:** Implements **Owner Draw** buttons and **Custom Draw** list views to override standard Windows grey styles with a modern, flat color palette.
* **Responsive Layout:** Window content resizes dynamically with the screen; buttons and lists adjust positions automatically.
* **Real-time Search:** Filter student records instantly by Name or Roll Number.
* **Column Sorting:** Click on any column header (ID, Name, CGPA, etc.) to sort data ascending or descending.

### 🛠️ Core Functionality
* **Secure Authentication:** Login system with simple password hashing to prevent plain-text storage.
* **CRUD Operations:** Complete Create, Read, Update, and Delete functionality for student records.
* **Data Persistence:** Records are saved to CSV files, ensuring data survives application restarts.
* **Import/Export:** Seamlessly import data from or export data to external `.csv` files for Excel compatibility.
* **Dashboard Analytics:** View real-time statistics (Total Students, Average CGPA, Top Performer).
* **Validation:** Robust input validation prevents duplicate Roll Numbers and invalid data entry.

---

## 📂 Project Structure

The project follows a modular, industry-standard directory structure to separate concerns:

```text
SRMS/
├── bin/            # Compiled Executables (Debug/Release)
├── data/           # Database files
│   ├── students.csv    # Main data storage
│   ├── users.cfg       # User credentials
│   └── activity.log    # System logs
├── gui/            # GUI Implementation (The "View")
│   ├── MainWindow.cpp      # Main dashboard & list logic
│   ├── LoginWindow.cpp     # Authentication logic
│   ├── AddStudentWindow.cpp # Input forms
│   └── resource.rc         # Windows resources (Icons, Menus)
├── include/        # Header files defining the architecture
│   ├── DataManager.h       # Data handling interface
│   ├── Student.h           # Student object definition
│   └── Validator.h         # Input validation logic
├── src/            # Core Business Logic (The "Model")
│   ├── DataManager.cpp     # File I/O, Sorting, Searching implementation
│   └── Student.cpp         # Student class implementation
└── SRMS.cbp        # Code::Blocks Project File
```
-------------------------------------------------------------------
🚀 How to Build & Run
-------------------------------------------------------------------
Prerequisites
OS: Microsoft Windows 7, 8, 10, or 11.

IDE: Code::Blocks (recommended) or Visual Studio.

Compiler: MinGW (GCC for Windows) or MSVC.

-------------------------------------------------------------------

Step-by-Step Guide
-------------------------------------------------------------------
Clone/Download this repository.

Open Code::Blocks.

Go to File -> Open -> Select SRMS.cbp.

Ensure the build target is set to Debug or Release.

Press F9 (Build and Run).
--------------------------------------------------------------------
🔑 Default Credentials
--------------------------------------------------------------------
Username: admin

Password: admin

(Note: Passwords can be changed in data/users.cfg)

---------------------------------------------------------------------
📊 Technical Highlights for Evaluators
---------------------------------------------------------------------
If you are grading this project, please note the following advanced C++ concepts used:

Win32 Subclassing & Custom Draw:

Used WM_DRAWITEM to manually paint buttons with custom colors (Blue, Green, Red) instead of using standard Windows buttons.

Used NM_CUSTOMDRAW to paint alternating row colors (stripes) in the ListView for better readability.

-----------------------------------------------------------------------
Memory Management:
-----------------------------------------------------------------------

Careful handling of GDI objects (Brushes, Pens) to prevent memory leaks (DeleteObject calls in WM_DESTROY).

Use of std::vector and smart pointers where appropriate for data safety.

Algorithmic Logic:

Sorting: Implemented a custom comparator for std::sort to handle multi-column sorting based on user clicks.

Search: Implemented case-insensitive substring search for filtering records.

------------------------------------------------------------------------
🔮 Future Improvements
------------------------------------------------------------------------

Database: Migrate from CSV to SQLite for handling thousands of records.

Client-Server Architecture:*Refactor the application to separate the GUI (Client) from the logic (Server) using **TCP/IP Sockets**, allowing multiple admins to manage records simultaneously.

Undo/Redo System: Implement the **Command Design Pattern** to allow users to undo accidental deletions or edits.
