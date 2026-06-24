#ifndef ORDERITEM_H
#define ORDERITEM_H

#include "Entity.h"
#include <QString>
#include <QList>

class OrderItem : public Entity {
public:
    OrderItem(int id = -1, int orderId = -1, int dishId = -1,
              int quantity = 1, double priceAtOrder = 0.0);

    int getId() const override { return m_id; }
    int getOrderId() const { return m_orderId; }
    int getDishId() const { return m_dishId; }
    int getQuantity() const { return m_quantity; }
    double getPriceAtOrder() const { return m_priceAtOrder; }
    double getSubtotal() const { return m_quantity * m_priceAtOrder; }

    void setId(int id) { m_id = id; }
    void setOrderId(int id) { m_orderId = id; }
    void setDishId(int id) { m_dishId = id; }
    void setQuantity(int qty) { m_quantity = qty; }
    void setPriceAtOrder(double price) { m_priceAtOrder = price; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    static QList<OrderItem> getItemsByOrder(int orderId);
    static QList<OrderItem> getItemsByDish(int dishId);
    static bool deleteItemsByOrder(int orderId);
    static QList<OrderItem> getItemsWithDetails(int orderId);

private:
    int m_id;
    int m_orderId;
    int m_dishId;
    int m_quantity;
    double m_priceAtOrder;
};

#endif // ORDERITEM_H