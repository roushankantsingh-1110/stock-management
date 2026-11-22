#include <iostream>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include "database/DatabaseManager.h"
#include "repositories/CategoryRepository.h"
#include "repositories/SupplierRepository.h"
#include "repositories/ProductRepository.h"
#include "repositories/TransactionRepository.h"
#include "services/ProductService.h"
#include "services/SupplierService.h"
#include "services/StockService.h"
#include "services/ReportService.h"

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAILED: " << msg << " at line " << __LINE__ << std::endl; \
            return 1; \
        } \
    } while (0)

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    std::cout << "==========================================\n";
    std::cout << "Starting StockPilot Automated Verification\n";
    std::cout << "==========================================\n";

    QString testDbPath = "test_stockpilot.db";
    if (QFile::exists(testDbPath)) {
        QFile::remove(testDbPath);
    }

    // 1. Test Database Initialization & Auto-Seeding
    std::cout << "[TEST 1] Testing DatabaseManager::initialize...\n";
    bool initOk = DatabaseManager::instance().initialize(testDbPath);
    CHECK(initOk, "Database initialization failed!");
    CHECK(DatabaseManager::instance().isConnected(), "Database should report connected!");
    std::cout << "  -> PASS: Database initialized and connected.\n";

    // 2. Test CategoryRepository
    std::cout << "[TEST 2] Testing CategoryRepository...\n";
    CategoryRepository catRepo;
    auto categories = catRepo.getAll();
    std::cout << "  -> Categories count: " << categories.size() << "\n";
    CHECK(categories.size() >= 4, "Expected at least 4 seeded categories!");

    Category newCat;
    newCat.name = "Test Peripherals";
    int newCatId = 0;
    CHECK(catRepo.add(newCat, &newCatId), "Failed to add category");
    CHECK(newCatId > 0, "Invalid inserted category id");
    CHECK(catRepo.getByName("Test Peripherals").has_value(), "Category lookup by name failed");
    std::cout << "  -> PASS: Category added and verified (ID: " << newCatId << ").\n";

    // 3. Test SupplierRepository
    std::cout << "[TEST 3] Testing SupplierRepository...\n";
    SupplierRepository supRepo;
    auto suppliers = supRepo.getAll();
    CHECK(suppliers.size() >= 3, "Expected at least 3 seeded suppliers!");
    
    Supplier newSup;
    newSup.name = "Apex Components";
    newSup.email = "info@apex.com";
    newSup.phone = "+1-555-8888";
    newSup.address = "123 Tech Park";
    int newSupId = 0;
    CHECK(supRepo.add(newSup, &newSupId), "Failed to add supplier");
    CHECK(newSupId > 0, "Invalid inserted supplier id");
    std::cout << "  -> PASS: Supplier added and verified (ID: " << newSupId << ").\n";

    // 4. Test ProductService CRUD
    std::cout << "[TEST 4] Testing ProductService CRUD...\n";
    ProductService prodService;
    int initialCount = prodService.getTotalProductCount();
    CHECK(initialCount >= 5, "Expected at least 5 seeded products!");

    Product p;
    p.name = "Ultra HD 4K Webcam";
    p.description = "60fps autofocus webcam with stereo mic";
    p.categoryId = newCatId;
    p.supplierId = newSupId;
    p.purchasePrice = 45.00;
    p.sellingPrice = 79.99;
    p.quantity = 20;
    p.minimumStock = 5;

    QString errMsg;
    CHECK(prodService.createProduct(p, errMsg), QString("Failed to create product: %1").arg(errMsg).toStdString());
    CHECK(prodService.getTotalProductCount() == initialCount + 1, "Count should increment");
    std::cout << "  -> PASS: Product created successfully. Total: " << prodService.getTotalProductCount() << "\n";

    auto filtered = prodService.getFilteredProducts("Webcam", 0, 0, false);
    CHECK(filtered.size() == 1, "Search filtering failed for product");
    int testProdId = filtered[0].id;
    std::cout << "  -> PASS: Filter search returned correct item (ID: " << testProdId << ").\n";

    // 5. Test StockService: Stock In
    std::cout << "[TEST 5] Testing Stock In...\n";
    StockService stockService;
    CHECK(stockService.stockIn(testProdId, 15, "Batch PO #9901", errMsg), errMsg.toStdString());
    auto checkP1 = prodService.getProductById(testProdId);
    CHECK(checkP1.has_value() && checkP1->quantity == 35, "Stock In did not update quantity to 35!");
    std::cout << "  -> PASS: Stock In increased inventory from 20 to 35 units.\n";

    // 6. Test StockService: Stock Out
    std::cout << "[TEST 6] Testing Stock Out...\n";
    CHECK(stockService.stockOut(testProdId, 10, "Sales order fulfillment", errMsg), errMsg.toStdString());
    auto checkP2 = prodService.getProductById(testProdId);
    CHECK(checkP2.has_value() && checkP2->quantity == 25, "Stock Out did not update quantity to 25!");
    std::cout << "  -> PASS: Stock Out decreased inventory from 35 to 25 units.\n";

    // 7. Test Overdraft Prevention (Stock Out > available quantity)
    std::cout << "[TEST 7] Testing Overdraft Prevention...\n";
    bool overdraftResult = stockService.stockOut(testProdId, 999, "Overdraft attempt", errMsg);
    CHECK(!overdraftResult, "Stock Out MUST reject overdraft!");
    auto checkP3 = prodService.getProductById(testProdId);
    CHECK(checkP3.has_value() && checkP3->quantity == 25, "Overdraft attempt must NOT alter quantity!");
    std::cout << "  -> PASS: Overdraft rejected correctly with message: \"" << errMsg.toStdString() << "\"\n";

    // 8. Test Negative Stock Input Validation
    std::cout << "[TEST 8] Testing Quantity Validation (<= 0)...\n";
    CHECK(!stockService.stockIn(testProdId, 0, "Zero test", errMsg), "Zero stock in must be rejected");
    CHECK(!stockService.stockOut(testProdId, -5, "Negative test", errMsg), "Negative stock out must be rejected");
    std::cout << "  -> PASS: Non-positive quantities properly rejected.\n";

    // 9. Test Transaction History Ledger
    std::cout << "[TEST 9] Testing Transaction History...\n";
    auto trans = stockService.getFilteredTransactions(testProdId, "ALL");
    CHECK(trans.size() >= 2, "Expected at least 2 transactions for this product!");
    std::cout << "  -> PASS: Found " << trans.size() << " transactions for product.\n";

    // 10. Test Deletion Constraint Protection
    std::cout << "[TEST 10] Testing Safe Deletion Protections...\n";
    bool delProdResult = prodService.deleteProduct(testProdId, errMsg);
    CHECK(!delProdResult, "Product with transactions must NOT be deleted without protection!");
    std::cout << "  -> PASS: Protected product from deletion due to existing transaction history.\n";

    // 11. Test Report Generation and CSV Export
    std::cout << "[TEST 11] Testing ReportService & CSV Export...\n";
    ReportService repService;
    auto summary = repService.getSummary();
    CHECK(summary.totalProducts >= 6 && summary.totalUnits > 0 && summary.totalValue > 0.0, "Invalid summary stats");
    std::cout << "  -> Valuation Summary: SKUs=" << summary.totalProducts 
              << ", Units=" << summary.totalUnits 
              << ", Value=$" << summary.totalValue << "\n";

    QString csvPath = "test_export.csv";
    QStringList headers = {"ID", "Name", "Qty", "Price"};
    std::vector<QStringList> rows = {
        {"1", "Mouse", "45", "9.50"},
        {"2", "Keyboard", "18", "55.00"}
    };
    CHECK(repService.exportToCsv(csvPath, headers, rows, errMsg), errMsg.toStdString());
    CHECK(QFile::exists(csvPath), "CSV file was not created on disk");
    QFile::remove(csvPath);
    std::cout << "  -> PASS: CSV export successful.\n";

    DatabaseManager::instance().close();
    QFile::remove(testDbPath);

    std::cout << "==========================================\n";
    std::cout << "ALL 11 AUTOMATED VERIFICATION TESTS PASSED!\n";
    std::cout << "==========================================\n";
    return 0;
}
