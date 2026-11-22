#pragma once
#include <QString>

struct StockTransaction {
    int id = 0;
    int productId = 0;
    QString productName;
    QString transactionType; // "IN" or "OUT"
    int quantity = 0;
    QString transactionDate;
    QString remarks;
};
