#include "ProductRepository.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

namespace {
Product mapRow(const QSqlQuery& q) {
    Product p;
    p.id = q.value("product_id").toInt();
    p.name = q.value("name").toString();
    p.description = q.value("description").toString();
    p.categoryId = q.value("category_id").toInt();
    p.categoryName = q.value("category_name").toString();
    p.supplierId = q.value("supplier_id").toInt();
    p.supplierName = q.value("supplier_name").toString();
    p.purchasePrice = q.value("purchase_price").toDouble();
    p.sellingPrice = q.value("selling_price").toDouble();
    p.quantity = q.value("quantity").toInt();
    p.minimumStock = q.value("minimum_stock").toInt();
    p.createdAt = q.value("created_at").toString();
    p.updatedAt = q.value("updated_at").toString();
    return p;
}

const QString SELECT_BASE = R"(
    SELECT p.product_id, p.name, p.description, p.category_id, c.category_name,
           p.supplier_id, COALESCE(s.name, 'N/A') AS supplier_name,
           p.purchase_price, p.selling_price, p.quantity, p.minimum_stock,
           p.created_at, p.updated_at
    FROM products p
    LEFT JOIN categories c ON p.category_id = c.category_id
    LEFT JOIN suppliers s ON p.supplier_id = s.supplier_id
)";
}

std::vector<Product> ProductRepository::getAll() {
    std::vector<Product> list;
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = SELECT_BASE + " ORDER BY p.name ASC";
    if (q.exec(sql)) {
        while (q.next()) {
            list.push_back(mapRow(q));
        }
    } else {
        qWarning() << "ProductRepository::getAll error:" << q.lastError().text();
    }
    return list;
}

std::optional<Product> ProductRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = SELECT_BASE + " WHERE p.product_id = ?";
    q.prepare(sql);
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        return mapRow(q);
    }
    return std::nullopt;
}

std::vector<Product> ProductRepository::getFiltered(const QString& search, int categoryId, int supplierId, bool lowStockOnly) {
    std::vector<Product> list;
    QSqlQuery q(DatabaseManager::instance().database());

    QString sql = SELECT_BASE + " WHERE 1=1 ";
    QVariantList bindings;

    if (!search.trimmed().isEmpty()) {
        sql += " AND (p.name LIKE ? OR p.description LIKE ?) ";
        QString pat = "%" + search.trimmed() + "%";
        bindings << pat << pat;
    }
    if (categoryId > 0) {
        sql += " AND p.category_id = ? ";
        bindings << categoryId;
    }
    if (supplierId > 0) {
        sql += " AND p.supplier_id = ? ";
        bindings << supplierId;
    }
    if (lowStockOnly) {
        sql += " AND p.quantity <= p.minimum_stock ";
    }
    sql += " ORDER BY p.name ASC";

    q.prepare(sql);
    for (const auto& b : bindings) {
        q.addBindValue(b);
    }

    if (q.exec()) {
        while (q.next()) {
            list.push_back(mapRow(q));
        }
    } else {
        qWarning() << "ProductRepository::getFiltered error:" << q.lastError().text();
    }
    return list;
}

std::vector<Product> ProductRepository::getLowStock() {
    return getFiltered("", 0, 0, true);
}

std::vector<Product> ProductRepository::getBySupplier(int supplierId) {
    return getFiltered("", 0, supplierId, false);
}

bool ProductRepository::add(const Product& product, int* outId) {
    QSqlQuery q(DatabaseManager::instance().database());
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    q.prepare(R"(
        INSERT INTO products (name, description, category_id, supplier_id,
                             purchase_price, selling_price, quantity, minimum_stock,
                             created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    q.addBindValue(product.name.trimmed());
    q.addBindValue(product.description.trimmed());
    q.addBindValue(product.categoryId);
    if (product.supplierId > 0) {
        q.addBindValue(product.supplierId);
    } else {
        q.addBindValue(QVariant());
    }
    q.addBindValue(product.purchasePrice);
    q.addBindValue(product.sellingPrice);
    q.addBindValue(product.quantity);
    q.addBindValue(product.minimumStock);
    q.addBindValue(now);
    q.addBindValue(now);

    if (q.exec()) {
        if (outId) {
            *outId = q.lastInsertId().toInt();
        }
        return true;
    }
    qWarning() << "ProductRepository::add error:" << q.lastError().text();
    return false;
}

bool ProductRepository::update(const Product& product) {
    QSqlQuery q(DatabaseManager::instance().database());
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    q.prepare(R"(
        UPDATE products
        SET name = ?, description = ?, category_id = ?, supplier_id = ?,
            purchase_price = ?, selling_price = ?, quantity = ?, minimum_stock = ?,
            updated_at = ?
        WHERE product_id = ?
    )");
    q.addBindValue(product.name.trimmed());
    q.addBindValue(product.description.trimmed());
    q.addBindValue(product.categoryId);
    if (product.supplierId > 0) {
        q.addBindValue(product.supplierId);
    } else {
        q.addBindValue(QVariant());
    }
    q.addBindValue(product.purchasePrice);
    q.addBindValue(product.sellingPrice);
    q.addBindValue(product.quantity);
    q.addBindValue(product.minimumStock);
    q.addBindValue(now);
    q.addBindValue(product.id);

    return q.exec();
}

bool ProductRepository::updateQuantity(int productId, int newQuantity) {
    QSqlQuery q(DatabaseManager::instance().database());
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    q.prepare("UPDATE products SET quantity = ?, updated_at = ? WHERE product_id = ?");
    q.addBindValue(newQuantity);
    q.addBindValue(now);
    q.addBindValue(productId);
    return q.exec();
}

bool ProductRepository::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM products WHERE product_id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool ProductRepository::hasTransactions(int productId) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT COUNT(*) FROM stock_transactions WHERE product_id = ?");
    q.addBindValue(productId);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

int ProductRepository::getTotalCount() {
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT COUNT(*) FROM products") && q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}

int ProductRepository::getTotalStockUnits() {
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT COALESCE(SUM(quantity), 0) FROM products") && q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}

double ProductRepository::getTotalInventoryValue() {
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT COALESCE(SUM(quantity * purchase_price), 0.0) FROM products") && q.next()) {
        return q.value(0).toDouble();
    }
    return 0.0;
}

int ProductRepository::getLowStockCount() {
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT COUNT(*) FROM products WHERE quantity <= minimum_stock") && q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}
