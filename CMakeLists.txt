cmake_minimum_required(VERSION 3.20)

project(StockPilot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Default to installed Qt 6 if CMAKE_PREFIX_PATH is not provided
if(NOT DEFINED CMAKE_PREFIX_PATH AND EXISTS "C:/Qt/6.8.2/msvc2022_64")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/msvc2022_64")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

set(SOURCES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/repositories/CategoryRepository.cpp
    src/repositories/SupplierRepository.cpp
    src/repositories/ProductRepository.cpp
    src/repositories/TransactionRepository.cpp
    src/services/ProductService.cpp
    src/services/SupplierService.cpp
    src/services/StockService.cpp
    src/services/ReportService.cpp
    src/ui/MainWindow.cpp
    src/ui/DashboardWidget.cpp
    src/ui/ProductsWidget.cpp
    src/ui/ProductDialog.cpp
    src/ui/SuppliersWidget.cpp
    src/ui/SupplierDialog.cpp
    src/ui/StockWidget.cpp
    src/ui/CategoriesWidget.cpp
    src/ui/ReportsWidget.cpp
)

set(HEADERS
    src/database/DatabaseManager.h
    src/models/Category.h
    src/models/Supplier.h
    src/models/Product.h
    src/models/StockTransaction.h
    src/repositories/CategoryRepository.h
    src/repositories/SupplierRepository.h
    src/repositories/ProductRepository.h
    src/repositories/TransactionRepository.h
    src/services/ProductService.h
    src/services/SupplierService.h
    src/services/StockService.h
    src/services/ReportService.h
    src/ui/MainWindow.h
    src/ui/DashboardWidget.h
    src/ui/ProductsWidget.h
    src/ui/ProductDialog.h
    src/ui/SuppliersWidget.h
    src/ui/SupplierDialog.h
    src/ui/StockWidget.h
    src/ui/CategoriesWidget.h
    src/ui/ReportsWidget.h
)

set(RESOURCES
    resources/resources.qrc
)

add_executable(StockPilot WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_include_directories(StockPilot PRIVATE src)

target_link_libraries(StockPilot PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

if(MSVC)
    target_compile_options(StockPilot PRIVATE /W4 /permissive-)
endif()

# Automated verification test suite
add_executable(TestBackend
    tests/test_backend.cpp
    src/database/DatabaseManager.cpp
    src/repositories/CategoryRepository.cpp
    src/repositories/SupplierRepository.cpp
    src/repositories/ProductRepository.cpp
    src/repositories/TransactionRepository.cpp
    src/services/ProductService.cpp
    src/services/SupplierService.cpp
    src/services/StockService.cpp
    src/services/ReportService.cpp
)

target_include_directories(TestBackend PRIVATE src)
target_link_libraries(TestBackend PRIVATE
    Qt6::Core
    Qt6::Sql
)

if(MSVC)
    target_compile_options(TestBackend PRIVATE /W4 /permissive-)
endif()
