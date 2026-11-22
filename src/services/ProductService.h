#pragma once

#include "../models/Product.h"
#include "../repositories/ProductRepository.h"
#include "../repositories/CategoryRepository.h"
#include "../repositories/SupplierRepository.h"
#include <vector>
#include <optional>
#include <QString>

class ProductService {
public:
    ProductService();

    std::vector<Product> getAllProducts();
    std::optional<Product> getProductById(int id);
    std::vector<Product> getFilteredProducts(const QString& search, int categoryId, int supplierId, bool lowStockOnly);
    std::vector<Product> getLowStockProducts();

    bool createProduct(const Product& product, QString& outError);
    bool updateProduct(const Product& product, QString& outError);
    bool deleteProduct(int id, QString& outError);

    int getTotalProductCount();
    int getTotalStockUnits();
    double getTotalInventoryValue();
    int getLowStockCount();

private:
    ProductRepository m_productRepo;
    CategoryRepository m_categoryRepo;
    SupplierRepository m_supplierRepo;

    bool validate(const Product& p, QString& outError);
};
