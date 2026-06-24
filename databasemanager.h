#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool openDatabase(const QString& path = "restaurant.db");
    void closeDatabase();
    QSqlDatabase& getDatabase() { return db; }

    bool executeQuery(const QString& query);
    QSqlQuery executeQueryWithResult(const QString& query);
    QString getLastError() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    QString lastError;
};

#endif // DATABASEMANAGER_H