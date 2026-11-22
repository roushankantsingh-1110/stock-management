# StockPilot – Desktop Inventory Management System

**StockPilot** is a standalone, offline-first desktop Inventory Management System built with **C++17** and **Qt 6 Widgets**, backed by an embedded **SQLite 3** relational database engine.

Developed for small-to-medium retail and wholesale enterprises, it delivers real-time stock control, supplier directory management, multi-criteria reporting, and transaction audit trails—completely free from cloud dependencies or recurring hosting requirements.

---

## Key Features

- **Dynamic Executive Dashboard:** Real-time KPI summary cards displaying Total SKUs, Aggregate Physical Stock Units, Total Inventory Valuation, and Low-Stock threshold warnings.
- **Product Lifecycle Management (CRUD):** Add, edit, search, filter, and safely delete products with full category and supplier associations.
- **Atomic Stock Operations:** Dedicated **Stock In** (receiving) and **Stock Out** (dispatching) workflows with atomic database transactions (`BEGIN`, `COMMIT`, `ROLLBACK`) that guarantee inventory levels never drift from audit logs.
- **Overdraft Prevention:** Intelligent business logic that validates inventory balances and rejects stock deductions exceeding on-hand quantities.
- **Vendor & Supplier Management:** Maintain supplier contact books and view all items supplied by specific vendors.
- **Category Classification:** Organize catalog products by department/category with relational integrity constraints preventing deletion of in-use categories.
- **Analytical Reports & CSV Export:** Preconfigured reports for Inventory Valuation, Low-Stock Reordering, Stock Movement Journals, and Supplier Performance, exportable to standard `.csv` for spreadsheet accounting.
- **Persistent Local Database & Backups:** Automatic database initialization with initial seed data on first run, plus one-click full database backup to any directory.

---

## System Architecture

StockPilot adopts a 4-tier modular architecture adhering to OOP principles and Single Responsibility:

```
┌────────────────────────────────────────────────────────┐
│           Presentation Layer (Qt 6 Widgets)            │
│  MainWindow, DashboardWidget, ProductsWidget, etc.     │
└───────────────────────────┬────────────────────────────┘
                            │
┌───────────────────────────▼────────────────────────────┐
│            Business Logic Layer (Services)             │
│  ProductService, SupplierService, StockService, etc.   │
└───────────────────────────┬────────────────────────────┘
                            │
┌───────────────────────────▼────────────────────────────┐
│          Data Access Layer (Repositories)              │
│  ProductRepository, TransactionRepository, etc.        │
└───────────────────────────┬────────────────────────────┘
                            │
┌───────────────────────────▼────────────────────────────┐
│       Database Engine (DatabaseManager - SQLite 3)     │
│  stockpilot.db (with foreign keys and atomic txns)     │
└────────────────────────────────────────────────────────┘
```

---

## Technology Stack

| Component | Technology | Version / Specification |
|:---|:---|:---|
| Language | C++ | C++17 Standard |
| GUI Framework | Qt 6 Widgets | Qt 6.8.2 (MSVC 2022 x64) |
| Database Engine | SQLite 3 | Embedded via Qt SQL (`QSQLITE`) |
| Build System | CMake | 3.20+ with Ninja Generator |
| Compiler | MSVC (Visual Studio 2022/2026) / MinGW | x64 Native |
| Styling | Qt Style Sheets (QSS) | Custom responsive corporate theme |

---

## Directory Structure

```
inventory mgt/
├── CMakeLists.txt              # Primary CMake project configuration
├── README.md                   # System documentation & usage guide
├── Docs/
│   ├── SPECIFICATION.md        # Technical specification & architecture design
│   ├── ini.md                  # Project design requirements
│   └── gpt/ini.md              # Business Requirements Document (BRD)
├── docs/
│   └── PROJECT_REPORT.md       # Full academic project documentation & report
├── dist/
│   └── StockPilot/             # Standalone packaged release (Ready to run)
│       ├── StockPilot.exe      # Main compiled application executable
│       ├── qt.conf             # Qt plugin path configuration
│       ├── Qt6Core.dll         # Qt runtime libraries
│       ├── Qt6Gui.dll
│       ├── Qt6Widgets.dll
│       ├── Qt6Sql.dll
│       └── plugins/            # Platforms & sqldrivers (qsqlite.dll)
├── resources/
│   ├── resources.qrc           # Qt resource definition
│   └── styles/
│       └── style.qss           # Modern light theme stylesheet
├── src/
│   ├── main.cpp                # Application entry point & lifecycle
│   ├── database/               # Database connection & schema management
│   ├── models/                 # Plain data entities (Product, Supplier, etc.)
│   ├── repositories/           # Direct SQL query abstractions
│   ├── services/               # Business rules, transactions, validations
│   └── ui/                     # Qt Widgets, dialogs, and navigation
└── tests/
    └── test_backend.cpp        # 11-step automated verification suite
```

---

## Build & Installation Guide

### Prerequisites
- Windows 10/11 (64-bit)
- CMake 3.20+ and Ninja
- Visual Studio 2022/2026 C++ build tools (MSVC x64)
- Qt 6.8+ (MSVC 2022 64-bit) with `Core`, `Gui`, `Widgets`, `Sql` modules

### Building from Source

1. Open **x64 Native Tools Command Prompt for VS 2022/2026** (or run `vcvars64.bat`).
2. Navigate to the project root directory:
   ```cmd
   cd "D:\Project files\Personal\inventory mgt"
   ```
3. Configure CMake with Ninja:
   ```cmd
   cmake -B build -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.8.2/msvc2022_64" -DCMAKE_BUILD_TYPE=Release
   ```
4. Compile the application and test suite:
   ```cmd
   cmake --build build --config Release
   ```
5. Run the automated backend verification test:
   ```cmd
   build\TestBackend.exe
   ```

### Running the Packaged Executable

The standalone application is pre-packaged in the `dist\StockPilot\` directory:
```cmd
cd "D:\Project files\Personal\inventory mgt\dist\StockPilot"
StockPilot.exe
```
This release is portable and includes all required Qt runtime DLLs and the `qsqlite` driver plugin.

---

## Database Schema & Storage

The database is automatically created upon first launch and stored persistently at:
```
%APPDATA%\StockPilot\stockpilot.db
```

### Relational Tables
1. **`categories`**: `category_id` (PK), `category_name` (UNIQUE)
2. **`suppliers`**: `supplier_id` (PK), `name`, `phone`, `email`, `address`
3. **`products`**: `product_id` (PK), `name`, `description`, `category_id` (FK), `supplier_id` (FK), `purchase_price`, `selling_price`, `quantity`, `minimum_stock`, `created_at`, `updated_at`
4. **`stock_transactions`**: `transaction_id` (PK), `product_id` (FK), `transaction_type` (IN/OUT), `quantity`, `transaction_date`, `remarks`

Foreign key integrity is strictly enforced via `PRAGMA foreign_keys = ON;`.

---

## License & Academic Attribution
Developed as an academic desktop software engineering project demonstrating modern C++17 design patterns, Qt 6 GUI programming, and relational database systems.
