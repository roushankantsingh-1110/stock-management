#pragma once

#include "../models/StockTransaction.h"
#include <vector>
#include <optional>
#include <QString>
#include <QDate>

class TransactionRepository {
public:
    TransactionRepository() = default;

    std::vector<StockTransaction> getAll(int limit = 50);
    std::vector<StockTransaction> getRecent(int count = 10);
    std::vector<StockTransaction> getByProduct(int productId);
    std::vector<StockTransaction> getFiltered(int productId, const QString& type,
                                             const QDate& startDate = QDate(),
                                             const QDate& endDate = QDate());
    bool record(const StockTransaction& transaction, int* outId = nullptr);
};
