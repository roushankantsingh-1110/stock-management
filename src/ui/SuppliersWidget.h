#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../services/SupplierService.h"

class SuppliersWidget : public QWidget {
    Q_OBJECT
public:
    explicit SuppliersWidget(QWidget* parent = nullptr);

    void refresh();

signals:
    void dataChanged();

private slots:
    void onSearchChanged();
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onViewProductsClicked();
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    void setupUI();
    void renderTable(const std::vector<Supplier>& suppliers);

    SupplierService m_supplierService;
    QLineEdit* m_searchEdit;
    QLabel* m_resultCountLabel;
    QTableWidget* m_table;

    std::vector<Supplier> m_currentSuppliers;
};
