#ifndef USER_H
#define USER_H

#include "Entity.h"
#include <QString>
#include <QList>

class User : public Entity {
public:
    User(int id = -1, const QString& username = "", const QString& password = "",
         const QString& name = "", const QString& phone = "", const QString& role = "Client");

    int getId() const override { return m_id; }
    QString getUsername() const { return m_username; }
    QString getPassword() const { return m_password; }
    QString getName() const { return m_name; }
    QString getPhone() const { return m_phone; }
    QString getRole() const { return m_role; }

    void setId(int id) { m_id = id; }
    void setUsername(const QString& username) { m_username = username; }
    void setPassword(const QString& password) { m_password = password; }
    void setName(const QString& name) { m_name = name; }
    void setPhone(const QString& phone) { m_phone = phone; }
    void setRole(const QString& role) { m_role = role; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    static User authenticate(const QString& username, const QString& password);
    static User getByUsername(const QString& username);
    static User getById(int id);
    static QList<User> getAllUsers();
    static QList<User> getByRole(const QString& role);

    friend bool compareUsersById(const User& a, const User& b);
    friend bool compareUsersByName(const User& a, const User& b);

private:
    int m_id;
    QString m_username;
    QString m_password;
    QString m_name;
    QString m_phone;
    QString m_role;
};

#endif // USER_H