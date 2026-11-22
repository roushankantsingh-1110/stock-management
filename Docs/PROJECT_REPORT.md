# ACADEMIC PROJECT REPORT
# StockPilot – Desktop Inventory Management System

**Course:** Software Engineering & Object-Oriented System Design  
**Project Title:** StockPilot – Standalone Desktop Inventory Management System  
**Implementation Language:** C++17  
**GUI Framework:** Qt 6 Widgets (6.8.2)  
**Database System:** SQLite 3 (`QSQLITE`)  
**Build Automation:** CMake & Ninja  
**Operating System:** Windows 10/11 (x64)  
**Version:** 1.0.0  

---

## 1. Introduction

Inventory management is a critical business function that governs the acquisition, tracking, storage, and turnover of physical goods. For small-to-medium retail outlets, hardware suppliers, and wholesale distributors, maintaining an accurate count of available merchandise is essential for operational profitability and customer satisfaction.

**StockPilot** is a standalone, offline-first desktop application developed in modern C++17 utilizing Qt 6 Widgets and an embedded SQLite database engine. StockPilot delivers enterprise-grade stock tracking, vendor lifecycle management, reorder monitoring, and financial valuation without incurring recurring SaaS fees or requiring persistent network connectivity.

---

## 2. Problem Statement

Many small retail establishments and growing trade businesses still rely on manual record-keeping, paper logbooks, or disconnected spreadsheets. These legacy workflows present significant risks:
1. **Data Desynchronization:** Stock levels updated in spreadsheets are often not reconciled with purchase and dispatch records, leading to phantom inventory.
2. **Stockouts and Lost Revenue:** Lack of automated threshold alerts means managers discover shortages only when fulfilling customer orders.
3. **Audit Vulnerability:** Absence of immutable transaction ledgers makes tracking shrinkage or identifying billing discrepancies difficult.
4. **Cloud SaaS Burden:** Existing commercial solutions require monthly subscriptions, continuous broadband access, and third-party data hosting that small businesses often find prohibitive or insecure.

---

## 3. Project Objectives

1. **Self-Contained Local Execution:** Build an application that runs completely offline with local SQLite database storage.
2. **Strict Transaction Atomicity:** Guarantee that every inventory addition or reduction is accompanied by an audit log committed in an atomic database transaction.
3. **Overdraft Prevention:** Enforce validation rules that prevent stock deductions from reducing inventory below zero.
4. **Intuitive Graphical User Interface:** Provide a desktop UI adhering to corporate UX standards with clean sidebar navigation, real-time KPI stat cards, and status badges.
5. **Multi-Criteria Reporting:** Support instant reporting on inventory valuation, reorder candidates, and supplier distribution with one-click CSV export.

---

## 4. Existing System vs. Proposed System

| Dimension | Existing Systems (Spreadsheets / Paper) | Proposed System (StockPilot) |
|:---|:---|:---|
| **Data Integrity** | Prone to human overwrite and desynchronization | Enforced via SQL schema, foreign keys, and atomic transactions |
| **Search & Filtering** | Sluggish manual scrolling and basic text matching | Real-time indexed multi-column filtering and keyword search |
| **Stock Alerts** | Manual formula auditing | Instant visual badges (`In Stock`, `Low Stock`, `Out of Stock`) and banner alerts |
| **Security & Privacy** | Files exposed in open folders or shared drives | Encapsulated local relational database (`stockpilot.db`) with backup utility |
| **Usability** | Raw cells requiring spreadsheet expertise | Structured desktop forms, input guards, and unified navigation |

---

## 5. System Requirements

### 5.1 Hardware Requirements
- **Processor:** x86_64 compatible CPU (Intel Core i3 / AMD Ryzen 3 or higher)
- **Memory (RAM):** 2 GB minimum (4 GB recommended)
- **Storage:** 150 MB free disk space for application binaries, Qt runtime libraries, and database records
- **Display:** 1280x720 minimum screen resolution (High-DPI supported)

### 5.2 Software Requirements
- **Operating System:** Microsoft Windows 10 or Windows 11 (64-bit)
- **C++ Compiler:** MSVC 19.3x/19.5x (Visual Studio 2022/2026) or MinGW-W64 GCC 13+
- **GUI Framework:** Qt 6.8+ (MSVC 2022 64-bit kit)
- **Build Tools:** CMake 3.20+ and Ninja Build Engine

---

## 6. System Architecture

StockPilot employs a 4-tier modular architecture designed for high cohesion and loose coupling:

