# BUSINESS REQUIREMENTS DOCUMENT (BRD)
# StockPilot – Desktop Inventory Management System

**Project Type:** Standalone Desktop Application  
**Programming Language:** C++17  
**GUI Framework:** Qt 6 Widgets  
**Database:** SQLite  
**Build System:** CMake  
**Target Platform:** Windows 10/11  
**Development Scope:** Individual Academic Project  
**Application Mode:** Offline / Local Desktop Application  
**Version:** 1.0

---

# 1. PROJECT OVERVIEW

## 1.1 Project Description

StockPilot is a standalone desktop-based Inventory Management System developed using C++ and Qt 6.

The application is designed to help small businesses manage their inventory, maintain product records, track stock movements, manage supplier information and generate inventory reports.

The application must operate independently without requiring an internet connection, cloud services, external APIs or a continuously running database server.

All application data must be stored locally using an SQLite database.

The objective is to develop a functional, user-friendly desktop application that demonstrates practical implementation of:

- Object-Oriented Programming in C++.
- Database Management Systems.
- SQL queries and relational database design.
- GUI development.
- CRUD operations.
- Data validation and exception handling.
- Modular software architecture.

## 1.2 Project Objectives

1. Develop a fully functional standalone C++ desktop application.
2. Implement persistent local data storage using SQLite.
3. Provide an intuitive graphical user interface.
4. Enable complete product and supplier management.
5. Track stock additions and deductions.
6. Maintain a complete history of inventory transactions.
7. Identify products requiring restocking.
8. Generate useful inventory reports.
9. Package the application for execution on compatible Windows computers.

# 2. TECHNOLOGY STACK

Use the following technology stack:

| Component | Technology |
|---|---|
| Programming Language | C++17 |
| GUI Framework | Qt 6 Widgets |
| Database | SQLite 3 |
| Database Integration | Qt SQL |
| Build System | CMake |
| Compiler | MinGW or MSVC |
| IDE | VS Code / Qt Creator |
| Styling | Qt Style Sheets (QSS) |
| Version Control | Git |

### Technical Requirements

- Use Qt Widgets instead of developing a web-based interface.
- Use Qt SQL with QSQLITE for database integration.
- Use prepared SQL statements for all database operations.
- Use C++ classes and appropriate OOP principles.
- Use CMake for project configuration and compilation.
- Do not use a web server, REST API or cloud database.
- Do not introduce unnecessary third-party dependencies.
- Ensure compatibility with the selected Qt compiler kit.

# 3. FUNCTIONAL REQUIREMENTS

## 3.1 Dashboard Module

The dashboard must be the default screen displayed after launching the application.

It must display the following statistics:

1. Total Products – Count of all registered products.
2. Total Stock Units – Sum of available product quantities.
3. Total Inventory Value – Sum of product purchase cost multiplied by available quantity.
4. Low Stock Products – Number of products whose quantity is at or below their minimum stock threshold.
5. Recent Transactions – Display the latest inventory movements.

### Dashboard Requirements

- Statistics must be retrieved dynamically from SQLite.
- Dashboard values must refresh after relevant database changes.
- Low-stock products must be visually highlighted.
- Recent transactions must display product name, transaction type, quantity and date.
- Provide navigation shortcuts to frequently used modules.

## 3.2 Product Management Module

The application must support complete CRUD functionality.

### Add Product

The user must be able to enter:

- Product Name
- Category
- Description (optional)
- Purchase Price
- Selling Price
- Available Quantity
- Minimum Stock Threshold
- Supplier

### View Products

Display products in a structured table containing:

- Product ID
- Product Name
- Category
- Purchase Price
- Selling Price
- Available Quantity
- Supplier
- Stock Status

### Edit Product

Users must be able to modify existing product details.

### Delete Product

Users must be able to delete products after displaying a confirmation dialog.

Prevent deletion of products that have associated transaction history. Alternatively, implement a safe archival approach.

### Search and Filtering

Implement:

- Search by product name.
- Filter by category.
- Filter by supplier.
- Filter by low-stock status.
- Sort by product name, price and quantity.

## 3.3 Supplier Management Module

Allow users to maintain supplier information.

### Supplier Fields

- Supplier ID
- Supplier Name
- Contact Number
- Email Address
- Address

### Required Operations

- Add supplier.
- View all suppliers.
- Edit supplier information.
- Delete supplier if no dependent product records exist.
- Search suppliers by name.
- View products associated with a supplier.

## 3.4 Stock Management Module

This is one of the application's primary modules.

Implement two transaction types:

### A. Stock In

Allows users to increase product inventory.

Input fields:

- Product
- Quantity
- Transaction Date
- Remarks

On successful submission:

