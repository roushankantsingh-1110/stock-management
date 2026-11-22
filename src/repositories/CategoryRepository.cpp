#include "CategoryRepository.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

std::vector<Category> CategoryRepository::getAll() {
    std::vector<Category> list;
    QSqlQuery q(DatabaseManager::instance().database());
    if (q.exec("SELECT category_id, category_name FROM categories ORDER BY category_name ASC")) {
        while (q.next()) {
            Category c;
            c.id = q.value(0).toInt();
            c.name = q.value(1).toString();
            list.push_back(c);
        }
    } else {
        qWarning() << "CategoryRepository::getAll error:" << q.lastError().text();
    }
    return list;
}

std::optional<Category> CategoryRepository::getById(int id) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT category_id, category_name FROM categories WHERE category_id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        Category c;
        c.id = q.value(0).toInt();
        c.name = q.value(1).toString();
        return c;
    }
    return std::nullopt;
}

std::optional<Category> CategoryRepository::getByName(const QString& name) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT category_id, category_name FROM categories WHERE LOWER(category_name) = LOWER(?)");
    q.addBindValue(name.trimmed());
    if (q.exec() && q.next()) {
        Category c;
        c.id = q.value(0).toInt();
        c.name = q.value(1).toString();
        return c;
    }
    return std::nullopt;
}

bool CategoryRepository::add(const Category& category, int* outId) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("INSERT INTO categories (category_name) VALUES (?)");
    q.addBindValue(category.name.trimmed());
    if (q.exec()) {
        if (outId) {
            *outId = q.lastInsertId().toInt();
        }
        return true;
    }
    qWarning() << "CategoryRepository::add error:" << q.lastError().text();
    return false;
}

bool CategoryRepository::update(const Category& category) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("UPDATE categories SET category_name = ? WHERE category_id = ?");
    q.addBindValue(category.name.trimmed());
    q.addBindValue(category.id);
    return q.exec();
}

bool CategoryRepository::remove(int id) {
    if (isUsedByProducts(id)) {
        return false;
    }
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("DELETE FROM categories WHERE category_id = ?");
    q.addBindValue(id);
    return q.exec();
}

bool CategoryRepository::isUsedByProducts(int categoryId) {
    QSqlQuery q(DatabaseManager::instance().database());
    q.prepare("SELECT COUNT(*) FROM products WHERE category_id = ?");
    q.addBindValue(categoryId);
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}
