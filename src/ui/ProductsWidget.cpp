#include "ProductsWidget.h"
#include "ProductDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFrame>

ProductsWidget::ProductsWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadFilterCombos();
    refresh();
}

void ProductsWidget::setupUI() {
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
    m_searchEdit->setPlaceholderText("Search products by name or description...");
    m_searchEdit->setMinimumWidth(220);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ProductsWidget::onSearchChanged);
    filterLayout->addWidget(m_searchEdit);

    m_categoryCombo = new QComboBox(filterCard);
    m_categoryCombo->setMinimumWidth(140);
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProductsWidget::onSearchChanged);
    filterLayout->addWidget(m_categoryCombo);

    m_supplierCombo = new QComboBox(filterCard);
    m_supplierCombo->setMinimumWidth(150);
    connect(m_supplierCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProductsWidget::onSearchChanged);
    filterLayout->addWidget(m_supplierCombo);

    m_lowStockCheck = new QCheckBox("Low Stock Only", filterCard);
    connect(m_lowStockCheck, &QCheckBox::toggled, this, &ProductsWidget::onSearchChanged);
    filterLayout->addWidget(m_lowStockCheck);

    filterLayout->addStretch();

    auto addBtn = new QPushButton("+ Add Product", filterCard);
    addBtn->setProperty("class", "primaryBtn");
    connect(addBtn, &QPushButton::clicked, this, &ProductsWidget::onAddClicked);
    filterLayout->addWidget(addBtn);

    auto editBtn = new QPushButton("Edit", filterCard);
    editBtn->setProperty("class", "secondaryBtn");
    connect(editBtn, &QPushButton::clicked, this, &ProductsWidget::onEditClicked);
    filterLayout->addWidget(editBtn);

    auto delBtn = new QPushButton("Delete", filterCard);
    delBtn->setProperty("class", "dangerBtn");
    connect(delBtn, &QPushButton::clicked, this, &ProductsWidget::onDeleteClicked);
    filterLayout->addWidget(delBtn);

    auto refreshBtn = new QPushButton("Refresh", filterCard);
    refreshBtn->setProperty("class", "secondaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &ProductsWidget::refresh);
    filterLayout->addWidget(refreshBtn);

    mainLayout->addWidget(filterCard);

    // 2. Count label
    m_resultCountLabel = new QLabel(this);
    m_resultCountLabel->setStyleSheet("color: #64748b; font-weight: 500; font-size: 12px;");
    mainLayout->addWidget(m_resultCountLabel);

    // 3. Products Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(9);
    m_table->setHorizontalHeaderLabels({
        "ID", "Product Name", "Category", "Purchase Price", "Selling Price",
        "Stock Qty", "Min Stock", "Supplier", "Status"
    });

    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);

    connect(m_table, &QTableWidget::itemDoubleClicked, this, &ProductsWidget::onItemDoubleClicked);

    mainLayout->addWidget(m_table);
}

void ProductsWidget::loadFilterCombos() {
    m_categoryCombo->blockSignals(true);
    m_categoryCombo->clear();
    m_categoryCombo->addItem("All Categories", 0);
    auto cats = m_categoryRepo.getAll();
    for (const auto& c : cats) {
        m_categoryCombo->addItem(c.name, c.id);
    }
    m_categoryCombo->blockSignals(false);

    m_supplierCombo->blockSignals(true);
    m_supplierCombo->clear();
    m_supplierCombo->addItem("All Suppliers", 0);
    auto sups = m_supplierRepo.getAll();
    for (const auto& s : sups) {
        m_supplierCombo->addItem(s.name, s.id);
    }
    m_supplierCombo->blockSignals(false);
}

void ProductsWidget::onSearchChanged() {
    QString search = m_searchEdit->text();
    int catId = m_categoryCombo->currentData().toInt();
    int supId = m_supplierCombo->currentData().toInt();
    bool lowStockOnly = m_lowStockCheck->isChecked();

    m_currentProducts = m_productService.getFilteredProducts(search, catId, supId, lowStockOnly);
    renderTable(m_currentProducts);
}

void ProductsWidget::refresh() {
    loadFilterCombos();
    onSearchChanged();
}