1. Increase the available product quantity.
2. Record the transaction in the database.
3. Refresh dashboard statistics.
4. Display a success notification.

### B. Stock Out

Allows users to decrease product inventory.

Input fields:

- Product
- Quantity
- Transaction Date
- Remarks

On successful submission:

1. Validate available stock.
2. Reject transactions exceeding available inventory.
3. Decrease the product quantity.
4. Record the transaction in the database.
5. Refresh dashboard statistics.

### Transaction History

Display:

- Transaction ID
- Product Name
- Transaction Type
- Quantity
- Transaction Date
- Remarks

Provide filters for:

- Product
- Transaction Type
- Date range

IMPORTANT: Stock quantity updates and corresponding transaction records must be committed atomically using SQLite transactions. If either operation fails, roll back both.

## 3.5 Category Management Module

Implement basic category management.

Users must be able to:

- Add categories.
- View categories.
- Rename categories.
- Delete categories that are not associated with products.

Prevent deletion of categories currently assigned to products.

## 3.6 Reports Module

Provide an inventory reporting interface.

Required reports:

1. Complete Inventory Report.
2. Low Stock Report.
3. Stock Movement Report.
4. Supplier-wise Product Report.
5. Inventory Valuation Report.

Reports must retrieve real data from SQLite.

Include basic date filtering where relevant.

PDF or CSV export may be implemented as an optional enhancement, provided it does not compromise core functionality.

# 4. DATABASE DESIGN

Database filename:

`stockpilot.db`

Store the database in a persistent application data directory, not in a temporary build folder.

Create the following relational tables.

## 4.1 Suppliers Table

Table: `suppliers`

| Column | Type | Constraint |
|---|---|---|
| supplier_id | INTEGER | PRIMARY KEY AUTOINCREMENT |
| name | TEXT | NOT NULL |
| phone | TEXT | |
| email | TEXT | |
| address | TEXT | |

## 4.2 Categories Table

Table: `categories`

| Column | Type | Constraint |
|---|---|---|
| category_id | INTEGER | PRIMARY KEY AUTOINCREMENT |
| category_name | TEXT | UNIQUE, NOT NULL |

## 4.3 Products Table

Table: `products`

| Column | Type | Constraint |
|---|---|---|
| product_id | INTEGER | PRIMARY KEY AUTOINCREMENT |
| name | TEXT | NOT NULL |
| description | TEXT | |
| category_id | INTEGER | FOREIGN KEY |
| supplier_id | INTEGER | FOREIGN KEY |
| purchase_price | REAL | NOT NULL |
| selling_price | REAL | NOT NULL |
| quantity | INTEGER | NOT NULL DEFAULT 0 |
| minimum_stock | INTEGER | NOT NULL DEFAULT 5 |
| created_at | TEXT | NOT NULL |
| updated_at | TEXT | NOT NULL |

## 4.4 Stock Transactions Table

Table: `stock_transactions`

| Column | Type | Constraint |
|---|---|---|
| transaction_id | INTEGER | PRIMARY KEY AUTOINCREMENT |
| product_id | INTEGER | FOREIGN KEY |
| transaction_type | TEXT | IN / OUT |
| quantity | INTEGER | NOT NULL |
| transaction_date | TEXT | NOT NULL |
| remarks | TEXT | |

### Database Rules

- Enable SQLite foreign key enforcement using `PRAGMA foreign_keys = ON`.
- Create tables automatically on the first application launch.
- Implement database initialization and connection management.
- Use parameterized queries.
- Use appropriate indexes for frequently searched fields.
- Preserve existing data between application restarts.
- Include sensible sample data for first-time demonstration, without duplicating seed data on subsequent launches.
- Handle database connection failures gracefully.
- Implement database backup functionality as an optional enhancement.

# 5. USER INTERFACE REQUIREMENTS

## 5.1 Design Direction

Develop a professional, modern desktop application.

The interface should resemble polished business inventory management software rather than a basic college demonstration project.

### Design Characteristics

- Clean and minimal layout.
- Light theme.
- White and light-grey content surfaces.
- Navy or deep-blue sidebar.
- Blue primary action buttons.
- Rounded cards and input fields.
- Consistent spacing and typography.
- Professional data tables.
- Clear status indicators.
- Appropriate icons.

Avoid excessive animations, unnecessary gradients, cluttered screens and oversized decorative elements.

## 5.2 Main Application Layout

The main window must contain:

**Left Sidebar**

- StockPilot Logo / Application Name
- Dashboard
- Products
- Suppliers
- Stock Management
- Categories
- Reports
- Settings / About

**Top Navigation Area**

- Current Page Title
- Search (where relevant)
- Database connection status
- Current date

