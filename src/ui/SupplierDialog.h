#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include "../models/Supplier.h"

class SupplierDialog : public QDialog {
    Q_OBJECT
public:
    explicit SupplierDialog(QWidget* parent = nullptr, const Supplier* existing = nullptr);
    Supplier getSupplier() const;

private slots:
    void onSaveClicked();

private:
    void setupUI();

    bool m_isEdit;
    int m_supplierId;

    QLineEdit* m_nameEdit;
    QLineEdit* m_phoneEdit;
    QLineEdit* m_emailEdit;
    QTextEdit* m_addressEdit;
    QLabel* m_errorLabel;

    Supplier m_resultSupplier;
};
