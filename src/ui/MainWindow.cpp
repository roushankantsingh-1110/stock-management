#include "MainWindow.h"
#include "../database/DatabaseManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("StockPilot – Desktop Inventory Management System");
    resize(1200, 760);
    setMinimumSize(960, 640);
    setupUI();

    // Start live clock
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        m_clockLabel->setText(QDateTime::currentDateTime().toString("dddd, MMM d, yyyy  hh:mm:ss AP"));
    });
    timer->start(1000);

    // Initial page: Dashboard
    navigateToPage(0);
}

void MainWindow::setupUI() {
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Sidebar
    auto sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    setupSidebar(sidebar);
    rootLayout->addWidget(sidebar);

    // 2. Right Content Area (TopBar + StackedWidget)
    auto contentArea = new QWidget(this);
    auto contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    auto topBar = new QWidget(contentArea);
    topBar->setObjectName("topBar");
    setupTopBar(topBar);
    contentLayout->addWidget(topBar);

    m_stackedWidget = new QStackedWidget(contentArea);

    m_dashboardWidget = new DashboardWidget(m_stackedWidget);
    m_productsWidget = new ProductsWidget(m_stackedWidget);
    m_stockWidget = new StockWidget(m_stackedWidget);
    m_suppliersWidget = new SuppliersWidget(m_stackedWidget);
    m_categoriesWidget = new CategoriesWidget(m_stackedWidget);
    m_reportsWidget = new ReportsWidget(m_stackedWidget);

    m_stackedWidget->addWidget(m_dashboardWidget);
    m_stackedWidget->addWidget(m_productsWidget);
    m_stackedWidget->addWidget(m_stockWidget);
    m_stackedWidget->addWidget(m_suppliersWidget);
    m_stackedWidget->addWidget(m_categoriesWidget);
    m_stackedWidget->addWidget(m_reportsWidget);

    contentLayout->addWidget(m_stackedWidget);
    rootLayout->addWidget(contentArea);

    // Cross-module signal-slot wiring
    connect(m_dashboardWidget, &DashboardWidget::navigateToPage, this, &MainWindow::navigateToPage);
    connect(m_dashboardWidget, &DashboardWidget::requestStockIn, this, [this]() {
        navigateToPage(2);
        m_stockWidget->selectTab(0);
    });
    connect(m_dashboardWidget, &DashboardWidget::requestStockOut, this, [this]() {
        navigateToPage(2);
        m_stockWidget->selectTab(1);
    });
    connect(m_dashboardWidget, &DashboardWidget::requestAddProduct, this, [this]() {
        navigateToPage(1);
        m_productsWidget->showAddDialog();
    });

    connect(m_productsWidget, &ProductsWidget::dataChanged, this, &MainWindow::refreshAll);
    connect(m_suppliersWidget, &SuppliersWidget::dataChanged, this, &MainWindow::refreshAll);
    connect(m_stockWidget, &StockWidget::dataChanged, this, &MainWindow::refreshAll);
    connect(m_categoriesWidget, &CategoriesWidget::dataChanged, this, &MainWindow::refreshAll);
}

void MainWindow::setupSidebar(QWidget* sidebar) {
    auto layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 16);
    layout->setSpacing(4);

    auto logoLabel = new QLabel("StockPilot", sidebar);
    logoLabel->setObjectName("logoLabel");
    layout->addWidget(logoLabel);

    auto subLabel = new QLabel("Inventory Management", sidebar);
    subLabel->setObjectName("logoSubLabel");
    layout->addWidget(subLabel);

    QStringList navItems = {
        "  Dashboard",
        "  Products",
        "  Stock Operations",
        "  Suppliers",
        "  Categories",
        "  Reports"
    };

    for (int i = 0; i < navItems.size(); ++i) {
        auto btn = new QPushButton(navItems[i], sidebar);
        btn->setProperty("class", "navBtn");
        btn->setProperty("pageIndex", i);
        btn->setCheckable(true);
        connect(btn, &QPushButton::clicked, this, &MainWindow::onNavButtonClicked);
        m_navButtons.append(btn);
        layout->addWidget(btn);
    }

    layout->addStretch();

    // Bottom sidebar actions
    auto backupBtn = new QPushButton("  Backup Database", sidebar);
    backupBtn->setProperty("class", "navBtn");
    connect(backupBtn, &QPushButton::clicked, this, &MainWindow::onBackupDatabase);
    layout->addWidget(backupBtn);

    auto aboutBtn = new QPushButton("  About StockPilot", sidebar);
    aboutBtn->setProperty("class", "navBtn");
    connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::showAboutDialog);
    layout->addWidget(aboutBtn);
}

