#include "Dish.h"
#include "DatabaseManager.h"
#include <QSqlQuery>

Dish::Dish(int id, const QString& name, const QString& description,
           const QString& category, double price, double weight,
           const QString& imagePath) : m_id(id), m_name(name), m_description(description), m_category(category), m_price(price), m_weight(weight),m_imagePath(imagePath)
{

}

bool Dish::save()
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1)
    {
        query.prepare("INSERT INTO Dishes (Name, Description, Category, Price, Weight, ImagePath) "
                      "VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(m_name);
        query.addBindValue(m_description);
        query.addBindValue(m_category);
        query.addBindValue(m_price);
        query.addBindValue(m_weight);
        query.addBindValue(m_imagePath);

        if (!query.exec())
        {
            qDebug() << "Помилка збереження страви:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();
    }
    else
    {
        query.prepare("UPDATE Dishes SET Name=?, Description=?, Category=?, Price=?, Weight=?, ImagePath=? "
                      "WHERE ID_Dish=?");
        query.addBindValue(m_name);
        query.addBindValue(m_description);
        query.addBindValue(m_category);
        query.addBindValue(m_price);
        query.addBindValue(m_weight);
        query.addBindValue(m_imagePath);
        query.addBindValue(m_id);

        if (!query.exec())
        {
            qDebug() << "Помилка оновлення страви:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool Dish::load(int id)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Dishes WHERE ID_Dish = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next())
    {
        return false;
    }

    m_id = query.value("ID_Dish").toInt();
    m_name = query.value("Name").toString();
    m_description = query.value("Description").toString();
    m_category = query.value("Category").toString();
    m_price = query.value("Price").toDouble();
    m_weight = query.value("Weight").toDouble();
    m_imagePath = query.value("ImagePath").toString();

    return true;
}

bool Dish::remove()
{
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM Dishes WHERE ID_Dish = ?");
    query.addBindValue(m_id);

    return query.exec();
}

QString Dish::toString() const
{
    return QString("%1 - %2 грн (%3 г)").arg(m_name).arg(m_price).arg(m_weight);
}

QList<Dish> Dish::getAllDishes()
{
    QList<Dish> dishes;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT * FROM Dishes ORDER BY Name");

    while (query.next())
    {
        Dish dish;
        dish.m_id = query.value("ID_Dish").toInt();
        dish.m_name = query.value("Name").toString();
        dish.m_description = query.value("Description").toString();
        dish.m_category = query.value("Category").toString();
        dish.m_price = query.value("Price").toDouble();
        dish.m_weight = query.value("Weight").toDouble();
        dish.m_imagePath = query.value("ImagePath").toString();
        dishes.append(dish);
    }
    return dishes;
}

QList<Dish> Dish::getDishesByCategory(const QString& category) {
    QList<Dish> dishes;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Dishes WHERE Category = ? ORDER BY Name");
    query.addBindValue(category);
    query.exec();

    while (query.next())
    {
        Dish dish;
        dish.m_id = query.value("ID_Dish").toInt();
        dish.m_name = query.value("Name").toString();
        dish.m_description = query.value("Description").toString();
        dish.m_category = query.value("Category").toString();
        dish.m_price = query.value("Price").toDouble();
        dish.m_weight = query.value("Weight").toDouble();
        dish.m_imagePath = query.value("ImagePath").toString();
        dishes.append(dish);
    }
    return dishes;
}

Dish Dish::getById(int id)
{
    Dish dish;
    dish.load(id);
    return dish;
}

bool compareDishesByPrice(const Dish& a, const Dish& b)
{
    return a.m_price < b.m_price;
}

bool compareDishesByName(const Dish& a, const Dish& b)
{
    return a.m_name < b.m_name;
}