#include "ProductDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>

ProductDialog::ProductDialog(QWidget* parent, const Product* existing)
    : QDialog(parent), m_isEdit(existing != nullptr), m_productId(existing ? existing->id : 0),
      m_createdAt(existing ? existing->createdAt : "") {
    setWindowTitle(m_isEdit ? "Edit Product" : "Add New Product");
    resize(480, 520);
    setupUI();
    loadCombos();

    if (existing) {
        m_nameEdit->setText(existing->name);
        m_descEdit->setPlainText(existing->description);
        m_purchasePriceSpin->setValue(existing->purchasePrice);
        m_sellingPriceSpin->setValue(existing->sellingPrice);
        m_quantitySpin->setValue(existing->quantity);
        m_minStockSpin->setValue(existing->minimumStock);

        int catIdx = m_categoryCombo->findData(existing->categoryId);
        if (catIdx >= 0) m_categoryCombo->setCurrentIndex(catIdx);

        int supIdx = m_supplierCombo->findData(existing->supplierId);
        if (supIdx >= 0) m_supplierCombo->setCurrentIndex(supIdx);
    }
}

void ProductDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    auto headerLabel = new QLabel(m_isEdit ? "Edit Product Details" : "Register New Product", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a;");
    mainLayout->addWidget(headerLabel);

    auto formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("e.g. Logitech MX Master 3S");
    formLayout->addRow("Product Name *:", m_nameEdit);

    m_categoryCombo = new QComboBox(this);
    formLayout->addRow("Category *:", m_categoryCombo);

    m_supplierCombo = new QComboBox(this);
    formLayout->addRow("Supplier:", m_supplierCombo);

    m_purchasePriceSpin = new QDoubleSpinBox(this);
    m_purchasePriceSpin->setRange(0.0, 1000000.0);
    m_purchasePriceSpin->setDecimals(2);
    m_purchasePriceSpin->setPrefix("$ ");
    formLayout->addRow("Purchase Price ($) *:", m_purchasePriceSpin);

    m_sellingPriceSpin = new QDoubleSpinBox(this);
    m_sellingPriceSpin->setRange(0.0, 1000000.0);
    m_sellingPriceSpin->setDecimals(2);
    m_sellingPriceSpin->setPrefix("$ ");
    formLayout->addRow("Selling Price ($) *:", m_sellingPriceSpin);

    m_quantitySpin = new QSpinBox(this);
    m_quantitySpin->setRange(0, 1000000);
    formLayout->addRow("Current Stock Quantity *:", m_quantitySpin);

    m_minStockSpin = new QSpinBox(this);
    m_minStockSpin->setRange(0, 100000);
    m_minStockSpin->setValue(5);
    formLayout->addRow("Low Stock Threshold *:", m_minStockSpin);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setMaximumHeight(70);
    m_descEdit->setPlaceholderText("Optional product description, specifications, or model info...");
    formLayout->addRow("Description:", m_descEdit);

    mainLayout->addLayout(formLayout);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: #dc2626; font-weight: 500;");
    m_errorLabel->setVisible(false);
    mainLayout->addWidget(m_errorLabel);

    auto btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    auto cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setProperty("class", "secondaryBtn");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    auto saveBtn = new QPushButton(m_isEdit ? "Save Changes" : "Create Product", this);
    saveBtn->setProperty("class", "primaryBtn");
    connect(saveBtn, &QPushButton::clicked, this, &ProductDialog::onSaveClicked);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);
}

void ProductDialog::loadCombos() {
    m_categoryCombo->clear();
    auto categories = m_categoryRepo.getAll();
    for (const auto& c : categories) {
        m_categoryCombo->addItem(c.name, c.id);
    }

    m_supplierCombo->clear();
    m_supplierCombo->addItem("-- No Supplier --", 0);
    auto suppliers = m_supplierRepo.getAll();
    for (const auto& s : suppliers) {
        m_supplierCombo->addItem(s.name, s.id);
    }
}

void ProductDialog::onSaveClicked() {
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        m_errorLabel->setText("Product name cannot be empty.");
        m_errorLabel->setVisible(true);
        m_nameEdit->setFocus();
        return;
    }
    if (m_categoryCombo->count() == 0 || m_categoryCombo->currentData().toInt() <= 0) {
        m_errorLabel->setText("Please select a valid category.");
        m_errorLabel->setVisible(true);
        return;
    }

    m_resultProduct.id = m_productId;
    m_resultProduct.name = name;
    m_resultProduct.description = m_descEdit->toPlainText().trimmed();
    m_resultProduct.categoryId = m_categoryCombo->currentData().toInt();
    m_resultProduct.categoryName = m_categoryCombo->currentText();
    m_resultProduct.supplierId = m_supplierCombo->currentData().toInt();
    m_resultProduct.supplierName = m_supplierCombo->currentIndex() > 0 ? m_supplierCombo->currentText() : "N/A";
    m_resultProduct.purchasePrice = m_purchasePriceSpin->value();
    m_resultProduct.sellingPrice = m_sellingPriceSpin->value();
    m_resultProduct.quantity = m_quantitySpin->value();
    m_resultProduct.minimumStock = m_minStockSpin->value();
    m_resultProduct.createdAt = m_createdAt;

    accept();
}

Product ProductDialog::getProduct() const {
    return m_resultProduct;
}
