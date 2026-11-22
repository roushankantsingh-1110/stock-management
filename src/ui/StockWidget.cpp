#include "StockWidget.h"
#include "../services/ReportService.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QFrame>

StockWidget::StockWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    refresh();
}

void StockWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(14);

    m_tabWidget = new QTabWidget(this);

    auto inTab = new QWidget();
    setupStockInTab(inTab);
    m_tabWidget->addTab(inTab, "Stock In (Add Units)");

    auto outTab = new QWidget();
    setupStockOutTab(outTab);
    m_tabWidget->addTab(outTab, "Stock Out (Deduct Units)");

    auto histTab = new QWidget();
    setupHistoryTab(histTab);
    m_tabWidget->addTab(histTab, "Transaction History Ledger");

    mainLayout->addWidget(m_tabWidget);
}

void StockWidget::setupStockInTab(QWidget* tab) {
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto card = new QFrame(tab);
    card->setProperty("class", "card");
    auto formLayout = new QFormLayout(card);
    formLayout->setContentsMargins(24, 20, 24, 20);
    formLayout->setSpacing(12);

    auto title = new QLabel("Record Incoming Stock Movement", card);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a; margin-bottom: 8px;");
    formLayout->addRow(title);

    m_inProductCombo = new QComboBox(card);
    m_inProductCombo->setMinimumWidth(300);
    connect(m_inProductCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockWidget::onStockInProductChanged);
    formLayout->addRow("Select Product *:", m_inProductCombo);

    m_inCurrentStockLabel = new QLabel("0 units available", card);
    m_inCurrentStockLabel->setStyleSheet("color: #475569; font-weight: 600;");
    formLayout->addRow("Current Stock Level:", m_inCurrentStockLabel);

    m_inQtySpin = new QSpinBox(card);
    m_inQtySpin->setRange(1, 1000000);
    m_inQtySpin->setValue(10);
    m_inQtySpin->setFixedWidth(140);
    formLayout->addRow("Units to Add *:", m_inQtySpin);

    m_inRemarksEdit = new QLineEdit(card);
    m_inRemarksEdit->setPlaceholderText("e.g. Shipment PO #4092, Supplier Delivery, Restock");
    formLayout->addRow("Notes / Reference:", m_inRemarksEdit);

    m_inStatusLabel = new QLabel(card);
    m_inStatusLabel->setVisible(false);
    formLayout->addRow(m_inStatusLabel);

    auto btn = new QPushButton("+ Submit Stock In", card);
    btn->setProperty("class", "successBtn");
    btn->setFixedWidth(180);
    connect(btn, &QPushButton::clicked, this, &StockWidget::onStockInSubmit);
    formLayout->addRow("", btn);

    layout->addWidget(card);
    layout->addStretch();
}

void StockWidget::setupStockOutTab(QWidget* tab) {
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto card = new QFrame(tab);
    card->setProperty("class", "card");
    auto formLayout = new QFormLayout(card);
    formLayout->setContentsMargins(24, 20, 24, 20);
    formLayout->setSpacing(12);

    auto title = new QLabel("Record Outgoing Stock Movement", card);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a; margin-bottom: 8px;");
    formLayout->addRow(title);

    m_outProductCombo = new QComboBox(card);
    m_outProductCombo->setMinimumWidth(300);
    connect(m_outProductCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockWidget::onStockOutProductChanged);
    formLayout->addRow("Select Product *:", m_outProductCombo);

    m_outCurrentStockLabel = new QLabel("0 units available", card);
    m_outCurrentStockLabel->setStyleSheet("color: #475569; font-weight: 600;");
    formLayout->addRow("Available Inventory:", m_outCurrentStockLabel);

    m_outQtySpin = new QSpinBox(card);
    m_outQtySpin->setRange(1, 1000000);
    m_outQtySpin->setValue(1);
    m_outQtySpin->setFixedWidth(140);
    formLayout->addRow("Units to Deduct *:", m_outQtySpin);

    m_outRemarksEdit = new QLineEdit(card);
    m_outRemarksEdit->setPlaceholderText("e.g. Sales Order #1209, Store display, Damaged goods");
    formLayout->addRow("Notes / Reference:", m_outRemarksEdit);

    m_outStatusLabel = new QLabel(card);
    m_outStatusLabel->setVisible(false);
    formLayout->addRow(m_outStatusLabel);

    auto btn = new QPushButton("- Submit Stock Out", card);
    btn->setProperty("class", "dangerBtn");
    btn->setFixedWidth(180);
    connect(btn, &QPushButton::clicked, this, &StockWidget::onStockOutSubmit);
    formLayout->addRow("", btn);

    layout->addWidget(card);
    layout->addStretch();
}

