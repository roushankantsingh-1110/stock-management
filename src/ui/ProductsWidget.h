#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include "../services/ProductService.h"
#include "../repositories/CategoryRepository.h"
#include "../repositories/SupplierRepository.h"

class ProductsWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProductsWidget(QWidget* parent = nullptr);

    void refresh();
    void showAddDialog();

signals:
    void dataChanged();

private slots:
    void onSearchChanged();
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    void setupUI();
    void loadFilterCombos();
    void renderTable(const std::vector<Product>& products);

    ProductService m_productService;
    CategoryRepository m_categoryRepo;
    SupplierRepository m_supplierRepo;

    QLineEdit* m_searchEdit;
    QComboBox* m_categoryCombo;
    QComboBox* m_supplierCombo;
    QCheckBox* m_lowStockCheck;
    QLabel* m_resultCountLabel;
    QTableWidget* m_table;

    std::vector<Product> m_currentProducts;
};