```
+-------------------------------------------------------------+
|                     PRESENTATION LAYER                      |
|  MainWindow, DashboardWidget, ProductsWidget, StockWidget,  |
|  SuppliersWidget, CategoriesWidget, ReportsWidget, Dialogs   |
+------------------------------+------------------------------+
                               | Signals / Slots / Method Calls
+------------------------------v------------------------------+
|                    BUSINESS LOGIC LAYER                     |
|  ProductService, SupplierService, StockService, ReportService|
|  (Input validation, atomic transactions, business rules)    |
+------------------------------+------------------------------+
                               | Direct Entity Requests
+------------------------------v------------------------------+
|                     DATA ACCESS LAYER                       |
|  ProductRepository, SupplierRepository, CategoryRepository, |
|  TransactionRepository                                      |
+------------------------------+------------------------------+
                               | Parameterized SQL Queries
+------------------------------v------------------------------+
|                     PERSISTENCE LAYER                       |
|  DatabaseManager (SQLite 3 via Qt QSQLITE Driver)            |
|  - Foreign key PRAGMA enforcement                           |
|  - Schema generation & initial seed data                   |
+-------------------------------------------------------------+
```

---

## 7. Database Design & Relational Schema

The persistent database is saved as `stockpilot.db` in `%APPDATA%\StockPilot\`. Foreign key enforcement is turned on explicitly on every connection via `PRAGMA foreign_keys = ON;`.

### 7.1 Schema Definitions

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

### 7.2 Entity-Relationship (ER) Diagram

```
       +-------------------+               +-------------------+
       |    SUPPLIERS      |               |    CATEGORIES     |
       +-------------------+               +-------------------+
       | PK supplier_id    |               | PK category_id    |
       |    name           |               |    category_name  |
       |    phone          |               +---------+---------+
       |    email          |                         |
       |    address        |                         | 1
       +---------+---------+                         |
                 | 1                                 |
                 | supplies                          | categorizes
                 |                                   |
                 | N                                 | N
       +---------v-----------------------------------v---------+
       |                       PRODUCTS                        |
       +-------------------------------------------------------+
       | PK  product_id                                        |
       |     name                                              |
       |     description                                       |
       | FK  category_id                                       |
       | FK  supplier_id                                       |
       |     purchase_price                                    |
       |     selling_price                                     |
       |     quantity                                          |
       |     minimum_stock                                     |
       |     created_at / updated_at                           |
       +---------------------------+---------------------------+
                                   | 1
                                   | tracks movements
                                   | N
       +---------------------------v---------------------------+
       |                  STOCK_TRANSACTIONS                   |
       +-------------------------------------------------------+
       | PK  transaction_id                                    |
       | FK  product_id                                        |
       |     transaction_type ('IN' / 'OUT')                   |
       |     quantity (> 0)                                    |
       |     transaction_date                                  |
       |     remarks                                           |
       +-------------------------------------------------------+
```

---

## 8. Detailed Module Implementations

### 8.1 Dashboard Module
- Displays 4 real-time KPI metrics calculated dynamically via SQLite aggregation queries:
  - Total Products count
  - Total physical units on hand
  - Total inventory valuation ($)
  - Number of items at or below reorder threshold
- Features an alert banner when low-stock items exist, offering a direct shortcut to the reorder view.
- Embeds a 10-row recent transaction log showing date, product name, type (colored badge), units moved, and notes.
- Quick action buttons ("+ Stock In", "- Stock Out", "+ New Product") trigger instantaneous navigation.

### 8.2 Product Management Module
- Structured table listing Product ID, Name, Category, Purchase Price, Selling Price, Available Qty, Min Stock, Supplier, and Status Badge (`In Stock`, `Low Stock`, `Out of Stock`).
- Multi-field live search filtering products as the user types.
- Category and Supplier dropdown filters.
- "Low Stock Only" filter checkbox.
- Full CRUD operations with `ProductDialog` enforcing validation (non-empty name, positive prices, valid category).
- Safe deletion validation: Products with active stock transactions are protected from accidental deletion to maintain audit integrity.

### 8.3 Stock Operations Module (Stock In / Stock Out)
- **Stock In:** Increases inventory count, logs incoming shipment with transaction date and remarks.
- **Stock Out:** Deducts inventory count, validates against available units, and rejects overdraft attempts.
- **Atomicity:** Updates to `products.quantity` and inserts into `stock_transactions` are wrapped inside `db.beginTransaction()` and `db.commitTransaction()`. If any step fails, `db.rollbackTransaction()` ensures zero data corruption.
- **Transaction Ledger:** Historical table filterable by product, movement type (`ALL`, `IN`, `OUT`), and date range, with one-click CSV export.

### 8.4 Supplier Directory Module
- Vendor directory maintaining company name, telephone, email address, and physical address.
- Email format validation via regular expressions.
- Relationship inspector allowing users to view all products supplied by a specific vendor.
- Deletion safety preventing removal of suppliers that have registered catalog products.

### 8.5 Category Management Module
- Dedicated catalog category management interface.
- Prevents duplicate category names.
- Displays the live count of products currently assigned to each category.
- Prohibits deletion of categories currently referenced by products.

### 8.6 Analytics & Reports Module
- Generates 4 preconfigured business reports:
  1. **Inventory Valuation Report:** Breakdown of asset cost vs. projected retail revenue per SKU.
  2. **Low-Stock Reorder Report:** Prioritized list of depleted products with recommended reorder unit quantities.
  3. **Stock Movement Journal:** Full ledger of shipments received and orders dispatched over a selected date range.
  4. **Supplier Product Catalog:** Inventory distribution grouped by vendor.
- Instant export to standard CSV for spreadsheet reporting in Microsoft Excel or Google Sheets.

---

## 9. Verification & Automated Test Results

An automated C++ verification suite (`tests/test_backend.cpp`) was implemented and executed against a temporary SQLite instance to validate all core components:

```
==========================================
Starting StockPilot Automated Verification
==========================================
[TEST 1] Testing DatabaseManager::initialize...
  -> PASS: Database initialized and connected.
