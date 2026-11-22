#pragma once

#include "../models/Product.h"
#include "../models/StockTransaction.h"
#include "../repositories/ProductRepository.h"
#include "../repositories/TransactionRepository.h"
#include "../repositories/SupplierRepository.h"
#include <vector>
#include <QString>
#include <QStringList>
#include <QDate>

struct InventorySummary {
    int totalProducts = 0;
    int totalUnits = 0;
    double totalValue = 0.0;
    int lowStockCount = 0;
};

class ReportService {
public:
    ReportService();

    InventorySummary getSummary();
    std::vector<Product> getInventoryValuationReport();
    std::vector<Product> getLowStockReport();
    std::vector<StockTransaction> getStockMovementReport(const QDate& from = QDate(), const QDate& to = QDate());
    std::vector<Product> getSupplierWiseReport(int supplierId);

    bool exportToCsv(const QString& filePath, const QStringList& headers,
                     const std::vector<QStringList>& rows, QString& outError);

private:
    ProductRepository m_productRepo;
    TransactionRepository m_transRepo;
    SupplierRepository m_supplierRepo;
};