void StockWidget::setupHistoryTab(QWidget* tab) {
    auto layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Filters
    auto filterCard = new QFrame(tab);
    filterCard->setProperty("class", "card");
    auto filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(12, 10, 12, 10);
    filterLayout->setSpacing(10);

    m_histProductCombo = new QComboBox(filterCard);
    m_histProductCombo->setMinimumWidth(180);
    connect(m_histProductCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockWidget::onHistoryFilterChanged);
    filterLayout->addWidget(new QLabel("Product:", filterCard));
    filterLayout->addWidget(m_histProductCombo);

    m_histTypeCombo = new QComboBox(filterCard);
    m_histTypeCombo->addItem("All Types", "ALL");
    m_histTypeCombo->addItem("Stock In Only", "IN");
    m_histTypeCombo->addItem("Stock Out Only", "OUT");
    connect(m_histTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockWidget::onHistoryFilterChanged);
    filterLayout->addWidget(new QLabel("Type:", filterCard));
    filterLayout->addWidget(m_histTypeCombo);

    m_histStartDate = new QDateEdit(filterCard);
    m_histStartDate->setCalendarPopup(true);
    m_histStartDate->setDate(QDate::currentDate().addMonths(-1));
    connect(m_histStartDate, &QDateEdit::dateChanged, this, &StockWidget::onHistoryFilterChanged);
    filterLayout->addWidget(new QLabel("From:", filterCard));
    filterLayout->addWidget(m_histStartDate);

    m_histEndDate = new QDateEdit(filterCard);
    m_histEndDate->setCalendarPopup(true);
    m_histEndDate->setDate(QDate::currentDate().addDays(1));
    connect(m_histEndDate, &QDateEdit::dateChanged, this, &StockWidget::onHistoryFilterChanged);
    filterLayout->addWidget(new QLabel("To:", filterCard));
    filterLayout->addWidget(m_histEndDate);

    filterLayout->addStretch();

    auto exportBtn = new QPushButton("Export CSV", filterCard);
    exportBtn->setProperty("class", "secondaryBtn");
    connect(exportBtn, &QPushButton::clicked, this, &StockWidget::onExportHistoryCsv);
    filterLayout->addWidget(exportBtn);

    layout->addWidget(filterCard);

    m_histCountLabel = new QLabel(tab);
    m_histCountLabel->setStyleSheet("color: #64748b; font-size: 12px;");
    layout->addWidget(m_histCountLabel);

    m_histTable = new QTableWidget(tab);
    m_histTable->setColumnCount(6);
    m_histTable->setHorizontalHeaderLabels({"ID", "Date / Time", "Product", "Type", "Units", "Remarks / Reference"});
    m_histTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_histTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_histTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_histTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_histTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_histTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

    m_histTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_histTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_histTable->setAlternatingRowColors(true);
    m_histTable->verticalHeader()->setVisible(false);

    layout->addWidget(m_histTable);
}

void StockWidget::reloadProductCombos() {
    auto prods = m_productService.getAllProducts();

    int inPrevId = m_inProductCombo->currentData().toInt();
    int outPrevId = m_outProductCombo->currentData().toInt();
    int histPrevId = m_histProductCombo->currentData().toInt();

    m_inProductCombo->blockSignals(true);
    m_outProductCombo->blockSignals(true);
    m_histProductCombo->blockSignals(true);

    m_inProductCombo->clear();
    m_outProductCombo->clear();
    m_histProductCombo->clear();

    m_histProductCombo->addItem("All Products", 0);

    for (const auto& p : prods) {
        QString itemText = QString("%1 (Stock: %2)").arg(p.name).arg(p.quantity);
        m_inProductCombo->addItem(itemText, p.id);
        m_outProductCombo->addItem(itemText, p.id);
        m_histProductCombo->addItem(p.name, p.id);
    }

    int idxIn = m_inProductCombo->findData(inPrevId);
    if (idxIn >= 0) m_inProductCombo->setCurrentIndex(idxIn);

    int idxOut = m_outProductCombo->findData(outPrevId);
    if (idxOut >= 0) m_outProductCombo->setCurrentIndex(idxOut);

    int idxHist = m_histProductCombo->findData(histPrevId);
    if (idxHist >= 0) m_histProductCombo->setCurrentIndex(idxHist);

    m_inProductCombo->blockSignals(false);
    m_outProductCombo->blockSignals(false);
    m_histProductCombo->blockSignals(false);

    onStockInProductChanged(m_inProductCombo->currentIndex());
    onStockOutProductChanged(m_outProductCombo->currentIndex());
}

void StockWidget::refresh() {
    reloadProductCombos();
    onHistoryFilterChanged();
}

