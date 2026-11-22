#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <memory>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initialize(const QString& customPath = QString());
    void close();
    bool isConnected() const;
    QString databasePath() const;
    QSqlDatabase database() const;

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    bool backupDatabase(const QString& destPath);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool seedSampleData();

    QString m_dbPath;
    bool m_connected;
    static const QString CONNECTION_NAME;
};
