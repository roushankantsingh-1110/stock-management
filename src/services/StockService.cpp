#include "StockService.h"
#include "../database/DatabaseManager.h"
#include <QDateTime>

StockService::StockService() = default;

bool StockService::stockIn(int productId, int quantity, const QString& remarks, QString& outError) {
    if (productId <= 0) {
        outError = "Invalid product selection.";
        return false;
    }
    if (quantity <= 0) {
        outError = "Stock In quantity must be strictly greater than zero.";
        return false;
    }

    auto optProd = m_productRepo.getById(productId);
    if (!optProd.has_value()) {
        outError = "Selected product was not found in database.";
        return false;
    }

    Product prod = optProd.value();
    int newQty = prod.quantity + quantity;

    // ATOMIC TRANSACTION: update quantity AND record transaction log
    auto& db = DatabaseManager::instance();
    if (!db.beginTransaction()) {
        outError = "Failed to initiate database transaction.";
        return false;
    }

    if (!m_productRepo.updateQuantity(productId, newQty)) {
        db.rollbackTransaction();
        outError = "Failed to update product stock quantity.";
        return false;
    }

    StockTransaction t;
    t.productId = productId;
    t.transactionType = "IN";
    t.quantity = quantity;
    t.transactionDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    t.remarks = remarks.trimmed().isEmpty() ? "Stock received" : remarks.trimmed();

    if (!m_transRepo.record(t)) {
        db.rollbackTransaction();
        outError = "Failed to log stock movement transaction.";
        return false;
    }

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        outError = "Failed to commit database transaction.";
        return false;
    }

    return true;
}

bool StockService::stockOut(int productId, int quantity, const QString& remarks, QString& outError) {
    if (productId <= 0) {
        outError = "Invalid product selection.";
        return false;
    }
    if (quantity <= 0) {
        outError = "Stock Out quantity must be strictly greater than zero.";
        return false;
    }

    auto optProd = m_productRepo.getById(productId);
    if (!optProd.has_value()) {
        outError = "Selected product was not found in database.";
        return false;
    }

    Product prod = optProd.value();
    if (quantity > prod.quantity) {
        outError = QString("Insufficient inventory! Requested %1 units, but only %2 units are available.")
                       .arg(quantity)
                       .arg(prod.quantity);
        return false;
    }

    int newQty = prod.quantity - quantity;

    // ATOMIC TRANSACTION: update quantity AND record transaction log
    auto& db = DatabaseManager::instance();
    if (!db.beginTransaction()) {
        outError = "Failed to initiate database transaction.";
        return false;
    }

    if (!m_productRepo.updateQuantity(productId, newQty)) {
        db.rollbackTransaction();
        outError = "Failed to update product stock quantity.";
        return false;
    }

    StockTransaction t;
    t.productId = productId;
    t.transactionType = "OUT";
    t.quantity = quantity;
    t.transactionDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    t.remarks = remarks.trimmed().isEmpty() ? "Stock dispatched" : remarks.trimmed();

    if (!m_transRepo.record(t)) {
        db.rollbackTransaction();
        outError = "Failed to log stock movement transaction.";
        return false;
    }

    if (!db.commitTransaction()) {
        db.rollbackTransaction();
        outError = "Failed to commit database transaction.";
        return false;
    }

    return true;
}

std::vector<StockTransaction> StockService::getRecentTransactions(int count) {
    return m_transRepo.getRecent(count);
}

std::vector<StockTransaction> StockService::getFilteredTransactions(int productId, const QString& type,
                                                                   const QDate& startDate,
                                                                   const QDate& endDate) {
    return m_transRepo.getFiltered(productId, type, startDate, endDate);
}