**Main Content Area**

- Dynamic page content.
- Relevant tables, forms and statistics.
- Context-specific action buttons.

## 5.3 Screen Requirements

Develop the following screens:

1. Dashboard Screen.
2. Product Listing Screen.
3. Add/Edit Product Dialog.
4. Supplier Listing Screen.
5. Add/Edit Supplier Dialog.
6. Stock In/Stock Out Screen.
7. Transaction History Screen.
8. Category Management Screen.
9. Reports Screen.
10. About Application Dialog.

Use Qt layouts and reusable widgets. Avoid hardcoded absolute positioning.

Ensure the interface adapts properly when the application window is resized.

# 6. VALIDATION AND BUSINESS RULES

Implement the following validations.

### Product Validation

- Product name cannot be empty.
- Product price cannot be negative.
- Selling price must be greater than or equal to zero.
- Purchase price must be greater than or equal to zero.
- Quantity cannot be negative.
- Minimum stock cannot be negative.
- Category must be selected.
- Supplier may be optional.

### Supplier Validation

- Supplier name is mandatory.
- Email must have a reasonable valid format when provided.
- Contact number must be validated appropriately.

### Stock Validation

- Transaction quantity must be greater than zero.
- Stock Out cannot exceed available quantity.
- Invalid products cannot be selected.
- Stock transactions must be saved successfully before showing success notifications.
- Failed transactions must not modify product quantities.

### General Validation

- Confirm destructive operations.
- Display meaningful error messages.
- Prevent duplicate categories.
- Handle empty database states.
- Never silently ignore database errors.

# 7. SOFTWARE ARCHITECTURE

Use a modular, maintainable structure.

Recommended architecture:

**Presentation Layer**
- MainWindow
- Dashboard
- ProductsWidget
- SuppliersWidget
- StockWidget
- ReportsWidget

**Business Logic Layer**
- ProductService
- SupplierService
- StockService
- ReportService

**Data Access Layer**
- DatabaseManager
- ProductRepository
- SupplierRepository
- CategoryRepository
- TransactionRepository

**Model Layer**
- Product
- Supplier
- Category
- StockTransaction

Keep database queries out of UI classes wherever practical.

Use encapsulation and separation of responsibilities.

# 8. RECOMMENDED PROJECT STRUCTURE

Create the following structure:

StockPilot/
│
├── CMakeLists.txt
├── README.md
│
├── src/
│   ├── main.cpp
│   │
│   ├── database/
│   │   ├── DatabaseManager.h
│   │   └── DatabaseManager.cpp
│   │
│   ├── models/
│   │   ├── Product.h
│   │   ├── Supplier.h
│   │   ├── Category.h
│   │   └── StockTransaction.h
│   │
│   ├── repositories/
│   │   ├── ProductRepository.h
│   │   ├── ProductRepository.cpp
│   │   ├── SupplierRepository.h
│   │   ├── SupplierRepository.cpp
│   │   ├── CategoryRepository.h
│   │   ├── CategoryRepository.cpp
│   │   ├── TransactionRepository.h
│   │   └── TransactionRepository.cpp
│   │
│   ├── services/
│   │   ├── ProductService.h
│   │   ├── ProductService.cpp
│   │   ├── StockService.h
│   │   ├── StockService.cpp
│   │   └── ReportService.h
│   │
│   └── ui/
│       ├── MainWindow.h
│       ├── MainWindow.cpp
│       ├── DashboardWidget.h
│       ├── DashboardWidget.cpp
│       ├── ProductsWidget.h
│       ├── ProductsWidget.cpp
│       ├── SuppliersWidget.h
│       ├── SuppliersWidget.cpp
│       ├── StockWidget.h
│       ├── StockWidget.cpp
│       ├── ReportsWidget.h
│       └── ReportsWidget.cpp
│
├── resources/
│   ├── resources.qrc
│   ├── icons/
│   └── styles/
│       └── style.qss
│
├── database/
│   └── schema.sql
│
├── tests/
│
└── docs/

You may refine this structure if required, but preserve clear separation between UI, business logic and database operations.

# 9. NON-FUNCTIONAL REQUIREMENTS

### Performance
- Application should launch quickly.
- Tables should load efficiently.
- Database operations should not freeze the GUI.
- Use efficient SQL queries.

### Reliability
- All data must persist after closing and reopening the application.
- Handle invalid input and database errors.
- Use SQLite transactions for multi-step operations.

### Usability
- Navigation should be intuitive.
- Forms should have clear labels.
- Buttons should have meaningful names.
- Provide confirmation dialogs for destructive actions.
- Display appropriate success and error notifications.

