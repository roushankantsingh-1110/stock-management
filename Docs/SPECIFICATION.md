# StockPilot – Desktop Inventory Management System
## Technical Specification & Architecture Design Document

**Project:** StockPilot (v1.0)  
**Type:** Standalone C++ Desktop Application  
**Target Platform:** Windows 10/11 (x64)  
**Core Technologies:** C++17, Qt 6 Widgets, SQLite 3 (Qt SQL), CMake, Ninja  

---

## 1. Executive Summary & Objectives

StockPilot is an offline-first, professional desktop inventory management system designed for small-to-medium retail and wholesale businesses. It eliminates external server dependencies by leveraging a high-performance local SQLite relational database and native Qt 6 desktop widgets.

### Primary Goals:
1. **Zero Cloud Dependency:** Completely offline execution with self-contained persistent local storage (`stockpilot.db`).
2. **Strict Inventory Integrity:** Atomic database transactions ensuring quantity deductions/additions and transaction logs never drift or desynchronize.
3. **Ergonomic Business GUI:** Modern UI with clean sidebar navigation, high-contrast KPI cards, real-time searchable tables, low-stock visual alerts, and contextual dialogs.
4. **Academic & Industrial Code Quality:** Strict separation of concerns (Presentation, Business Service, Repository, and Data Access layers) with idiomatic C++17 and RAII.

---

## 2. Technology Stack & Build Pipeline

| Component | Specification | Description / Justification |
| :--- | :--- | :--- |
| **Language** | C++17 standard | Strongly typed OOP, RAII memory management, `<optional>`, `<chrono>`, `<filesystem>` |
| **UI Framework** | Qt 6 Widgets | Native high-DPI rendering, signal-slot architecture, stable widget lifecycle |
| **Database** | SQLite 3 (`QSQLITE`) | Embedded, zero-configuration ACID database with foreign key support |
| **Build System** | CMake 3.25+ | Cross-compiler standard configuration, target-based dependency linking |
| **Build Engine** | Ninja | Rapid incremental compilation |
| **Compiler** | MSVC (VS 2026 / 2022 x64) / MinGW GCC 13+ | Standard 64-bit native Windows code generation |
| **Styling** | Qt Style Sheets (QSS) | Polished light theme, custom modern scrollbars, tables, and buttons |

---

## 3. Relational Database Schema

### 3.1 ER Diagram

```
       SUPPLIERS (1)
            |
            | supplies
            v (N)
         PRODUCTS (1) <------- (N) STOCK_TRANSACTIONS
            ^
            | categorized by
            | (N)
       CATEGORIES (1)
```

### 3.2 Schema Details & Constraints

1. **`categories`**
   - `category_id`: INTEGER PRIMARY KEY AUTOINCREMENT
   - `category_name`: TEXT NOT NULL UNIQUE

2. **`suppliers`**
   - `supplier_id`: INTEGER PRIMARY KEY AUTOINCREMENT
   - `name`: TEXT NOT NULL
   - `phone`: TEXT
   - `email`: TEXT
   - `address`: TEXT

3. **`products`**
   - `product_id`: INTEGER PRIMARY KEY AUTOINCREMENT
   - `name`: TEXT NOT NULL
   - `description`: TEXT
   - `category_id`: INTEGER NOT NULL REFERENCES categories(category_id) ON DELETE RESTRICT
   - `supplier_id`: INTEGER REFERENCES suppliers(supplier_id) ON DELETE SET NULL
   - `purchase_price`: REAL NOT NULL CHECK (purchase_price >= 0)
   - `selling_price`: REAL NOT NULL CHECK (selling_price >= 0)
   - `quantity`: INTEGER NOT NULL DEFAULT 0 CHECK (quantity >= 0)
   - `minimum_stock`: INTEGER NOT NULL DEFAULT 5 CHECK (minimum_stock >= 0)
   - `created_at`: TEXT NOT NULL
   - `updated_at`: TEXT NOT NULL

4. **`stock_transactions`**
   - `transaction_id`: INTEGER PRIMARY KEY AUTOINCREMENT
   - `product_id`: INTEGER NOT NULL REFERENCES products(product_id) ON DELETE RESTRICT
   - `transaction_type`: TEXT NOT NULL CHECK (transaction_type IN ('IN', 'OUT'))
   - `quantity`: INTEGER NOT NULL CHECK (quantity > 0)
   - `transaction_date`: TEXT NOT NULL
   - `remarks`: TEXT

---

## 4. Module Specifications & Business Logic

### 4.1 Dashboard Module
- Dynamic metrics: Total products, physical stock units, inventory valuation, low stock items count.
- Recent movements table (latest 10 transactions).
- Restock alert banners highlighting critical products.

### 4.2 Product Management Module
- High-performance data table displaying all attributes.
- Live filtering by search query, category, supplier, and low-stock threshold.
- Safe CRUD dialogs with real-time field validation.

### 4.3 Stock Management Module
- Stock In / Stock Out forms with atomic transaction handling.
- Validation prevents Stock Out from depleting inventory below zero.
- Complete transaction log with multi-criteria filtering.

### 4.4 Supplier & Category Management
- Full lifecycle management for vendors and catalog categories.
- Referential integrity protection prevents dropping categories/suppliers that have associated inventory records.

### 4.5 Reports & Data Export Module
- Predefined reports: Valuation, Low Stock, Stock Flow, Supplier Portfolio.
- Standard CSV data export for external spreadsheet integration.
