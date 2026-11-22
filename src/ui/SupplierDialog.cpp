#include "SupplierDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QRegularExpression>

SupplierDialog::SupplierDialog(QWidget* parent, const Supplier* existing)
    : QDialog(parent), m_isEdit(existing != nullptr), m_supplierId(existing ? existing->id : 0) {
    setWindowTitle(m_isEdit ? "Edit Supplier" : "Add Supplier");
    resize(440, 420);
    setupUI();

    if (existing) {
        m_nameEdit->setText(existing->name);
        m_phoneEdit->setText(existing->phone);
        m_emailEdit->setText(existing->email);
        m_addressEdit->setPlainText(existing->address);
    }
}

void SupplierDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    auto headerLabel = new QLabel(m_isEdit ? "Edit Supplier Details" : "Register New Supplier", this);
    headerLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a;");
    mainLayout->addWidget(headerLabel);

    auto formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("e.g. Acme Distribution Corp");
    formLayout->addRow("Company / Name *:", m_nameEdit);

    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setPlaceholderText("+1-555-0199");
    formLayout->addRow("Phone Number:", m_phoneEdit);

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText("orders@acme.com");
    formLayout->addRow("Email Address:", m_emailEdit);

    m_addressEdit = new QTextEdit(this);
    m_addressEdit->setMaximumHeight(70);
    m_addressEdit->setPlaceholderText("Physical / Billing address...");
    formLayout->addRow("Address:", m_addressEdit);

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

    auto saveBtn = new QPushButton(m_isEdit ? "Save Changes" : "Save Supplier", this);
    saveBtn->setProperty("class", "primaryBtn");
    connect(saveBtn, &QPushButton::clicked, this, &SupplierDialog::onSaveClicked);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);
}

void SupplierDialog::onSaveClicked() {
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        m_errorLabel->setText("Supplier name is required.");
        m_errorLabel->setVisible(true);
        m_nameEdit->setFocus();
        return;
    }

    QString email = m_emailEdit->text().trimmed();
    if (!email.isEmpty()) {
        static QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        if (!emailRegex.match(email).hasMatch()) {
            m_errorLabel->setText("Please enter a valid email format.");
            m_errorLabel->setVisible(true);
            m_emailEdit->setFocus();
            return;
        }
    }

    m_resultSupplier.id = m_supplierId;
    m_resultSupplier.name = name;
    m_resultSupplier.phone = m_phoneEdit->text().trimmed();
    m_resultSupplier.email = email;
    m_resultSupplier.address = m_addressEdit->toPlainText().trimmed();

    accept();
}

Supplier SupplierDialog::getSupplier() const {
    return m_resultSupplier;
}
