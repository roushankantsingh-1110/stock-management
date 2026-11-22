#pragma once

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include "../services/ProductService.h"
#include "../services/StockService.h"

class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget* parent = nullptr);

    void refresh();

signals:
    void navigateToPage(int pageIndex);
    void requestStockIn();
    void requestStockOut();
    void requestAddProduct();

private:
    void setupUI();
    QWidget* createStatCard(const QString& title, QLabel** outValueLabel, const QString& iconText, const QString& accentColor);

    ProductService m_productService;
    StockService m_stockService;

    QLabel* m_totalProductsLabel;
    QLabel* m_totalStockUnitsLabel;
    QLabel* m_inventoryValueLabel;
    QLabel* m_lowStockCountLabel;
    QWidget* m_alertBanner;
    QLabel* m_alertText;
    QTableWidget* m_recentTable;
};
