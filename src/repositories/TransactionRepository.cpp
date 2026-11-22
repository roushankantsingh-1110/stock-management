#include "TransactionRepository.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

namespace {
StockTransaction mapRow(const QSqlQuery& q) {
    StockTransaction t;
    t.id = q.value("transaction_id").toInt();
    t.productId = q.value("product_id").toInt();
    t.productName = q.value("product_name").toString();
    t.transactionType = q.value("transaction_type").toString();
    t.quantity = q.value("quantity").toInt();
    t.transactionDate = q.value("transaction_date").toString();
    t.remarks = q.value("remarks").toString();
    return t;
}

const QString SELECT_BASE = R"(
    SELECT t.transaction_id, t.product_id, p.name AS product_name,
           t.transaction_type, t.quantity, t.transaction_date, t.remarks
    FROM stock_transactions t
    INNER JOIN products p ON t.product_id = p.product_id
)";
}

std::vector<StockTransaction> TransactionRepository::getAll(int limit) {
    std::vector<StockTransaction> list;
    QSqlQuery q(DatabaseManager::instance().database());
    QString sql = SELECT_BASE + " ORDER BY t.transaction_date DESC, t.transaction_id DESC";
    if (limit > 0) {
        sql += QString(" LIMIT %1").arg(limit);
    }
    if (q.exec(sql)) {
        while (q.next()) {
            list.push_back(mapRow(q));
        }
    } else {
        qWarning() << "TransactionRepository::getAll error:" << q.lastError().text();
    }
    return list;
}

std::vector<StockTransaction> TransactionRepository::getRecent(int count) {
    return getAll(count);
}

std::vector<StockTransaction> TransactionRepository::getByProduct(int productId) {
    return getFiltered(productId, "");
}

std::vector<StockTransaction> TransactionRepository::getFiltered(int productId, const QString& type,
                                                                 const QDate& startDate,
                                                                 const QDate& endDate) {
    std::vector<StockTransaction> list;
    QSqlQuery q(DatabaseManager::instance().database());

    QString sql = SELECT_BASE + " WHERE 1=1 ";
    QVariantList bindings;

    if (productId > 0) {
        sql += " AND t.product_id = ? ";
        bindings << productId;
    }
    if (!type.isEmpty() && type != "ALL") {
        sql += " AND t.transaction_type = ? ";
        bindings << type;
    }
    if (startDate.isValid()) {
        sql += " AND date(t.transaction_date) >= ? ";
        bindings << startDate.toString("yyyy-MM-dd");
    }
    if (endDate.isValid()) {
        sql += " AND date(t.transaction_date) <= ? ";
        bindings << endDate.toString("yyyy-MM-dd");
    }

    sql += " ORDER BY t.transaction_date DESC, t.transaction_id DESC";

    q.prepare(sql);
    for (const auto& b : bindings) {
        q.addBindValue(b);
    }

    if (q.exec()) {
        while (q.next()) {
            list.push_back(mapRow(q));
        }
    } else {
        qWarning() << "TransactionRepository::getFiltered error:" << q.lastError().text();
    }
    return list;
}

bool TransactionRepository::record(const StockTransaction& transaction, int* outId) {
    QSqlQuery q(DatabaseManager::instance().database());
    QString dateStr = transaction.transactionDate.isEmpty()
                          ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                          : transaction.transactionDate;

    q.prepare(R"(
        INSERT INTO stock_transactions (product_id, transaction_type, quantity, transaction_date, remarks)
        VALUES (?, ?, ?, ?, ?)
    )");
    q.addBindValue(transaction.productId);
    q.addBindValue(transaction.transactionType);
    q.addBindValue(transaction.quantity);
    q.addBindValue(dateStr);
    q.addBindValue(transaction.remarks.trimmed());

    if (q.exec()) {
        if (outId) {
            *outId = q.lastInsertId().toInt();
        }
        return true;
    }
    qWarning() << "TransactionRepository::record error:" << q.lastError().text();
    return false;
}
