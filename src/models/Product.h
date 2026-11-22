#pragma once
#include <QString>

struct Product {
    int id = 0;
    QString name;
    QString description;
    int categoryId = 0;
    QString categoryName;
    int supplierId = 0;
    QString supplierName;
    double purchasePrice = 0.0;
    double sellingPrice = 0.0;
    int quantity = 0;
    int minimumStock = 5;
    QString createdAt;
    QString updatedAt;

    bool isLowStock() const { return quantity <= minimumStock; }
    bool isOutOfStock() const { return quantity <= 0; }
};
