#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QTableWidget>
#include <QPushButton>
#include "../services/StockService.h"
#include "../services/ProductService.h"

class StockWidget : public QWidget {
    Q_OBJECT
public:
    explicit StockWidget(QWidget* parent = nullptr);

    void refresh();
    void selectTab(int index);

signals:
    void dataChanged();

private slots:
    void onStockInProductChanged(int index);
    void onStockOutProductChanged(int index);
    void onStockInSubmit();
    void onStockOutSubmit();
    void onHistoryFilterChanged();
    void onExportHistoryCsv();

private:
    void setupUI();
    void setupStockInTab(QWidget* tab);
    void setupStockOutTab(QWidget* tab);
    void setupHistoryTab(QWidget* tab);
    void reloadProductCombos();

    StockService m_stockService;
    ProductService m_productService;

    QTabWidget* m_tabWidget;

    // Stock In
    QComboBox* m_inProductCombo;
    QLabel* m_inCurrentStockLabel;
    QSpinBox* m_inQtySpin;
    QLineEdit* m_inRemarksEdit;
    QLabel* m_inStatusLabel;

    // Stock Out
    QComboBox* m_outProductCombo;
    QLabel* m_outCurrentStockLabel;
    QSpinBox* m_outQtySpin;
    QLineEdit* m_outRemarksEdit;
    QLabel* m_outStatusLabel;

    // History
    QComboBox* m_histProductCombo;
    QComboBox* m_histTypeCombo;
    QDateEdit* m_histStartDate;
    QDateEdit* m_histEndDate;
    QTableWidget* m_histTable;
    QLabel* m_histCountLabel;
};
