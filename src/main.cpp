#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include "database/DatabaseManager.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("StockPilot");
    app.setOrganizationName("StockPilotOrg");
    app.setApplicationVersion("1.0.0");

    // Load stylesheet from Qt resource or local disk
    QFile styleFile(":/styles/style.qss");
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Fallback to local relative path
        styleFile.setFileName("resources/styles/style.qss");
        styleFile.open(QIODevice::ReadOnly | QIODevice::Text);
    }
    if (styleFile.isOpen()) {
        QString qss = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(qss);
        styleFile.close();
    }

    // Initialize SQLite Database
    if (!DatabaseManager::instance().initialize()) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Failed to initialize SQLite database.\n\n"
                              "The application cannot start without persistent local storage.");
        return 1;
    }

    MainWindow mainWindow;
    mainWindow.show();

    int result = app.exec();

    DatabaseManager::instance().close();
    return result;
}
