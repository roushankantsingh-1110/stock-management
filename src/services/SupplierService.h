#pragma once

#include "../models/Supplier.h"
#include "../models/Product.h"
#include "../repositories/SupplierRepository.h"
#include "../repositories/ProductRepository.h"
#include <vector>
#include <optional>
#include <QString>

class SupplierService {
public:
    SupplierService();

    std::vector<Supplier> getAllSuppliers();
    std::optional<Supplier> getSupplierById(int id);
    std::vector<Supplier> searchSuppliers(const QString& keyword);
    std::vector<Product> getProductsBySupplier(int supplierId);

    bool createSupplier(const Supplier& supplier, QString& outError);
    bool updateSupplier(const Supplier& supplier, QString& outError);
    bool deleteSupplier(int id, QString& outError);

private:
    SupplierRepository m_supplierRepo;
    ProductRepository m_productRepo;

    bool validate(const Supplier& s, QString& outError);
};
