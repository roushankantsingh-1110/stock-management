#include "CategoriesWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFrame>

CategoriesWidget::CategoriesWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
    refresh();
}

void CategoriesWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 24);
    mainLayout->setSpacing(14);

    // 1. Add Category Bar
    auto actionCard = new QFrame(this);
    actionCard->setProperty("class", "card");
    auto actionLayout = new QHBoxLayout(actionCard);
    actionLayout->setContentsMargins(14, 12, 14, 12);
    actionLayout->setSpacing(10);

    auto lbl = new QLabel("New Category:", actionCard);
    lbl->setStyleSheet("font-weight: 600; color: #334155;");
    actionLayout->addWidget(lbl);

    m_nameEdit = new QLineEdit(actionCard);
    m_nameEdit->setPlaceholderText("Enter category name (e.g. Peripherals, Office Furniture)...");
    m_nameEdit->setMinimumWidth(260);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &CategoriesWidget::onAddClicked);
    actionLayout->addWidget(m_nameEdit);

    auto addBtn = new QPushButton("+ Add Category", actionCard);
    addBtn->setProperty("class", "primaryBtn");
    connect(addBtn, &QPushButton::clicked, this, &CategoriesWidget::onAddClicked);
    actionLayout->addWidget(addBtn);

    actionLayout->addStretch();

    auto renameBtn = new QPushButton("Rename Selected", actionCard);
    renameBtn->setProperty("class", "secondaryBtn");
    connect(renameBtn, &QPushButton::clicked, this, &CategoriesWidget::onRenameClicked);
    actionLayout->addWidget(renameBtn);

    auto delBtn = new QPushButton("Delete Selected", actionCard);
    delBtn->setProperty("class", "dangerBtn");
    connect(delBtn, &QPushButton::clicked, this, &CategoriesWidget::onDeleteClicked);
    actionLayout->addWidget(delBtn);

    mainLayout->addWidget(actionCard);

    // 2. Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Category ID", "Category Name", "Products in Category"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);

    mainLayout->addWidget(m_table);
}

void CategoriesWidget::refresh() {
    renderTable();
}

void CategoriesWidget::renderTable() {
    auto categories = m_categoryRepo.getAll();
    m_table->setRowCount(static_cast<int>(categories.size()));

    for (int i = 0; i < static_cast<int>(categories.size()); ++i) {
        const auto& c = categories[i];

        auto idItem = new QTableWidgetItem(QString::number(c.id));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, idItem);

        auto nameItem = new QTableWidgetItem(c.name);
        QFont f = nameItem->font();
        f.setBold(true);
        nameItem->setFont(f);
        m_table->setItem(i, 1, nameItem);

        auto prods = m_productService.getFilteredProducts("", c.id, 0, false);
        auto countItem = new QTableWidgetItem(QString("%1 product(s)").arg(prods.size()));
        countItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, countItem);
    }
}

void CategoriesWidget::onAddClicked() {
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter a category name.");
        m_nameEdit->setFocus();
        return;
    }

    if (m_categoryRepo.getByName(name).has_value()) {
        QMessageBox::warning(this, "Duplicate Category", "A category with this name already exists.");
        m_nameEdit->setFocus();
        return;
    }

    Category c;
    c.name = name;
    if (m_categoryRepo.add(c)) {
        m_nameEdit->clear();
        refresh();
        emit dataChanged();
        QMessageBox::information(this, "Success", "Category created successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to insert category into database.");
    }
}

void CategoriesWidget::onRenameClicked() {
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Required", "Please select a category row to rename.");
        return;
    }

    int catId = m_table->item(row, 0)->text().toInt();
    QString oldName = m_table->item(row, 1)->text();

    bool ok = false;
    QString newName = QInputDialog::getText(this, "Rename Category", "New category name:", QLineEdit::Normal, oldName, &ok);
    if (!ok || newName.trimmed().isEmpty()) return;

    newName = newName.trimmed();
    if (newName == oldName) return;

    if (m_categoryRepo.getByName(newName).has_value()) {
        QMessageBox::warning(this, "Duplicate Name", "Another category already exists with that name.");
        return;
    }

    Category c;
    c.id = catId;
    c.name = newName;
    if (m_categoryRepo.update(c)) {
        refresh();
        emit dataChanged();
        QMessageBox::information(this, "Success", "Category renamed successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update category name.");
    }
}

void CategoriesWidget::onDeleteClicked() {
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Required", "Please select a category row to delete.");
        return;
    }

    int catId = m_table->item(row, 0)->text().toInt();
    QString name = m_table->item(row, 1)->text();

    if (m_categoryRepo.isUsedByProducts(catId)) {
        QMessageBox::critical(this, "Deletion Prevented",
                              QString("Cannot delete category '%1' because products are currently assigned to it.")
                                  .arg(name));
        return;
    }

    auto reply = QMessageBox::question(
        this, "Confirm Deletion",
        QString("Are you sure you want to delete category '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_categoryRepo.remove(catId)) {
            refresh();
            emit dataChanged();
            QMessageBox::information(this, "Success", "Category deleted successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete category.");
        }
    }
}
