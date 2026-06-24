#include "Table.h"
#include "DatabaseManager.h"
#include <QSqlQuery>

Table::Table(int id, int number, int capacity, bool isAvailable)
    : m_id(id), m_number(number), m_capacity(capacity), m_isAvailable(isAvailable) {}

bool Table::save() {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1) {
        query.prepare("INSERT INTO Tables (TableNumber, Capacity, IsAvailable) VALUES (?, ?, ?)");
        query.addBindValue(m_number);
        query.addBindValue(m_capacity);
        query.addBindValue(m_isAvailable);

        if (!query.exec()) {
            qDebug() << "Помилка збереження столика:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();
    } else {
        query.prepare("UPDATE Tables SET TableNumber=?, Capacity=?, IsAvailable=? WHERE ID_Table=?");
        query.addBindValue(m_number);
        query.addBindValue(m_capacity);
        query.addBindValue(m_isAvailable);
        query.addBindValue(m_id);

        if (!query.exec()) {
            qDebug() << "Помилка оновлення столика:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool Table::load(int id) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Tables WHERE ID_Table = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    m_id = query.value("ID_Table").toInt();
    m_number = query.value("TableNumber").toInt();
    m_capacity = query.value("Capacity").toInt();
    m_isAvailable = query.value("IsAvailable").toBool();

    return true;
}

bool Table::remove() {
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM Tables WHERE ID_Table = ?");
    query.addBindValue(m_id);

    return query.exec();
}

QString Table::toString() const {
    return QString("Столик №%1 (%2 місць) - %3")
        .arg(m_number)
        .arg(m_capacity)
        .arg(m_isAvailable ? "Вільний" : "Зайнятий");
}

QList<Table> Table::getAllTables() {
    QList<Table> tables;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT * FROM Tables ORDER BY TableNumber");

    while (query.next()) {
        Table table;
        table.m_id = query.value("ID_Table").toInt();
        table.m_number = query.value("TableNumber").toInt();
        table.m_capacity = query.value("Capacity").toInt();
        table.m_isAvailable = query.value("IsAvailable").toBool();
        tables.append(table);
    }
    return tables;
}

QList<Table> Table::getAvailableTables() {
    QList<Table> tables;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT * FROM Tables WHERE IsAvailable = 1 ORDER BY TableNumber");
    query.exec();

    while (query.next()) {
        Table table;
        table.m_id = query.value("ID_Table").toInt();
        table.m_number = query.value("TableNumber").toInt();
        table.m_capacity = query.value("Capacity").toInt();
        table.m_isAvailable = query.value("IsAvailable").toBool();
        tables.append(table);
    }
    return tables;
}

Table Table::getById(int id) {
    Table table;
    table.load(id);
    return table;
}