#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include "../models/Product.h"
#include "../services/ProductService.h"
#include "../repositories/CategoryRepository.h"
#include "../repositories/SupplierRepository.h"

class ProductDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProductDialog(QWidget* parent = nullptr, const Product* existingProduct = nullptr);
    Product getProduct() const;

private slots:
    void onSaveClicked();

private:
    void setupUI();
    void loadCombos();

    bool m_isEdit;
    int m_productId;
    QString m_createdAt;

    QLineEdit* m_nameEdit;
    QTextEdit* m_descEdit;
    QComboBox* m_categoryCombo;
    QComboBox* m_supplierCombo;
    QDoubleSpinBox* m_purchasePriceSpin;
    QDoubleSpinBox* m_sellingPriceSpin;
    QSpinBox* m_quantitySpin;
    QSpinBox* m_minStockSpin;
    QLabel* m_errorLabel;

    Product m_resultProduct;
    CategoryRepository m_categoryRepo;
    SupplierRepository m_supplierRepo;
};
