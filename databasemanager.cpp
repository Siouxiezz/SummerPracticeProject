#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::openDatabase(const QString& path) {
    db.setDatabaseName("restaurantDB.db");
    if (!db.open()) {
        lastError = db.lastError().text();
        qDebug() << "Помилка відкриття БД:" << lastError;
        QMessageBox::critical(nullptr, "Помилка", "Не вдалося відкрити базу даних!\n");
        return false;
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::executeQuery(const QString& query) {
    QSqlQuery sqlQuery;
    if (!sqlQuery.exec(query)) {
        lastError = sqlQuery.lastError().text();
        qDebug() << "Помилка виконання запиту: " << lastError;
        return false;
    }
    return true;
}

QSqlQuery DatabaseManager::executeQueryWithResult(const QString& query) {
    QSqlQuery sqlQuery;
    sqlQuery.exec(query);
    return sqlQuery;
}

QString DatabaseManager::getLastError() const {
    return lastError;
}