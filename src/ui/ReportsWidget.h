#pragma once

#include <QWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "../services/ReportService.h"
#include "../repositories/SupplierRepository.h"

class ReportsWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReportsWidget(QWidget* parent = nullptr);

    void refresh();

private slots:
    void onReportTypeChanged(int index);
    void onFilterChanged();
    void onExportCsvClicked();

private:
    void setupUI();
    void generateReport();

    ReportService m_reportService;
    SupplierRepository m_supplierRepo;

    QComboBox* m_reportTypeCombo;
    QWidget* m_dateFilterContainer;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QWidget* m_supplierFilterContainer;
    QComboBox* m_supplierCombo;

    QLabel* m_summaryLabel;
    QTableWidget* m_table;
};