### Maintainability
- Use modular C++ code.
- Follow consistent naming conventions.
- Avoid unnecessary code duplication.
- Add comments to important business logic.
- Keep code understandable for a university-level C++ student.

# 10. TESTING REQUIREMENTS

Test the following scenarios before declaring the application complete:

1. Application launches successfully.
2. Database is created automatically.
3. Product can be added.
4. Product can be edited.
5. Product can be deleted when permitted.
6. Supplier can be added and modified.
7. Category can be created.
8. Stock In increases available quantity.
9. Stock Out decreases available quantity.
10. Stock Out rejects insufficient inventory.
11. Transaction history is updated correctly.
12. Dashboard statistics update dynamically.
13. Low-stock products are identified correctly.
14. Search and filters work correctly.
15. Data persists after restarting the application.
16. Database errors are handled without crashing.
17. Invalid inputs are rejected.
18. Stock updates roll back if transaction recording fails.

# 11. BUILD AND DEPLOYMENT REQUIREMENTS

The final application must be deployable as a Windows desktop executable.

Requirements:

- Provide a working CMake configuration.
- Ensure the project builds successfully.
- Resolve all missing dependencies.
- Package the required Qt runtime libraries.
- Include the QSQLITE database driver.
- Ensure the SQLite database is stored in a persistent writable directory.
- Do not rely on the development environment's absolute paths.
- Test the packaged application independently of Qt Creator.

Provide:

1. StockPilot.exe
2. Required runtime dependencies.
3. Database file or automatic database initialization.
4. Complete source code.
5. Build instructions.
6. Execution instructions.

# 12. DOCUMENTATION AND ACADEMIC DELIVERABLES

Generate a professional README.md containing:

- Project introduction.
- Problem statement.
- Project objectives.
- Technology stack.
- System requirements.
- Installation instructions.
- Build instructions.
- Application usage instructions.
- Database structure.
- Application architecture.
- Features and limitations.

Also create a project report in `docs/PROJECT_REPORT.md` containing:

1. Introduction.
2. Problem Statement.
3. Objectives.
4. Existing System.
5. Proposed System.
6. Hardware and Software Requirements.
7. System Architecture.
8. Database Design.
9. ER Diagram.
10. Module Descriptions.
11. Implementation Details.
12. Testing and Results.
13. Screenshots.
14. Limitations.
15. Future Scope.
16. Conclusion.

The report must accurately describe the implemented application, not planned or nonexistent features.

# 13. IMPLEMENTATION INSTRUCTIONS FOR ANTIGRAVITY

Follow these instructions strictly.

### Development Approach

1. First inspect the existing workspace.
2. Check installed compilers, Qt versions and CMake availability.
3. Identify the correct Qt kit and compiler combination.
4. Create the project structure.
5. Configure CMake.
6. Implement database initialization.
7. Implement database repositories and models.
8. Implement business logic.
9. Build the GUI.
10. Connect all GUI components to their respective services.
11. Implement validation and error handling.
12. Test every major workflow.
13. Build the release executable.
14. Prepare documentation and screenshots.

### Critical Instructions

- Do not create a web application.
- Do not use HTML, CSS, JavaScript, React or Electron for the application.
- Do not use Python for the application backend.
- Do not replace SQLite with a remote database.
- Do not create non-functional buttons or placeholder screens.
- Do not use mock data as a substitute for database functionality.
- Do not hardcode dashboard statistics.
- Do not declare the application complete merely because it compiles.
- All core modules must perform actual database operations.
- Ensure every navigation item works.
- Ensure every form is connected to the backend logic.
- Use proper SQL foreign keys and validation.
- Keep the application simple enough for an individual student to understand and explain.

### Development Priorities

Priority 1: Working application and database integration.

Priority 2: Product, supplier and stock management.

Priority 3: Professional GUI.

Priority 4: Reports, documentation and packaging.

If any optional feature threatens the completion of the core application, prioritize the core application.

# 14. FINAL ACCEPTANCE CRITERIA

The project will be considered complete only when:

- A working standalone C++ desktop application exists.
- The application uses Qt 6 Widgets.
- SQLite is successfully integrated.
- All core CRUD operations work.
- Stock In and Stock Out operations work correctly.
- Transaction history is maintained.
- Dashboard values are dynamically calculated.
- Data persists between application sessions.
- Input validation works.
- No critical runtime errors remain.
- The Windows release build executes independently.
- Complete source code and documentation are available.

**FINAL OBJECTIVE:**

Deliver StockPilot as a polished, fully functional, offline-first C++ desktop inventory management application with persistent SQLite storage, a professional graphical interface, modular architecture and complete academic documentation.

The priority is a genuinely working and demonstrable application, not merely a visually attractive interface.