[TEST 2] Testing CategoryRepository...
  -> Categories count: 4
  -> PASS: Category added and verified (ID: 5).
[TEST 3] Testing SupplierRepository...
  -> PASS: Supplier added and verified (ID: 4).
[TEST 4] Testing ProductService CRUD...
  -> PASS: Product created successfully. Total: 6
  -> PASS: Filter search returned correct item (ID: 6).
[TEST 5] Testing Stock In...
  -> PASS: Stock In increased inventory from 20 to 35 units.
[TEST 6] Testing Stock Out...
  -> PASS: Stock Out decreased inventory from 35 to 25 units.
[TEST 7] Testing Overdraft Prevention...
  -> PASS: Overdraft rejected correctly with message: "Insufficient inventory! Requested 999 units, but only 25 units are available."
[TEST 8] Testing Quantity Validation (<= 0)...
  -> PASS: Non-positive quantities properly rejected.
[TEST 9] Testing Transaction History...
  -> PASS: Found 2 transactions for product.
[TEST 10] Testing Safe Deletion Protections...
  -> PASS: Protected product from deletion due to existing transaction history.
[TEST 11] Testing ReportService & CSV Export...
  -> Valuation Summary: SKUs=6, Units=126, Value=$2788.5
  -> PASS: CSV export successful.
==========================================
ALL 11 AUTOMATED VERIFICATION TESTS PASSED!
==========================================
```

---

## 10. Packaging & Deployment

The application has been packaged as a standalone portable release using Qt's official `windeployqt` tool in `dist/StockPilot/`:

- **Main Executable:** `StockPilot.exe`
- **Core Qt DLLs:** `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Sql.dll`, `Qt6Network.dll`, `Qt6Svg.dll`
- **Database Driver:** `plugins/sqldrivers/qsqlite.dll`
- **Platform Plugin:** `plugins/platforms/qwindows.dll`
- **Configuration:** `qt.conf` directing plugin discovery to `./plugins`

This distribution can be moved to any compatible 64-bit Windows computer and launched immediately without installing Qt Creator or development SDKs.

---

## 11. Limitations & Future Scope

### Limitations in Version 1.0
- Single-user desktop mode without role-based access control (RBAC).
- Local SQLite database without multi-terminal network synchronization.
- Barcode/QR scanner hardware is not directly integrated (manual keyboard input supported).

### Future Enhancements
- Hardware barcode and QR scanner integration via serial/USB HID.
- Direct PDF report generation with company logo headers.
- Multi-location warehouse support (tracking stock across multiple physical branches).
- Local network database sharing via SQLite replication or PostgreSQL bridge.

---

## 12. Conclusion

The StockPilot project successfully demonstrates the design and development of an offline-first, professional desktop inventory management system in C++17 and Qt 6. By employing clean software architecture, strict database transactions, and comprehensive input validation, StockPilot provides a reliable and responsive tool for inventory control, fulfilling all academic and technical project requirements.
