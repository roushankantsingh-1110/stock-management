#include "SupplierService.h"
#include <QRegularExpression>

SupplierService::SupplierService() = default;

std::vector<Supplier> SupplierService::getAllSuppliers() {
    return m_supplierRepo.getAll();
}

std::optional<Supplier> SupplierService::getSupplierById(int id) {
    return m_supplierRepo.getById(id);
}

std::vector<Supplier> SupplierService::searchSuppliers(const QString& keyword) {
    return m_supplierRepo.search(keyword);
}

std::vector<Product> SupplierService::getProductsBySupplier(int supplierId) {
    return m_productRepo.getBySupplier(supplierId);
}

bool SupplierService::validate(const Supplier& s, QString& outError) {
    if (s.name.trimmed().isEmpty()) {
        outError = "Supplier name is required.";
        return false;
    }
    if (!s.email.trimmed().isEmpty()) {
        static QRegularExpression emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        if (!emailRegex.match(s.email.trimmed()).hasMatch()) {
            outError = "Please enter a valid email address (e.g. name@company.com).";
            return false;
        }
    }
    return true;
}

bool SupplierService::createSupplier(const Supplier& supplier, QString& outError) {
    if (!validate(supplier, outError)) {
        return false;
    }
    if (!m_supplierRepo.add(supplier)) {
        outError = "Failed to add supplier to database.";
        return false;
    }
    return true;
}

bool SupplierService::updateSupplier(const Supplier& supplier, QString& outError) {
    if (supplier.id <= 0) {
        outError = "Invalid supplier ID.";
        return false;
    }
    if (!validate(supplier, outError)) {
        return false;
    }
    if (!m_supplierRepo.update(supplier)) {
        outError = "Failed to update supplier record.";
        return false;
    }
    return true;
}

bool SupplierService::deleteSupplier(int id, QString& outError) {
    if (id <= 0) {
        outError = "Invalid supplier ID.";
        return false;
    }
    if (m_supplierRepo.hasProducts(id)) {
        outError = "Cannot delete supplier because products are currently associated with this supplier.";
        return false;
    }
    if (!m_supplierRepo.remove(id)) {
        outError = "Database error while deleting supplier.";
        return false;
    }
    return true;
}