void StockWidget::selectTab(int index) {
    if (index >= 0 && index < m_tabWidget->count()) {
        m_tabWidget->setCurrentIndex(index);
    }
}

void StockWidget::onStockInProductChanged(int index) {
    Q_UNUSED(index);
    int pId = m_inProductCombo->currentData().toInt();
    if (pId > 0) {
        auto opt = m_productService.getProductById(pId);
        if (opt.has_value()) {
            m_inCurrentStockLabel->setText(QString("%1 units currently in stock").arg(opt->quantity));
        }
    } else {
        m_inCurrentStockLabel->setText("No product selected");
    }
}

void StockWidget::onStockOutProductChanged(int index) {
    Q_UNUSED(index);
    int pId = m_outProductCombo->currentData().toInt();
    if (pId > 0) {
        auto opt = m_productService.getProductById(pId);
        if (opt.has_value()) {
            m_outCurrentStockLabel->setText(QString("%1 units currently available").arg(opt->quantity));
            m_outQtySpin->setMaximum(std::max(1, opt->quantity));
        }
    } else {
        m_outCurrentStockLabel->setText("No product selected");
    }
}

void StockWidget::onStockInSubmit() {
    int pId = m_inProductCombo->currentData().toInt();
    int qty = m_inQtySpin->value();
    QString remarks = m_inRemarksEdit->text();

    QString err;
    if (m_stockService.stockIn(pId, qty, remarks, err)) {
        m_inRemarksEdit->clear();
        m_inQtySpin->setValue(10);
        refresh();
        emit dataChanged();
        QMessageBox::information(this, "Success", QString("Successfully added %1 units to stock!").arg(qty));
    } else {
        QMessageBox::critical(this, "Stock In Failed", err);
    }
}

void StockWidget::onStockOutSubmit() {
    int pId = m_outProductCombo->currentData().toInt();
    int qty = m_outQtySpin->value();
    QString remarks = m_outRemarksEdit->text();

    QString err;
    if (m_stockService.stockOut(pId, qty, remarks, err)) {
        m_outRemarksEdit->clear();
        m_outQtySpin->setValue(1);
        refresh();
        emit dataChanged();
        QMessageBox::information(this, "Success", QString("Successfully dispatched %1 units from stock!").arg(qty));
    } else {
        QMessageBox::critical(this, "Stock Out Failed", err);
    }
}

void StockWidget::onHistoryFilterChanged() {
    int pId = m_histProductCombo->currentData().toInt();
    QString type = m_histTypeCombo->currentData().toString();
    QDate start = m_histStartDate->date();
    QDate end = m_histEndDate->date();

    auto trans = m_stockService.getFilteredTransactions(pId, type, start, end);
    m_histTable->setRowCount(static_cast<int>(trans.size()));
    m_histCountLabel->setText(QString("Found %1 transaction records").arg(trans.size()));

    for (int i = 0; i < static_cast<int>(trans.size()); ++i) {
        const auto& t = trans[i];

        auto idItem = new QTableWidgetItem(QString::number(t.id));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_histTable->setItem(i, 0, idItem);

        m_histTable->setItem(i, 1, new QTableWidgetItem(t.transactionDate));
        m_histTable->setItem(i, 2, new QTableWidgetItem(t.productName));

        auto typeItem = new QTableWidgetItem(t.transactionType == "IN" ? " STOCK IN" : " STOCK OUT");
        if (t.transactionType == "IN") {
            typeItem->setForeground(QColor("#15803d"));
        } else {
            typeItem->setForeground(QColor("#b91c1c"));
        }
        QFont f = typeItem->font();
        f.setBold(true);
        typeItem->setFont(f);
        m_histTable->setItem(i, 3, typeItem);

        auto qtyItem = new QTableWidgetItem(QString::number(t.quantity));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        m_histTable->setItem(i, 4, qtyItem);

        m_histTable->setItem(i, 5, new QTableWidgetItem(t.remarks));
    }
}

void StockWidget::onExportHistoryCsv() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Transaction History", "stock_transactions.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QStringList headers = {"Transaction ID", "Date", "Product", "Type", "Quantity", "Remarks"};
    std::vector<QStringList> rows;

    for (int i = 0; i < m_histTable->rowCount(); ++i) {
        QStringList row;
        for (int c = 0; c < 6; ++c) {
            row << (m_histTable->item(i, c) ? m_histTable->item(i, c)->text().trimmed() : "");
        }
        rows.push_back(row);
    }

    ReportService repService;
    QString err;
    if (repService.exportToCsv(fileName, headers, rows, err)) {
        QMessageBox::information(this, "Export Success", "Transaction history successfully exported to CSV!");
    } else {
        QMessageBox::critical(this, "Export Failed", err);
    }
}
