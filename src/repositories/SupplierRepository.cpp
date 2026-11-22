#include "SupplierRepository.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

std::vector<Supplier> SupplierRepository::getAll() {
    std::vector<Supplier> list;
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT supplier_id, name, phone, email, address FROM suppliers ORDER BY name ASC")) {
        while (q.next()) {
            Supplier s;
            s.id = q.value(0).toInt();
            s.name = q.value(1).toString();
            s.phone = q.value(2).toString();
            s.email = q.value(3).toString();
            s.address = q.value(4).toString();
            list.push_back(s);
        }
    } else {
        qWarning() << "SupplierRepository::getAll error:" << q.lastError().text();
    }
    return list;
}

std::optional<Supplier> SupplierRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT supplier_id, name, phone, email, address FROM suppliers WHERE supplier_id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        Supplier s;
        s.id = q.value(0).toInt();
        s.name = q.value(1).toString();
        s.phone = q.value(2).toString();
        s.email = q.value(3).toString();
        s.address = q.value(4).toString();
        return s;
    }
    return std::nullopt;
}

std::vector<Supplier> SupplierRepository::search(const QString& keyword) {
    std::vector<Supplier> list;
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT supplier_id, name, phone, email, address FROM suppliers "
              "WHERE name LIKE ? OR phone LIKE ? OR email LIKE ? ORDER BY name ASC");
    QString pattern = "%" + keyword.trimmed() + "%";
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    if (q.exec()) {
        while (q.next()) {
            Supplier s;
            s.id = q.value(0).toInt();
            s.name = q.value(1).toString();
            s.phone = q.value(2).toString();
            s.email = q.value(3).toString();
            s.address = q.value(4).toString();
            list.push_back(s);
        }
    }
    return list;
}

bool SupplierRepository::add(const Supplier& supplier, int* outId) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO suppliers (name, phone, email, address) VALUES (?, ?, ?, ?)");
    q.addBindValue(supplier.name.trimmed());
    q.addBindValue(supplier.phone.trimmed());
    q.addBindValue(supplier.email.trimmed());
    q.addBindValue(supplier.address.trimmed());
    if (q.exec()) {
        if (outId) {
            *outId = q.lastInsertId().toInt();
        }
        return true;
    }
    qWarning() << "SupplierRepository::add error:" << q.lastError().text();
    return false;
}

bool SupplierRepository::update(const Supplier& supplier) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE suppliers SET name = ?, phone = ?, email = ?, address = ? WHERE supplier_id = ?");
    q.addBindValue(supplier.name.trimmed());
    q.addBindValue(supplier.phone.trimmed());
    q.addBindValue(supplier.email.trimmed());
    q.addBindValue(supplier.address.trimmed());
    q.addBindValue(supplier.id);
    return q.exec();
}

bool SupplierRepository::remove(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM suppliers WHERE supplier_id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool SupplierRepository::hasProducts(int supplierId) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT COUNT(*) FROM products WHERE supplier_id = ?");
    q.addBindValue(supplierId);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}
