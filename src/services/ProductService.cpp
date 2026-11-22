#include "ProductService.h"

ProductService::ProductService() = default;

std::vector<Product> ProductService::getAllProducts() {
    return m_productRepo.getAll();
}

std::optional<Product> ProductService::getProductById(int id) {
    return m_productRepo.getById(id);
}

std::vector<Product> ProductService::getFilteredProducts(const QString& search, int categoryId, int supplierId, bool lowStockOnly) {
    return m_productRepo.getFiltered(search, categoryId, supplierId, lowStockOnly);
}

std::vector<Product> ProductService::getLowStockProducts() {
    return m_productRepo.getLowStock();
}

bool ProductService::validate(const Product& p, QString& outError) {
    if (p.name.trimmed().isEmpty()) {
        outError = "Product name cannot be empty.";
        return false;
    }
    if (p.categoryId <= 0) {
        outError = "A category must be selected.";
        return false;
    }
    if (p.purchasePrice < 0.0) {
        outError = "Purchase price cannot be negative.";
        return false;
    }
    if (p.sellingPrice < 0.0) {
        outError = "Selling price cannot be negative.";
        return false;
    }
    if (p.quantity < 0) {
        outError = "Quantity cannot be negative.";
        return false;
    }
    if (p.minimumStock < 0) {
        outError = "Minimum stock threshold cannot be negative.";
        return false;
    }
    return true;
}

bool ProductService::createProduct(const Product& product, QString& outError) {
    if (!validate(product, outError)) {
        return false;
    }
    if (!m_productRepo.add(product)) {
        outError = "Failed to add product to database.";
        return false;
    }
    return true;
}

bool ProductService::updateProduct(const Product& product, QString& outError) {
    if (product.id <= 0) {
        outError = "Invalid product ID.";
        return false;
    }
    if (!validate(product, outError)) {
        return false;
    }
    if (!m_productRepo.update(product)) {
        outError = "Failed to update product details.";
        return false;
    }
    return true;
}

bool ProductService::deleteProduct(int id, QString& outError) {
    if (id <= 0) {
        outError = "Invalid product ID.";
        return false;
    }
    if (m_productRepo.hasTransactions(id)) {
        outError = "Cannot delete product because stock transactions exist for this product. Deleting it would compromise transaction audit history.";
        return false;
    }
    if (!m_productRepo.remove(id)) {
        outError = "Database error while deleting product.";
        return false;
    }
    return true;
}

int ProductService::getTotalProductCount() {
    return m_productRepo.getTotalCount();
}

int ProductService::getTotalStockUnits() {
    return m_productRepo.getTotalStockUnits();
}

double ProductService::getTotalInventoryValue() {
    return m_productRepo.getTotalInventoryValue();
}

int ProductService::getLowStockCount() {
    return m_productRepo.getLowStockCount();
}
