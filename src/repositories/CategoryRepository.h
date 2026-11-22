#pragma once

#include "../models/Category.h"
#include <vector>
#include <optional>
#include <QString>

class CategoryRepository {
public:
    CategoryRepository() = default;

    std::vector<Category> getAll();
    std::optional<Category> getById(int id);
    std::optional<Category> getByName(const QString& name);
    bool add(const Category& category, int* outId = nullptr);
    bool update(const Category& category);
    bool remove(int id);
    bool isUsedByProducts(int categoryId);
};
