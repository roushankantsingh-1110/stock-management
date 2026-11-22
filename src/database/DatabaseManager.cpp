#include "DatabaseManager.h"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QFile>
#include <QDebug>

const QString DatabaseManager::CONNECTION_NAME = "StockPilotConnection";

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager mgr;
    return mgr;
}

DatabaseManager::DatabaseManager() : m_connected(false) {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::initialize(const QString& customPath) {
    if (customPath.isEmpty()) {
        QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(appData + "/StockPilot");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        m_dbPath = dir.filePath("stockpilot.db");
    } else {
        m_dbPath = customPath;
    }

    QSqlDatabase db;
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        db = QSqlDatabase::database(CONNECTION_NAME);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    }

    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        qCritical() << "Failed to open SQLite database:" << db.lastError().text();
        m_connected = false;
        return false;
    }

    m_connected = true;

    // Enable foreign key constraints
    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON;");

    if (!createTables()) {
        qCritical() << "Failed to create database tables";
        return false;
    }

    seedSampleData();
    return true;
}

void DatabaseManager::close() {
    if (m_connected) {
        {
            QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
        m_connected = false;
    }
}

bool DatabaseManager::isConnected() const {
    return m_connected && QSqlDatabase::database(CONNECTION_NAME).isOpen();
}

QString DatabaseManager::databasePath() const {
    return m_dbPath;
}

QSqlDatabase DatabaseManager::database() const {
    return QSqlDatabase::database(CONNECTION_NAME);
}

bool DatabaseManager::beginTransaction() {
    return database().transaction();
}

bool DatabaseManager::commitTransaction() {
    return database().commit();
}

bool DatabaseManager::rollbackTransaction() {
    return database().rollback();
}

bool DatabaseManager::createTables() {
    QSqlDatabase db = database();
    QSqlQuery query(db);

    // 1. Categories
    QString createCategories = R"(
        CREATE TABLE IF NOT EXISTS categories (
            category_id INTEGER PRIMARY KEY AUTOINCREMENT,
            category_name TEXT NOT NULL UNIQUE
        );
    )";
    if (!query.exec(createCategories)) {
        qWarning() << "Error creating categories table:" << query.lastError().text();
        return false;
    }

    // 2. Suppliers
    QString createSuppliers = R"(
        CREATE TABLE IF NOT EXISTS suppliers (
            supplier_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            phone TEXT,
            email TEXT,
            address TEXT
        );
    )";
    if (!query.exec(createSuppliers)) {
        qWarning() << "Error creating suppliers table:" << query.lastError().text();
        return false;
    }

    // 3. Products
    QString createProducts = R"(
        CREATE TABLE IF NOT EXISTS products (
            product_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            category_id INTEGER NOT NULL REFERENCES categories(category_id) ON DELETE RESTRICT,
            supplier_id INTEGER REFERENCES suppliers(supplier_id) ON DELETE SET NULL,
            purchase_price REAL NOT NULL CHECK(purchase_price >= 0),
            selling_price REAL NOT NULL CHECK(selling_price >= 0),
            quantity INTEGER NOT NULL DEFAULT 0 CHECK(quantity >= 0),
            minimum_stock INTEGER NOT NULL DEFAULT 5 CHECK(minimum_stock >= 0),
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL
        );
    )";
    if (!query.exec(createProducts)) {
        qWarning() << "Error creating products table:" << query.lastError().text();
        return false;
    }

    // 4. Stock Transactions
    QString createTransactions = R"(
        CREATE TABLE IF NOT EXISTS stock_transactions (
            transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER NOT NULL REFERENCES products(product_id) ON DELETE RESTRICT,
            transaction_type TEXT NOT NULL CHECK(transaction_type IN ('IN', 'OUT')),
            quantity INTEGER NOT NULL CHECK(quantity > 0),
            transaction_date TEXT NOT NULL,
            remarks TEXT
        );
    )";
    if (!query.exec(createTransactions)) {
        qWarning() << "Error creating stock_transactions table:" << query.lastError().text();
        return false;
    }

    // Indexes for high performance searches
    query.exec("CREATE INDEX IF NOT EXISTS idx_products_name ON products(name);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_products_category ON products(category_id);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_transactions_product ON stock_transactions(product_id);");
    query.exec("CREATE INDEX IF NOT EXISTS idx_transactions_date ON stock_transactions(transaction_date);");

    return true;
}

