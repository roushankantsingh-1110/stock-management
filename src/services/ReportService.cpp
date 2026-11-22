#include "ReportService.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

ReportService::ReportService() = default;

InventorySummary ReportService::getSummary() {
    InventorySummary s;
    s.totalProducts = m_productRepo.getTotalCount();
    s.totalUnits = m_productRepo.getTotalStockUnits();
    s.totalValue = m_productRepo.getTotalInventoryValue();
    s.lowStockCount = m_productRepo.getLowStockCount();
    return s;
}

std::vector<Product> ReportService::getInventoryValuationReport() {
    return m_productRepo.getAll();
}

std::vector<Product> ReportService::getLowStockReport() {
    return m_productRepo.getLowStock();
}

std::vector<StockTransaction> ReportService::getStockMovementReport(const QDate& from, const QDate& to) {
    return m_transRepo.getFiltered(0, "", from, to);
}

std::vector<Product> ReportService::getSupplierWiseReport(int supplierId) {
    return m_productRepo.getBySupplier(supplierId);
}

namespace {
QString escapeCsvCell(const QString& cell) {
    QString res = cell;
    if (res.contains(',') || res.contains('"') || res.contains('\n') || res.contains('\r')) {
        res.replace("\"", "\"\"");
        res = "\"" + res + "\"";
    }
    return res;
}
}

bool ReportService::exportToCsv(const QString& filePath, const QStringList& headers,
                                const std::vector<QStringList>& rows, QString& outError) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        outError = "Could not open destination file for writing: " + file.errorString();
        return false;
    }

    QTextStream out(&file);

    // Write header
    QStringList escapedHeaders;
    for (const auto& h : headers) {
        escapedHeaders << escapeCsvCell(h);
    }
    out << escapedHeaders.join(",") << "\n";

    // Write rows
    for (const auto& row : rows) {
        QStringList escapedRow;
        for (const auto& col : row) {
            escapedRow << escapeCsvCell(col);
        }
        out << escapedRow.join(",") << "\n";
    }

    file.close();
    return true;
}
