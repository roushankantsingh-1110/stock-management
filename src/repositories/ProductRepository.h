#pragma once

#include "../models/Product.h"
#include <vector>
#include <optional>
#include <QString>

class ProductRepository {
public:
    ProductRepository() = default;

    std::vector<Product> getAll();
    std::optional<Product> getById(int id);
    std::vector<Product> getFiltered(const QString& search, int categoryId, int supplierId, bool lowStockOnly);
    std::vector<Product> getLowStock();
    std::vector<Product> getBySupplier(int supplierId);

    bool add(const Product& product, int* outId = nullptr);
    bool update(const Product& product);
    bool updateQuantity(int productId, int newQuantity);
    bool remove(int id);
    bool hasTransactions(int productId);

    int getTotalCount();
    int getTotalStockUnits();
    double getTotalInventoryValue();
    int getLowStockCount();
};
