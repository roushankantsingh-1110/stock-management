#include "ReportsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFrame>

ReportsWidget::ReportsWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    refresh();
}

void ReportsWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(14);

    // 1. Selector and Filters Bar
    auto filterCard = new QFrame(this);
    filterCard->setProperty("class", "card");
    auto filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(14, 12, 14, 12);
    filterLayout->setSpacing(10);

    filterLayout->addWidget(new QLabel("Report Type:", filterCard));

    m_reportTypeCombo = new QComboBox(filterCard);
    m_reportTypeCombo->addItem("Inventory Valuation & Stock Summary", 0);
    m_reportTypeCombo->addItem("Low-Stock Reorder Report", 1);
    m_reportTypeCombo->addItem("Stock Movement Journal", 2);
    m_reportTypeCombo->addItem("Supplier Product Catalog", 3);
    m_reportTypeCombo->setMinimumWidth(240);
    connect(m_reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ReportsWidget::onReportTypeChanged);
    filterLayout->addWidget(m_reportTypeCombo);

    // Date filters (visible for Stock Movement)
    m_dateFilterContainer = new QWidget(filterCard);
    auto dateLayout = new QHBoxLayout(m_dateFilterContainer);
    dateLayout->setContentsMargins(0, 0, 0, 0);
    dateLayout->setSpacing(6);
    dateLayout->addWidget(new QLabel("From:", m_dateFilterContainer));
    m_startDateEdit = new QDateEdit(m_dateFilterContainer);
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &ReportsWidget::onFilterChanged);
    dateLayout->addWidget(m_startDateEdit);

    dateLayout->addWidget(new QLabel("To:", m_dateFilterContainer));
    m_endDateEdit = new QDateEdit(m_dateFilterContainer);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDate(QDate::currentDate().addDays(1));
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &ReportsWidget::onFilterChanged);
    dateLayout->addWidget(m_endDateEdit);
    m_dateFilterContainer->setVisible(false);
    filterLayout->addWidget(m_dateFilterContainer);

    // Supplier filter (visible for Supplier Report)
    m_supplierFilterContainer = new QWidget(filterCard);
    auto supLayout = new QHBoxLayout(m_supplierFilterContainer);
    supLayout->setContentsMargins(0, 0, 0, 0);
    supLayout->setSpacing(6);
    supLayout->addWidget(new QLabel("Supplier:", m_supplierFilterContainer));
    m_supplierCombo = new QComboBox(m_supplierFilterContainer);
    m_supplierCombo->setMinimumWidth(180);
    connect(m_supplierCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ReportsWidget::onFilterChanged);
    supLayout->addWidget(m_supplierCombo);
    m_supplierFilterContainer->setVisible(false);
    filterLayout->addWidget(m_supplierFilterContainer);

    filterLayout->addStretch();

    auto exportBtn = new QPushButton("Export to CSV", filterCard);
    exportBtn->setProperty("class", "primaryBtn");
    connect(exportBtn, &QPushButton::clicked, this, &ReportsWidget::onExportCsvClicked);
    filterLayout->addWidget(exportBtn);

    auto refreshBtn = new QPushButton("Generate", filterCard);
    refreshBtn->setProperty("class", "secondaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &ReportsWidget::onFilterChanged);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterCard);

    // 2. Summary bar
    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setStyleSheet("color: #0f172a; font-weight: 600; font-size: 13px;");
    mainLayout->addWidget(m_summaryLabel);

    // 3. Table
    m_table = new QTableWidget(this);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_table);
}

void ReportsWidget::refresh() {
    m_supplierCombo->blockSignals(true);
    m_supplierCombo->clear();
    m_supplierCombo->addItem("All Suppliers", 0);
    auto sups = m_supplierRepo.getAll();
    for (const auto& s : sups) {
        m_supplierCombo->addItem(s.name, s.id);
    }
    m_supplierCombo->blockSignals(false);

    generateReport();
}

