#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include "DashboardWidget.h"
#include "ProductsWidget.h"
#include "StockWidget.h"
#include "SuppliersWidget.h"
#include "CategoriesWidget.h"
#include "ReportsWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

public slots:
    void navigateToPage(int index);
    void refreshAll();

private slots:
    void onNavButtonClicked();
    void onBackupDatabase();
    void showAboutDialog();

private:
    void setupUI();
    void setupSidebar(QWidget* sidebar);
    void setupTopBar(QWidget* topBar);

    QStackedWidget* m_stackedWidget;
    QList<QPushButton*> m_navButtons;
    QLabel* m_pageTitleLabel;
    QLabel* m_dbStatusLabel;
    QLabel* m_clockLabel;

    DashboardWidget* m_dashboardWidget;
    ProductsWidget* m_productsWidget;
    StockWidget* m_stockWidget;
    SuppliersWidget* m_suppliersWidget;
    CategoriesWidget* m_categoriesWidget;
    ReportsWidget* m_reportsWidget;
};