bool DatabaseManager::seedSampleData() {
    QSqlDatabase db = database();
    QSqlQuery check(db);
    check.exec("SELECT COUNT(*) FROM categories");
    if (check.next() && check.value(0).toInt() > 0) {
        // Already seeded
        return true;
    }

    beginTransaction();

    QSqlQuery q(db);
    // Categories
    q.exec("INSERT INTO categories (category_name) VALUES ('Electronics');");
    q.exec("INSERT INTO categories (category_name) VALUES ('Accessories');");
    q.exec("INSERT INTO categories (category_name) VALUES ('Office Supplies');");
    q.exec("INSERT INTO categories (category_name) VALUES ('Networking');");

    // Suppliers
    q.exec("INSERT INTO suppliers (name, phone, email, address) VALUES "
           "('TechWorld Logistics', '+1-555-0192', 'contact@techworld.com', '104 Silicon Blvd, San Jose, CA');");
    q.exec("INSERT INTO suppliers (name, phone, email, address) VALUES "
           "('Global Hardware Depot', '+1-555-0144', 'sales@globalhardware.com', '52 Enterprise Rd, Dallas, TX');");
    q.exec("INSERT INTO suppliers (name, phone, email, address) VALUES "
           "('Prime Office Solutions', '+1-555-0178', 'orders@primeoffice.com', '88 Commercial Ave, Chicago, IL');");

    // Products
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    
    // Wireless Mouse
    q.prepare("INSERT INTO products (name, description, category_id, supplier_id, purchase_price, selling_price, quantity, minimum_stock, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue("Logitech M185 Wireless Mouse");
    q.addBindValue("2.4 GHz wireless optical mouse with USB nano receiver");
    q.addBindValue(2); // Accessories
    q.addBindValue(1); // TechWorld Logistics
    q.addBindValue(9.50);
    q.addBindValue(16.99);
    q.addBindValue(45);
    q.addBindValue(10);
    q.addBindValue(now);
    q.addBindValue(now);
    q.exec();

    // Mechanical Keyboard
    q.prepare("INSERT INTO products (name, description, category_id, supplier_id, purchase_price, selling_price, quantity, minimum_stock, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue("Keychron K2 Mechanical Keyboard");
    q.addBindValue("Compact 75% layout RGB mechanical keyboard with brown switches");
    q.addBindValue(1); // Electronics
    q.addBindValue(1); // TechWorld Logistics
    q.addBindValue(55.00);
    q.addBindValue(89.99);
    q.addBindValue(18);
    q.addBindValue(5);
    q.addBindValue(now);
    q.addBindValue(now);
    q.exec();

    // USB-C Hub
    q.prepare("INSERT INTO products (name, description, category_id, supplier_id, purchase_price, selling_price, quantity, minimum_stock, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue("Anker 7-in-1 USB-C Hub");
    q.addBindValue("4K HDMI, 100W Power Delivery, SD card reader, 3x USB 3.0");
    q.addBindValue(2); // Accessories
    q.addBindValue(2); // Global Hardware
    q.addBindValue(24.00);
    q.addBindValue(39.99);
    q.addBindValue(4); // Low stock!
    q.addBindValue(8);
    q.addBindValue(now);
    q.addBindValue(now);
    q.exec();

    // Cat6 Ethernet Cable 10m
    q.prepare("INSERT INTO products (name, description, category_id, supplier_id, purchase_price, selling_price, quantity, minimum_stock, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue("Cat6 High-Speed Ethernet Cable (10m)");
    q.addBindValue("Snagless RJ45 gigabit network patch cord");
    q.addBindValue(4); // Networking
    q.addBindValue(2); // Global Hardware
    q.addBindValue(4.20);
    q.addBindValue(8.99);
    q.addBindValue(32);
    q.addBindValue(10);
    q.addBindValue(now);
    q.addBindValue(now);
    q.exec();

    // Ergonomic Desk Pad
    q.prepare("INSERT INTO products (name, description, category_id, supplier_id, purchase_price, selling_price, quantity, minimum_stock, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue("Dual-Sided Leather Desk Mat");
    q.addBindValue("Waterproof PU leather desk blotter pad 90x45cm");
    q.addBindValue(3); // Office Supplies
    q.addBindValue(3); // Prime Office Solutions
    q.addBindValue(7.80);
    q.addBindValue(15.50);
    q.addBindValue(2); // Low stock!
    q.addBindValue(5);
    q.addBindValue(now);
    q.addBindValue(now);
    q.exec();

    // Initial Stock Transactions
    q.prepare("INSERT INTO stock_transactions (product_id, transaction_type, quantity, transaction_date, remarks) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(1);
    q.addBindValue("IN");
    q.addBindValue(50);
    q.addBindValue(now);
    q.addBindValue("Opening inventory shipment");
    q.exec();

    q.prepare("INSERT INTO stock_transactions (product_id, transaction_type, quantity, transaction_date, remarks) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(1);
    q.addBindValue("OUT");
    q.addBindValue(5);
    q.addBindValue(now);
    q.addBindValue("Customer order #1001 fulfillment");
    q.exec();

    q.prepare("INSERT INTO stock_transactions (product_id, transaction_type, quantity, transaction_date, remarks) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(3);
    q.addBindValue("IN");
    q.addBindValue(10);
    q.addBindValue(now);
    q.addBindValue("Initial stock receipt");
    q.exec();

    q.prepare("INSERT INTO stock_transactions (product_id, transaction_type, quantity, transaction_date, remarks) "
              "VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(3);
    q.addBindValue("OUT");
    q.addBindValue(6);
    q.addBindValue(now);
    q.addBindValue("Sold in retail store display");
    q.exec();

    commitTransaction();
    return true;
}

bool DatabaseManager::backupDatabase(const QString& destPath) {
    if (!isConnected()) return false;
    QFile currentDb(m_dbPath);
    if (QFile::exists(destPath)) {
        QFile::remove(destPath);
    }
    return currentDb.copy(destPath);
}