void ReportsWidget::onReportTypeChanged(int index) {
    m_dateFilterContainer->setVisible(index == 2);
    m_supplierFilterContainer->setVisible(index == 3);
    generateReport();
}

void ReportsWidget::onFilterChanged() {
    generateReport();
}

void ReportsWidget::generateReport() {
    int repType = m_reportTypeCombo->currentData().toInt();
    m_table->clear();

    if (repType == 0) {
        // Inventory Valuation Report
        auto products = m_reportService.getInventoryValuationReport();
        m_table->setColumnCount(7);
        m_table->setHorizontalHeaderLabels({
            "Product ID", "Product Name", "Category", "Quantity", "Purchase Cost", "Total Asset Value", "Potential Retail Value"
        });
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

        m_table->setRowCount(static_cast<int>(products.size()));
        double totalCost = 0.0;
        double totalRetail = 0.0;
        int totalUnits = 0;

        for (int i = 0; i < static_cast<int>(products.size()); ++i) {
            const auto& p = products[i];
            double assetVal = p.quantity * p.purchasePrice;
            double retailVal = p.quantity * p.sellingPrice;
            totalCost += assetVal;
            totalRetail += retailVal;
            totalUnits += p.quantity;

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
            m_table->setItem(i, 1, new QTableWidgetItem(p.name));
            m_table->setItem(i, 2, new QTableWidgetItem(p.categoryName));

            auto qItem = new QTableWidgetItem(QString::number(p.quantity));
            qItem->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, 3, qItem);

            auto cItem = new QTableWidgetItem(QString("$ %1").arg(p.purchasePrice, 0, 'f', 2));
            cItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_table->setItem(i, 4, cItem);

            auto aItem = new QTableWidgetItem(QString("$ %1").arg(assetVal, 0, 'f', 2));
            aItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_table->setItem(i, 5, aItem);

            auto rItem = new QTableWidgetItem(QString("$ %1").arg(retailVal, 0, 'f', 2));
            rItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_table->setItem(i, 6, rItem);
        }

        m_summaryLabel->setText(
            QString("Total SKUs: %1 | Total Stock Units: %2 | Total Inventory Valuation: $%3 | Projected Retail Value: $%4")
                .arg(products.size())
                .arg(totalUnits)
                .arg(totalCost, 0, 'f', 2)
                .arg(totalRetail, 0, 'f', 2)
        );

    } else if (repType == 1) {
        // Low Stock Reorder Report
        auto products = m_reportService.getLowStockReport();
        m_table->setColumnCount(6);
        m_table->setHorizontalHeaderLabels({
            "Product ID", "Product Name", "Category", "Current Stock", "Min Threshold", "Recommended Reorder"
        });
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

        m_table->setRowCount(static_cast<int>(products.size()));

        for (int i = 0; i < static_cast<int>(products.size()); ++i) {
            const auto& p = products[i];
            int recommendedReorder = std::max(10, (p.minimumStock * 2) - p.quantity);

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
            m_table->setItem(i, 1, new QTableWidgetItem(p.name));
            m_table->setItem(i, 2, new QTableWidgetItem(p.categoryName));

            auto curItem = new QTableWidgetItem(QString::number(p.quantity));
            curItem->setTextAlignment(Qt::AlignCenter);
            curItem->setForeground(QColor("#b91c1c"));
            QFont f = curItem->font();
            f.setBold(true);
            curItem->setFont(f);
            m_table->setItem(i, 3, curItem);

            auto minItem = new QTableWidgetItem(QString::number(p.minimumStock));
            minItem->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, 4, minItem);

            auto recItem = new QTableWidgetItem(QString("+%1 units").arg(recommendedReorder));
            recItem->setTextAlignment(Qt::AlignCenter);
            recItem->setForeground(QColor("#15803d"));
            m_table->setItem(i, 5, recItem);
        }

        m_summaryLabel->setText(QString("Found %1 product(s) needing reorder attention.").arg(products.size()));

    } else if (repType == 2) {
        // Stock Movement Journal
        QDate from = m_startDateEdit->date();
        QDate to = m_endDateEdit->date();
        auto trans = m_reportService.getStockMovementReport(from, to);

        m_table->setColumnCount(6);
        m_table->setHorizontalHeaderLabels({"ID", "Date / Time", "Product", "Type", "Units Moved", "Remarks"});
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

        m_table->setRowCount(static_cast<int>(trans.size()));
        int totalIn = 0;
        int totalOut = 0;

        for (int i = 0; i < static_cast<int>(trans.size()); ++i) {
            const auto& t = trans[i];
            if (t.transactionType == "IN") totalIn += t.quantity;
            else totalOut += t.quantity;

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
            m_table->setItem(i, 1, new QTableWidgetItem(t.transactionDate));
            m_table->setItem(i, 2, new QTableWidgetItem(t.productName));

            auto typeItem = new QTableWidgetItem(t.transactionType == "IN" ? " STOCK IN" : " STOCK OUT");
            typeItem->setForeground(t.transactionType == "IN" ? QColor("#15803d") : QColor("#b91c1c"));
            m_table->setItem(i, 3, typeItem);

            auto qItem = new QTableWidgetItem(QString::number(t.quantity));
            qItem->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, 4, qItem);

            m_table->setItem(i, 5, new QTableWidgetItem(t.remarks));
        }

        m_summaryLabel->setText(
            QString("Movement Period: %1 to %2 | Transactions: %3 | Total Received: +%4 | Total Dispatched: -%5")
                .arg(from.toString("yyyy-MM-dd"))
                .arg(to.toString("yyyy-MM-dd"))
                .arg(trans.size())
                .arg(totalIn)
                .arg(totalOut)
        );

    } else if (repType == 3) {
        // Supplier-wise Catalog
        int supId = m_supplierCombo->currentData().toInt();
        auto products = m_reportService.getSupplierWiseReport(supId);

        m_table->setColumnCount(6);
        m_table->setHorizontalHeaderLabels({"Product ID", "Product Name", "Supplier", "Purchase Cost", "Selling Price", "Units in Stock"});
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

        m_table->setRowCount(static_cast<int>(products.size()));

        for (int i = 0; i < static_cast<int>(products.size()); ++i) {
            const auto& p = products[i];
            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
            m_table->setItem(i, 1, new QTableWidgetItem(p.name));
            m_table->setItem(i, 2, new QTableWidgetItem(p.supplierName));

            auto bItem = new QTableWidgetItem(QString("$ %1").arg(p.purchasePrice, 0, 'f', 2));
            bItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_table->setItem(i, 3, bItem);

            auto sItem = new QTableWidgetItem(QString("$ %1").arg(p.sellingPrice, 0, 'f', 2));
            sItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_table->setItem(i, 4, sItem);

            auto qItem = new QTableWidgetItem(QString::number(p.quantity));
            qItem->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, 5, qItem);
        }

        m_summaryLabel->setText(QString("Found %1 item(s) in catalog.").arg(products.size()));
    }
}

void ReportsWidget::onExportCsvClicked() {
    QString defaultName = "stockpilot_report.csv";
    QString filePath = QFileDialog::getSaveFileName(this, "Export Report to CSV", defaultName, "CSV Files (*.csv)");
    if (filePath.isEmpty()) return;

    QStringList headers;
    for (int c = 0; c < m_table->columnCount(); ++c) {
        headers << m_table->horizontalHeaderItem(c)->text();
    }

    std::vector<QStringList> rows;
    for (int r = 0; r < m_table->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < m_table->columnCount(); ++c) {
            row << (m_table->item(r, c) ? m_table->item(r, c)->text().trimmed() : "");
        }
        rows.push_back(row);
    }

    QString err;
    if (m_reportService.exportToCsv(filePath, headers, rows, err)) {
        QMessageBox::information(this, "Export Success", "Report data successfully exported to CSV!");
    } else {
        QMessageBox::critical(this, "Export Failed", err);
    }
}