void MainWindow::setupTopBar(QWidget* topBar) {
    auto layout = new QHBoxLayout(topBar);
    layout->setContentsMargins(24, 12, 24, 12);
    layout->setSpacing(16);

    m_pageTitleLabel = new QLabel("Dashboard", topBar);
    m_pageTitleLabel->setObjectName("pageTitle");
    layout->addWidget(m_pageTitleLabel);

    layout->addStretch();

    // Database Status Indicator
    m_dbStatusLabel = new QLabel(topBar);
    if (DatabaseManager::instance().isConnected()) {
        m_dbStatusLabel->setText("● SQLite Connected (Local)");
        m_dbStatusLabel->setStyleSheet("color: #15803d; font-weight: 600; font-size: 12px; background: #dcfce7; padding: 4px 10px; border-radius: 4px;");
    } else {
        m_dbStatusLabel->setText("● SQLite Disconnected");
        m_dbStatusLabel->setStyleSheet("color: #b91c1c; font-weight: 600; font-size: 12px; background: #fee2e2; padding: 4px 10px; border-radius: 4px;");
    }
    m_dbStatusLabel->setToolTip("Database: " + DatabaseManager::instance().databasePath());
    layout->addWidget(m_dbStatusLabel);

    // Live clock
    m_clockLabel = new QLabel(QDateTime::currentDateTime().toString("dddd, MMM d, yyyy  hh:mm:ss AP"), topBar);
    m_clockLabel->setObjectName("topInfoLabel");
    layout->addWidget(m_clockLabel);
}

void MainWindow::onNavButtonClicked() {
    auto senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn) return;
    int index = senderBtn->property("pageIndex").toInt();
    navigateToPage(index);
}

void MainWindow::navigateToPage(int index) {
    if (index < 0 || index >= m_stackedWidget->count()) return;

    m_stackedWidget->setCurrentIndex(index);

    for (int i = 0; i < m_navButtons.size(); ++i) {
        m_navButtons[i]->setChecked(i == index);
    }

    switch (index) {
        case 0:
            m_pageTitleLabel->setText("Dashboard Overview");
            m_dashboardWidget->refresh();
            break;
        case 1:
            m_pageTitleLabel->setText("Product Catalog");
            m_productsWidget->refresh();
            break;
        case 2:
            m_pageTitleLabel->setText("Stock In & Out Operations");
            m_stockWidget->refresh();
            break;
        case 3:
            m_pageTitleLabel->setText("Supplier Directory");
            m_suppliersWidget->refresh();
            break;
        case 4:
            m_pageTitleLabel->setText("Category Management");
            m_categoriesWidget->refresh();
            break;
        case 5:
            m_pageTitleLabel->setText("Analytics & Inventory Reports");
            m_reportsWidget->refresh();
            break;
        default:
            break;
    }
}

void MainWindow::refreshAll() {
    m_dashboardWidget->refresh();
    m_productsWidget->refresh();
    m_stockWidget->refresh();
    m_suppliersWidget->refresh();
    m_categoriesWidget->refresh();
    m_reportsWidget->refresh();
}

void MainWindow::onBackupDatabase() {
    QString defaultName = QString("stockpilot_backup_%1.db")
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString destPath = QFileDialog::getSaveFileName(this, "Backup SQLite Database", defaultName, "SQLite Database Files (*.db *.sqlite)");
    if (destPath.isEmpty()) return;

    if (DatabaseManager::instance().backupDatabase(destPath)) {
        QMessageBox::information(this, "Backup Successful",
                                 QString("Database successfully copied to:\n%1").arg(destPath));
    } else {
        QMessageBox::critical(this, "Backup Failed", "Could not write database backup file.");
    }
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "About StockPilot",
                       "<h3>StockPilot – Desktop Inventory Management System</h3>"
                       "<p><b>Version:</b> 1.0 (Standalone Desktop Release)</p>"
                       "<p><b>Framework:</b> Qt 6 Widgets | C++17 Standard</p>"
                       "<p><b>Storage:</b> SQLite Relational Database Engine (QSQLITE)</p>"
                       "<p><b>Database Location:</b><br/>" + DatabaseManager::instance().databasePath() + "</p>"
                       "<hr/>"
                       "<p>Designed for small-to-medium retail and wholesale inventory workflows. Operates 100% offline with zero cloud server dependencies.</p>"
    );
}
