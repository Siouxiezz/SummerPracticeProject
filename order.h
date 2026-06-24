#ifndef ORDER_H
#define ORDER_H

#include "Entity.h"
#include "OrderItem.h"
#include <QDateTime>
#include <QString>
#include <QList>

class Order : public Entity {
public:
    Order(int id = -1, int tableId = -1, int clientId = -1, int waiterId = -1,
          const QDateTime& date = QDateTime::currentDateTime(),
          const QString& status = "New", double total = 0.0);

    int getId() const override { return m_id; }
    int getTableId() const { return m_tableId; }
    int getClientId() const { return m_clientId; }
    int getWaiterId() const { return m_waiterId; }
    QDateTime getDate() const { return m_date; }
    QString getStatus() const { return m_status; }
    double getTotal() const { return m_total; }
    QList<OrderItem> getItems() const { return m_items; }

    void setId(int id) { m_id = id; }
    void setTableId(int id) { m_tableId = id; }
    void setClientId(int id) { m_clientId = id; }
    void setWaiterId(int id) { m_waiterId = id; }
    void setDate(const QDateTime& date) { m_date = date; }
    void setStatus(const QString& status) { m_status = status; }
    void setTotal(double total) { m_total = total; }
    void setItems(const QList<OrderItem>& items) { m_items = items; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    bool addItem(int dishId, int quantity, double price);
    bool removeItem(int itemId);
    bool updateStatus(const QString& newStatus);
    double calculateTotal();
    void loadItems();

    static QList<Order> getAllOrders();
    static QList<Order> getOrdersByClient(int clientId);
    static QList<Order> getOrdersByWaiter(int waiterId);
    static QList<Order> getOrdersByStatus(const QString& status);
    static Order getById(int id);
    static QList<Order> getOrdersWithDetails();
    static double getTotalByClient(int clientId);
    static double getTotalByPeriod(const QDate& start, const QDate& end);

    friend bool compareOrdersByDate(const Order& a, const Order& b);
    friend bool compareOrdersByTotal(const Order& a, const Order& b);

private:
    int m_id;
    int m_tableId;
    int m_clientId;
    int m_waiterId;
    QDateTime m_date;
    QString m_status;
    double m_total;
    QList<OrderItem> m_items;
};

#endif // ORDER_H