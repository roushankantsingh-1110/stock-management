#include "SuppliersWidget.h"
#include "SupplierDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>

SuppliersWidget::SuppliersWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    refresh();
}

void SuppliersWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(14);

    // 1. Controls Bar
    auto filterCard = new QFrame(this);
    filterCard->setProperty("class", "card");
    auto filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(14, 12, 14, 12);
    filterLayout->setSpacing(10);

    m_searchEdit = new QLineEdit(filterCard);
    m_searchEdit->setPlaceholderText("Search suppliers by name, phone or email...");
    m_searchEdit->setMinimumWidth(260);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SuppliersWidget::onSearchChanged);
    filterLayout->addWidget(m_searchEdit);

    filterLayout->addStretch();

    auto addBtn = new QPushButton("+ Add Supplier", filterCard);
    addBtn->setProperty("class", "primaryBtn");
    connect(addBtn, &QPushButton::clicked, this, &SuppliersWidget::onAddClicked);
    filterLayout->addWidget(addBtn);

    auto editBtn = new QPushButton("Edit", filterCard);
    editBtn->setProperty("class", "secondaryBtn");
    connect(editBtn, &QPushButton::clicked, this, &SuppliersWidget::onEditClicked);
    filterLayout->addWidget(editBtn);

    auto viewProdsBtn = new QPushButton("Supplied Products", filterCard);
    viewProdsBtn->setProperty("class", "secondaryBtn");
    connect(viewProdsBtn, &QPushButton::clicked, this, &SuppliersWidget::onViewProductsClicked);
    filterLayout->addWidget(viewProdsBtn);

    auto delBtn = new QPushButton("Delete", filterCard);
    delBtn->setProperty("class", "dangerBtn");
    connect(delBtn, &QPushButton::clicked, this, &SuppliersWidget::onDeleteClicked);
    filterLayout->addWidget(delBtn);

    auto refreshBtn = new QPushButton("Refresh", filterCard);
    refreshBtn->setProperty("class", "secondaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &SuppliersWidget::refresh);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterCard);

    // 2. Count label
    m_resultCountLabel = new QLabel(this);
    m_resultCountLabel->setStyleSheet("color: #64748b; font-weight: 500; font-size: 12px;");
    mainLayout->addWidget(m_resultCountLabel);

    // 3. Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID", "Supplier / Company Name", "Phone", "Email", "Address", "Items Supplied"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);

    connect(m_table, &QTableWidget::itemDoubleClicked, this, &SuppliersWidget::onItemDoubleClicked);

    mainLayout->addWidget(m_table);
}

void SuppliersWidget::onSearchChanged() {
    QString kw = m_searchEdit->text().trimmed();
    if (kw.isEmpty()) {
        m_currentSuppliers = m_supplierService.getAllSuppliers();
    } else {
        m_currentSuppliers = m_supplierService.searchSuppliers(kw);
    }
    renderTable(m_currentSuppliers);
}

void SuppliersWidget::refresh() {
    onSearchChanged();
}

void SuppliersWidget::renderTable(const std::vector<Supplier>& suppliers) {
    m_table->setRowCount(static_cast<int>(suppliers.size()));
    m_resultCountLabel->setText(QString("Displaying %1 supplier(s)").arg(suppliers.size()));

    for (int i = 0; i < static_cast<int>(suppliers.size()); ++i) {
        const auto& s = suppliers[i];

        auto idItem = new QTableWidgetItem(QString::number(s.id));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, idItem);

        auto nameItem = new QTableWidgetItem(s.name);
        QFont f = nameItem->font();
        f.setBold(true);
        nameItem->setFont(f);
        m_table->setItem(i, 1, nameItem);

        m_table->setItem(i, 2, new QTableWidgetItem(s.phone.isEmpty() ? "—" : s.phone));
        m_table->setItem(i, 3, new QTableWidgetItem(s.email.isEmpty() ? "—" : s.email));
        m_table->setItem(i, 4, new QTableWidgetItem(s.address.isEmpty() ? "—" : s.address));

        auto prods = m_supplierService.getProductsBySupplier(s.id);
        auto countItem = new QTableWidgetItem(QString("%1 item(s)").arg(prods.size()));
        countItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 5, countItem);
    }
}

void SuppliersWidget::onAddClicked() {
    SupplierDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Supplier s = dlg.getSupplier();
        QString err;
        if (m_supplierService.createSupplier(s, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Supplier registered successfully!");
        } else {
            QMessageBox::critical(this, "Error", err);
        }
    }
}

void SuppliersWidget::onEditClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= static_cast<int>(m_currentSuppliers.size())) {
        QMessageBox::warning(this, "Selection Required", "Please select a supplier row to edit.");
        return;
    }

    const auto& s = m_currentSuppliers[row];
    SupplierDialog dlg(this, &s);
    if (dlg.exec() == QDialog::Accepted) {
        Supplier updated = dlg.getSupplier();
        QString err;
        if (m_supplierService.updateSupplier(updated, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Supplier details updated successfully!");
        } else {
            QMessageBox::critical(this, "Error", err);
        }
    }
}

void SuppliersWidget::onItemDoubleClicked(QTableWidgetItem* item) {
    Q_UNUSED(item);
    onEditClicked();
}

void SuppliersWidget::onViewProductsClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= static_cast<int>(m_currentSuppliers.size())) {
        QMessageBox::warning(this, "Selection Required", "Please select a supplier to view associated products.");
        return;
    }

    const auto& s = m_currentSuppliers[row];
    auto prods = m_supplierService.getProductsBySupplier(s.id);
    if (prods.empty()) {
        QMessageBox::information(this, "Supplied Products",
                                 QString("No products currently assigned to supplier '%1'.").arg(s.name));
        return;
    }

    QString msg = QString("Products supplied by '%1':\n\n").arg(s.name);
    for (const auto& p : prods) {
        msg += QString("- %1 (Current Stock: %2 units, Price: $%3)\n")
                   .arg(p.name)
                   .arg(p.quantity)
                   .arg(p.purchasePrice, 0, 'f', 2);
    }
    QMessageBox::information(this, "Supplied Products", msg);
}

void SuppliersWidget::onDeleteClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= static_cast<int>(m_currentSuppliers.size())) {
        QMessageBox::warning(this, "Selection Required", "Please select a supplier row to delete.");
        return;
    }

    const auto& s = m_currentSuppliers[row];

    auto reply = QMessageBox::question(
        this, "Confirm Deletion",
        QString("Are you sure you want to delete supplier '%1'?").arg(s.name),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        QString err;
        if (m_supplierService.deleteSupplier(s.id, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Supplier deleted successfully.");
        } else {
            QMessageBox::critical(this, "Deletion Prevented", err);
        }
    }
}
