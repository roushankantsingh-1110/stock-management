#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../repositories/CategoryRepository.h"
#include "../services/ProductService.h"

class CategoriesWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoriesWidget(QWidget* parent = nullptr);

    void refresh();

signals:
    void dataChanged();

private slots:
    void onAddClicked();
    void onRenameClicked();
    void onDeleteClicked();

private:
    void setupUI();
    void renderTable();

    CategoryRepository m_categoryRepo;
    ProductService m_productService;

    QLineEdit* m_nameEdit;
    QTableWidget* m_table;
    QLabel* m_statusLabel;
};
