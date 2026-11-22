#pragma once

#include "../models/StockTransaction.h"
#include "../repositories/ProductRepository.h"
#include "../repositories/TransactionRepository.h"
#include <vector>
#include <QString>
#include <QDate>

class StockService {
public:
    StockService();

    bool stockIn(int productId, int quantity, const QString& remarks, QString& outError);
    bool stockOut(int productId, int quantity, const QString& remarks, QString& outError);

    std::vector<StockTransaction> getRecentTransactions(int count = 10);
    std::vector<StockTransaction> getFilteredTransactions(int productId, const QString& type,
                                                         const QDate& startDate = QDate(),
                                                         const QDate& endDate = QDate());

private:
    ProductRepository m_productRepo;
    TransactionRepository m_transRepo;
};
