#pragma once

#include "../models/Supplier.h"
#include <vector>
#include <optional>
#include <QString>

class SupplierRepository {
public:
    SupplierRepository() = default;

    std::vector<Supplier> getAll();
    std::optional<Supplier> getById(int id);
    std::vector<Supplier> search(const QString& keyword);
    bool add(const Supplier& supplier, int* outId = nullptr);
    bool update(const Supplier& supplier);
    bool remove(int id);
    bool hasProducts(int supplierId);
};