void ProductsWidget::renderTable(const std::vector<Product>& products) {
    m_table->setRowCount(static_cast<int>(products.size()));
    m_resultCountLabel->setText(QString("Displaying %1 product(s)").arg(products.size()));

    for (int i = 0; i < static_cast<int>(products.size()); ++i) {
        const auto& p = products[i];

        auto idItem = new QTableWidgetItem(QString::number(p.id));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, idItem);

        auto nameItem = new QTableWidgetItem(p.name);
        nameItem->setToolTip(p.description.isEmpty() ? p.name : p.description);
        m_table->setItem(i, 1, nameItem);

        m_table->setItem(i, 2, new QTableWidgetItem(p.categoryName));

        auto buyItem = new QTableWidgetItem(QString("$ %1").arg(p.purchasePrice, 0, 'f', 2));
        buyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table->setItem(i, 3, buyItem);

        auto sellItem = new QTableWidgetItem(QString("$ %1").arg(p.sellingPrice, 0, 'f', 2));
        sellItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table->setItem(i, 4, sellItem);

        auto qtyItem = new QTableWidgetItem(QString::number(p.quantity));
        qtyItem->setTextAlignment(Qt::AlignCenter);
        QFont f = qtyItem->font();
        f.setBold(true);
        qtyItem->setFont(f);
        m_table->setItem(i, 5, qtyItem);

        auto minItem = new QTableWidgetItem(QString::number(p.minimumStock));
        minItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 6, minItem);

        m_table->setItem(i, 7, new QTableWidgetItem(p.supplierName));

        // Badge Status
        auto statusWidget = new QWidget();
        auto badgeLayout = new QHBoxLayout(statusWidget);
        badgeLayout->setContentsMargins(4, 2, 4, 2);
        badgeLayout->setAlignment(Qt::AlignCenter);

        auto badgeLabel = new QLabel(statusWidget);
        if (p.isOutOfStock()) {
            badgeLabel->setText("Out of Stock");
            badgeLabel->setProperty("class", "badgeOutStock");
        } else if (p.isLowStock()) {
            badgeLabel->setText("Low Stock");
            badgeLabel->setProperty("class", "badgeLowStock");
        } else {
            badgeLabel->setText("In Stock");
            badgeLabel->setProperty("class", "badgeInStock");
        }
        badgeLayout->addWidget(badgeLabel);
        m_table->setCellWidget(i, 8, statusWidget);
    }
}

void ProductsWidget::showAddDialog() {
    onAddClicked();
}

void ProductsWidget::onAddClicked() {
    ProductDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Product p = dlg.getProduct();
        QString err;
        if (m_productService.createProduct(p, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Product registered successfully!");
        } else {
            QMessageBox::critical(this, "Error", err);
        }
    }
}

void ProductsWidget::onEditClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= static_cast<int>(m_currentProducts.size())) {
        QMessageBox::warning(this, "Selection Required", "Please select a product row to edit.");
        return;
    }

    const auto& p = m_currentProducts[row];
    ProductDialog dlg(this, &p);
    if (dlg.exec() == QDialog::Accepted) {
        Product updated = dlg.getProduct();
        QString err;
        if (m_productService.updateProduct(updated, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Product updated successfully!");
        } else {
            QMessageBox::critical(this, "Error", err);
        }
    }
}

void ProductsWidget::onItemDoubleClicked(QTableWidgetItem* item) {
    Q_UNUSED(item);
    onEditClicked();
}

void ProductsWidget::onDeleteClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= static_cast<int>(m_currentProducts.size())) {
        QMessageBox::warning(this, "Selection Required", "Please select a product row to delete.");
        return;
    }

    const auto& p = m_currentProducts[row];

    auto reply = QMessageBox::question(
        this, "Confirm Deletion",
        QString("Are you sure you want to delete product '%1' (ID: %2)?\n\nThis action cannot be undone.")
            .arg(p.name)
            .arg(p.id),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        QString err;
        if (m_productService.deleteProduct(p.id, err)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Product deleted successfully.");
        } else {
            QMessageBox::critical(this, "Deletion Prevented", err);
        }
    }
}
