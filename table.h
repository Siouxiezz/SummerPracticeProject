#ifndef TABLE_H
#define TABLE_H

#include "Entity.h"
#include <QString>
#include <QList>

class Table : public Entity {
public:
    Table(int id = -1, int number = 0, int capacity = 4, bool isAvailable = true);

    int getId() const override { return m_id; }
    int getNumber() const { return m_number; }
    int getCapacity() const { return m_capacity; }
    bool isAvailable() const { return m_isAvailable; }

    void setId(int id) { m_id = id; }
    void setNumber(int number) { m_number = number; }
    void setCapacity(int capacity) { m_capacity = capacity; }
    void setAvailable(bool available) { m_isAvailable = available; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    static QList<Table> getAllTables();
    static QList<Table> getAvailableTables();
    static Table getById(int id);

private:
    int m_id;
    int m_number;
    int m_capacity;
    bool m_isAvailable;
};

#endif // TABLE_H