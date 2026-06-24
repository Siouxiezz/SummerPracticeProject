#include "User.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QVariant>

User::User(int id, const QString& username, const QString& password,
           const QString& name, const QString& phone, const QString& role)
    : m_id(id), m_username(username), m_password(password),
    m_name(name), m_phone(phone), m_role(role) {}

bool User::save() {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1) {
        query.prepare("INSERT INTO Users (Username, Password, Name, Phone, Role) "
                      "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(m_username);
        query.addBindValue(m_password);
        query.addBindValue(m_name);
        query.addBindValue(m_phone);
        query.addBindValue(m_role);

        if (!query.exec()) {
            qDebug() << "Помилка збереження користувача:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();
    } else {
        query.prepare("UPDATE Users SET Username=?, Password=?, Name=?, Phone=?, Role=? "
                      "WHERE ID_User=?");
        query.addBindValue(m_username);
        query.addBindValue(m_password);
        query.addBindValue(m_name);
        query.addBindValue(m_phone);
        query.addBindValue(m_role);
        query.addBindValue(m_id);

        if (!query.exec()) {
            qDebug() << "Помилка оновлення користувача:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool User::load(int id) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE ID_User = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    m_id = query.value("ID_User").toInt();
    m_username = query.value("Username").toString();
    m_password = query.value("Password").toString();
    m_name = query.value("Name").toString();
    m_phone = query.value("Phone").toString();
    m_role = query.value("Role").toString();

    return true;
}

bool User::remove() {
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM Users WHERE ID_User = ?");
    query.addBindValue(m_id);

    return query.exec();
}

QString User::toString() const {
    return QString("%1 (%2) - %3").arg(m_name).arg(m_username).arg(m_role);
}

User User::authenticate(const QString& username, const QString& password) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE Username = ? AND Password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec() || !query.next()) {
        return User();
    }

    User user;
    user.m_id = query.value("ID_User").toInt();
    user.m_username = query.value("Username").toString();
    user.m_password = query.value("Password").toString();
    user.m_name = query.value("Name").toString();
    user.m_phone = query.value("Phone").toString();
    user.m_role = query.value("Role").toString();

    return user;
}

User User::getByUsername(const QString& username) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE Username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        return User();
    }

    User user;
    user.m_id = query.value("ID_User").toInt();
    user.m_username = query.value("Username").toString();
    user.m_password = query.value("Password").toString();
    user.m_name = query.value("Name").toString();
    user.m_phone = query.value("Phone").toString();
    user.m_role = query.value("Role").toString();

    return user;
}

User User::getById(int id) {
    User user;
    user.load(id);
    return user;
}

QList<User> User::getAllUsers() {
    QList<User> users;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT * FROM Users ORDER BY Name");

    while (query.next()) {
        User user;
        user.m_id = query.value("ID_User").toInt();
        user.m_username = query.value("Username").toString();
        user.m_password = query.value("Password").toString();
        user.m_name = query.value("Name").toString();
        user.m_phone = query.value("Phone").toString();
        user.m_role = query.value("Role").toString();
        users.append(user);
    }
    return users;
}

QList<User> User::getByRole(const QString& role) {
    QList<User> users;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE Role = ? ORDER BY Name");
    query.addBindValue(role);
    query.exec();

    while (query.next()) {
        User user;
        user.m_id = query.value("ID_User").toInt();
        user.m_username = query.value("Username").toString();
        user.m_password = query.value("Password").toString();
        user.m_name = query.value("Name").toString();
        user.m_phone = query.value("Phone").toString();
        user.m_role = query.value("Role").toString();
        users.append(user);
    }
    return users;
}

bool compareUsersById(const User& a, const User& b) {
    return a.m_id < b.m_id;
}

bool compareUsersByName(const User& a, const User& b) {
    return a.m_name < b.m_name;
}