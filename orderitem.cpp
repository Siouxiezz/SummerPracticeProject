#include "OrderItem.h"
#include "DatabaseManager.h"
#include "Dish.h"
#include <QSqlQuery>
#include <QVariant>

OrderItem::OrderItem(int id, int orderId, int dishId,
                     int quantity, double priceAtOrder)
    : m_id(id), m_orderId(orderId), m_dishId(dishId),
    m_quantity(quantity), m_priceAtOrder(priceAtOrder) {}

bool OrderItem::save() {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1) {
        query.prepare("INSERT INTO OrderItems (OrderID, DishID, Quantity, PriceAtOrder) "
                      "VALUES (?, ?, ?, ?)");
        query.addBindValue(m_orderId);
        query.addBindValue(m_dishId);
        query.addBindValue(m_quantity);
        query.addBindValue(m_priceAtOrder);

        if (!query.exec()) {
            qDebug() << "Помилка збереження позиції:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();
    } else {
        query.prepare("UPDATE OrderItems SET OrderID=?, DishID=?, Quantity=?, PriceAtOrder=? "
                      "WHERE ID_OrderItem=?");
        query.addBindValue(m_orderId);
        query.addBindValue(m_dishId);
        query.addBindValue(m_quantity);
        query.addBindValue(m_priceAtOrder);
        query.addBindValue(m_id);

        if (!query.exec()) {
            qDebug() << "Помилка оновлення позиції:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool OrderItem::load(int id) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM OrderItems WHERE ID_OrderItem = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    m_id = query.value("ID_OrderItem").toInt();
    m_orderId = query.value("OrderID").toInt();
    m_dishId = query.value("DishID").toInt();
    m_quantity = query.value("Quantity").toInt();
    m_priceAtOrder = query.value("PriceAtOrder").toDouble();

    return true;
}

bool OrderItem::remove() {
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM OrderItems WHERE ID_OrderItem = ?");
    query.addBindValue(m_id);

    return query.exec();
}

QString OrderItem::toString() const {
    Dish dish = Dish::getById(m_dishId);
    return QString("%1 ×%2 = %3 грн")
        .arg(dish.getName())
        .arg(m_quantity)
        .arg(m_quantity * m_priceAtOrder);
}

QList<OrderItem> OrderItem::getItemsByOrder(int orderId) {
    QList<OrderItem> items;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM OrderItems WHERE OrderID = ?");
    query.addBindValue(orderId);
    query.exec();

    while (query.next()) {
        OrderItem item;
        item.m_id = query.value("ID_OrderItem").toInt();
        item.m_orderId = query.value("OrderID").toInt();
        item.m_dishId = query.value("DishID").toInt();
        item.m_quantity = query.value("Quantity").toInt();
        item.m_priceAtOrder = query.value("PriceAtOrder").toDouble();
        items.append(item);
    }
    return items;
}

QList<OrderItem> OrderItem::getItemsByDish(int dishId) {
    QList<OrderItem> items;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM OrderItems WHERE DishID = ?");
    query.addBindValue(dishId);
    query.exec();

    while (query.next()) {
        OrderItem item;
        item.m_id = query.value("ID_OrderItem").toInt();
        item.m_orderId = query.value("OrderID").toInt();
        item.m_dishId = query.value("DishID").toInt();
        item.m_quantity = query.value("Quantity").toInt();
        item.m_priceAtOrder = query.value("PriceAtOrder").toDouble();
        items.append(item);
    }
    return items;
}

bool OrderItem::deleteItemsByOrder(int orderId) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM OrderItems WHERE OrderID = ?");
    query.addBindValue(orderId);
    return query.exec();
}

QList<OrderItem> OrderItem::getItemsWithDetails(int orderId) {
    QList<OrderItem> items;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query;
    query.prepare(
        "SELECT oi.ID_OrderItem, oi.Quantity, oi.PriceAtOrder, "
        "d.ID_Dish, d.Name as DishName, d.Price as DishPrice "
        "FROM OrderItems oi "
        "JOIN Dishes d ON oi.DishID = d.ID_Dish "
        "WHERE oi.OrderID = ?"
        );
    query.addBindValue(orderId);
    query.exec();

    while (query.next()) {
        OrderItem item;
        item.m_id = query.value("ID_OrderItem").toInt();
        item.m_orderId = orderId;
        item.m_dishId = query.value("ID_Dish").toInt();
        item.m_quantity = query.value("Quantity").toInt();
        item.m_priceAtOrder = query.value("PriceAtOrder").toDouble();
        items.append(item);
    }
    return items;
}