#include "Order.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Dish.h"
#include "Table.h"
#include <QSqlQuery>
#include <QVariant>

Order::Order(int id, int tableId, int clientId, int waiterId,
             const QDateTime& date, const QString& status, double total)
    : m_id(id), m_tableId(tableId), m_clientId(clientId), m_waiterId(waiterId),
    m_date(date), m_status(status), m_total(total) {}

bool Order::save() {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1) {
        query.prepare("INSERT INTO Orders (TableID, ClientID, WaiterID, OrderDate, Status, Total) "
                      "VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(m_tableId);
        query.addBindValue(m_clientId);
        query.addBindValue(m_waiterId);
        query.addBindValue(m_date.toString("yyyy-MM-dd HH:mm:ss"));
        query.addBindValue(m_status);
        query.addBindValue(m_total);

        if (!query.exec()) {
            qDebug() << "Помилка збереження замовлення:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();

        for (const OrderItem& item : m_items) {
            OrderItem newItem = item;
            newItem.setOrderId(m_id);
            if (!newItem.save()) {
                return false;
            }
        }
        return true;
    } else {
        query.prepare("UPDATE Orders SET TableID=?, ClientID=?, WaiterID=?, "
                      "OrderDate=?, Status=?, Total=? WHERE ID_Order=?");
        query.addBindValue(m_tableId);
        query.addBindValue(m_clientId);
        query.addBindValue(m_waiterId);
        query.addBindValue(m_date.toString("yyyy-MM-dd HH:mm:ss"));
        query.addBindValue(m_status);
        query.addBindValue(m_total);
        query.addBindValue(m_id);

        if (!query.exec()) {
            qDebug() << "Помилка оновлення замовлення:" << query.lastError().text();
            return false;
        }

        OrderItem::deleteItemsByOrder(m_id);
        for (const OrderItem& item : m_items) {
            OrderItem newItem = item;
            newItem.setOrderId(m_id);
            if (!newItem.save()) {
                return false;
            }
        }
        return true;
    }
}

bool Order::load(int id) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Orders WHERE ID_Order = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    m_id = query.value("ID_Order").toInt();
    m_tableId = query.value("TableID").toInt();
    m_clientId = query.value("ClientID").toInt();
    m_waiterId = query.value("WaiterID").toInt();
    m_date = QDateTime::fromString(query.value("OrderDate").toString(), "yyyy-MM-dd HH:mm:ss");
    m_status = query.value("Status").toString();
    m_total = query.value("Total").toDouble();

    loadItems();

    return true;
}

void Order::loadItems() {
    m_items = OrderItem::getItemsWithDetails(m_id);
}

bool Order::remove() {
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();

    OrderItem::deleteItemsByOrder(m_id);

    QSqlQuery query;
    query.prepare("DELETE FROM Orders WHERE ID_Order = ?");
    query.addBindValue(m_id);
    return query.exec();
}

QString Order::toString() const {
    return QString("Замовлення #%1: столик %2, сума %3 грн, статус %4")
        .arg(m_id).arg(m_tableId).arg(m_total).arg(m_status);
}

bool Order::addItem(int dishId, int quantity, double price) {
    // Перевіряємо, чи вже є така страва в замовленні
    for (OrderItem& item : m_items) {
        if (item.getDishId() == dishId) {
            item.setQuantity(item.getQuantity() + quantity);
            m_total = calculateTotal();
            return true;
        }
    }

    // Додаємо нову позицію
    OrderItem newItem(-1, m_id, dishId, quantity, price);
    m_items.append(newItem);
    m_total = calculateTotal();
    return true;
}

bool Order::removeItem(int itemId) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].getId() == itemId) {
            m_items.removeAt(i);
            m_total = calculateTotal();
            return true;
        }
    }
    return false;
}

bool Order::updateStatus(const QString& newStatus) {
    m_status = newStatus;
    return save();
}

double Order::calculateTotal() {
    double total = 0;
    for (const OrderItem& item : m_items) {
        total += item.getQuantity() * item.getPriceAtOrder();
    }
    return total;
}

QList<Order> Order::getAllOrders() {
    QList<Order> orders;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT ID_Order FROM Orders ORDER BY OrderDate DESC");

    while (query.next()) {
        Order order;
        if (order.load(query.value(0).toInt())) {
            orders.append(order);
        }
    }
    return orders;
}

QList<Order> Order::getOrdersByClient(int clientId) {
    QList<Order> orders;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT ID_Order FROM Orders WHERE ClientID = ? ORDER BY OrderDate DESC");
    query.addBindValue(clientId);
    query.exec();

    while (query.next()) {
        Order order;
        if (order.load(query.value(0).toInt())) {
            orders.append(order);
        }
    }
    return orders;
}

QList<Order> Order::getOrdersByWaiter(int waiterId) {
    QList<Order> orders;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT ID_Order FROM Orders WHERE WaiterID = ? ORDER BY OrderDate DESC");
    query.addBindValue(waiterId);
    query.exec();

    while (query.next()) {
        Order order;
        if (order.load(query.value(0).toInt())) {
            orders.append(order);
        }
    }
    return orders;
}

QList<Order> Order::getOrdersByStatus(const QString& status) {
    QList<Order> orders;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT ID_Order FROM Orders WHERE Status = ? ORDER BY OrderDate DESC");
    query.addBindValue(status);
    query.exec();

    while (query.next()) {
        Order order;
        if (order.load(query.value(0).toInt())) {
            orders.append(order);
        }
    }
    return orders;
}

Order Order::getById(int id) {
    Order order;
    order.load(id);
    return order;
}

QList<Order> Order::getOrdersWithDetails() {
    QList<Order> orders = getAllOrders();
    for (Order& order : orders) {
        order.loadItems();
    }
    return orders;
}

double Order::getTotalByClient(int clientId) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT SUM(Total) FROM Orders WHERE ClientID = ? AND Status != 'Cancelled'");
    query.addBindValue(clientId);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

double Order::getTotalByPeriod(const QDate& start, const QDate& end) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT SUM(Total) FROM Orders "
                  "WHERE DATE(OrderDate) BETWEEN ? AND ? AND Status != 'Cancelled'");
    query.addBindValue(start.toString("yyyy-MM-dd"));
    query.addBindValue(end.toString("yyyy-MM-dd"));

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

bool compareOrdersByDate(const Order& a, const Order& b) {
    return a.m_date < b.m_date;
}

bool compareOrdersByTotal(const Order& a, const Order& b) {
    return a.m_total < b.m_total;
}