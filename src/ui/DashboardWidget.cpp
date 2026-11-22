#include "DashboardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFrame>

DashboardWidget::DashboardWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    refresh();
}

QWidget* DashboardWidget::createStatCard(const QString& title, QLabel** outValueLabel, const QString& iconText, const QString& accentColor) {
    auto card = new QFrame(this);
    card->setProperty("class", "card");
    card->setStyleSheet(QString(
        "QFrame.card { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; border-left: 4px solid %1; }"
    ).arg(accentColor));

    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(6);

    auto topRow = new QHBoxLayout();
    auto titleLabel = new QLabel(title, card);
    titleLabel->setProperty("class", "statLabel");
    titleLabel->setStyleSheet("color: #64748b; font-weight: 600; font-size: 12px; text-transform: uppercase;");
    topRow->addWidget(titleLabel);

    topRow->addStretch();

    auto iconLabel = new QLabel(iconText, card);
    iconLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(accentColor));
    topRow->addWidget(iconLabel);

    layout->addLayout(topRow);

    auto valLabel = new QLabel("0", card);
    valLabel->setProperty("class", "statValue");
    valLabel->setStyleSheet("font-size: 26px; font-weight: 700; color: #0f172a;");
    layout->addWidget(valLabel);

    *outValueLabel = valLabel;
    return card;
}

void DashboardWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(18);

    // 1. Stat cards grid
    auto cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(16);

    cardsLayout->addWidget(createStatCard("Total Products", &m_totalProductsLabel, "[Items]", "#2563eb"));
    cardsLayout->addWidget(createStatCard("Total Units in Stock", &m_totalStockUnitsLabel, "[Stock]", "#0284c7"));
    cardsLayout->addWidget(createStatCard("Total Inventory Value", &m_inventoryValueLabel, "[Value]", "#16a34a"));
    cardsLayout->addWidget(createStatCard("Low Stock Alert", &m_lowStockCountLabel, "[Alert]", "#ea580c"));

    mainLayout->addLayout(cardsLayout);

    // 2. Alert Banner (hidden if 0 low stock)
    m_alertBanner = new QFrame(this);
    m_alertBanner->setStyleSheet("background-color: #fef2f2; border: 1px solid #fecaca; border-radius: 6px; padding: 10px;");
    auto alertLayout = new QHBoxLayout(m_alertBanner);
    alertLayout->setContentsMargins(12, 8, 12, 8);
    m_alertText = new QLabel(m_alertBanner);
    m_alertText->setStyleSheet("color: #991b1b; font-weight: 600;");
    alertLayout->addWidget(m_alertText);

    alertLayout->addStretch();
    auto viewLowBtn = new QPushButton("View Low Stock Products", m_alertBanner);
    viewLowBtn->setProperty("class", "secondaryBtn");
    connect(viewLowBtn, &QPushButton::clicked, this, [this]() {
        emit navigateToPage(1); // Page 1 is Products
    });
    alertLayout->addWidget(viewLowBtn);

    mainLayout->addWidget(m_alertBanner);

    // 3. Middle Action Bar
    auto actionCard = new QFrame(this);
    actionCard->setProperty("class", "card");
    auto actionLayout = new QHBoxLayout(actionCard);
    actionLayout->setContentsMargins(16, 12, 16, 12);

    auto actionTitle = new QLabel("Quick Operations:", actionCard);
    actionTitle->setStyleSheet("font-weight: 600; color: #334155;");
    actionLayout->addWidget(actionTitle);

    auto stockInBtn = new QPushButton("+ Stock In (Receive)", actionCard);
    stockInBtn->setProperty("class", "successBtn");
    connect(stockInBtn, &QPushButton::clicked, this, &DashboardWidget::requestStockIn);
    actionLayout->addWidget(stockInBtn);

    auto stockOutBtn = new QPushButton("- Stock Out (Dispatch)", actionCard);
    stockOutBtn->setProperty("class", "dangerBtn");
    connect(stockOutBtn, &QPushButton::clicked, this, &DashboardWidget::requestStockOut);
    actionLayout->addWidget(stockOutBtn);

    auto addProdBtn = new QPushButton("+ Add New Product", actionCard);
    addProdBtn->setProperty("class", "primaryBtn");
    connect(addProdBtn, &QPushButton::clicked, this, &DashboardWidget::requestAddProduct);
    actionLayout->addWidget(addProdBtn);

    actionLayout->addStretch();

    auto refreshBtn = new QPushButton("Refresh", actionCard);
    refreshBtn->setProperty("class", "secondaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &DashboardWidget::refresh);
    actionLayout->addWidget(refreshBtn);

    mainLayout->addWidget(actionCard);

    // 4. Recent Transactions Section
    auto recentHeader = new QLabel("Recent Inventory Movements", this);
    recentHeader->setProperty("class", "cardHeader");
    recentHeader->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a;");
    mainLayout->addWidget(recentHeader);

    m_recentTable = new QTableWidget(this);
    m_recentTable->setColumnCount(5);
    m_recentTable->setHorizontalHeaderLabels({"Date / Time", "Product", "Type", "Units", "Remarks"});
    m_recentTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_recentTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_recentTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_recentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_recentTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_recentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_recentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentTable->setAlternatingRowColors(true);
    m_recentTable->verticalHeader()->setVisible(false);

    mainLayout->addWidget(m_recentTable);
}

void DashboardWidget::refresh() {
    int totalProds = m_productService.getTotalProductCount();
    int totalUnits = m_productService.getTotalStockUnits();
    double totalVal = m_productService.getTotalInventoryValue();
    int lowStock = m_productService.getLowStockCount();

    m_totalProductsLabel->setText(QString::number(totalProds));
    m_totalStockUnitsLabel->setText(QString::number(totalUnits));
    m_inventoryValueLabel->setText(QString("$ %1").arg(totalVal, 0, 'f', 2));
    m_lowStockCountLabel->setText(QString::number(lowStock));

    if (lowStock > 0) {
        m_alertText->setText(QString("Attention Required: %1 product(s) are at or below minimum threshold!").arg(lowStock));
        m_alertBanner->setVisible(true);
    } else {
        m_alertBanner->setVisible(false);
    }

    // Populate recent transactions
    auto recent = m_stockService.getRecentTransactions(10);
    m_recentTable->setRowCount(static_cast<int>(recent.size()));
    for (int i = 0; i < static_cast<int>(recent.size()); ++i) {
        const auto& t = recent[i];
        m_recentTable->setItem(i, 0, new QTableWidgetItem(t.transactionDate));
        m_recentTable->setItem(i, 1, new QTableWidgetItem(t.productName));

        auto typeItem = new QTableWidgetItem(t.transactionType == "IN" ? " STOCK IN" : " STOCK OUT");
        if (t.transactionType == "IN") {
            typeItem->setForeground(QColor("#15803d"));
        } else {
            typeItem->setForeground(QColor("#b91c1c"));
        }
        QFont f = typeItem->font();
        f.setBold(true);
        typeItem->setFont(f);
        m_recentTable->setItem(i, 2, typeItem);

        auto qtyItem = new QTableWidgetItem(QString::number(t.quantity));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        m_recentTable->setItem(i, 3, qtyItem);

        m_recentTable->setItem(i, 4, new QTableWidgetItem(t.remarks));
    }
}
