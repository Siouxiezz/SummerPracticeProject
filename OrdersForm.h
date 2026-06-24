#ifndef ORDERSFORM_H
#define ORDERSFORM_H

#include <QWidget>
#include <QMenu>
#include <QTimer>
#include "User.h"
#include "Order.h"

namespace Ui {
class OrdersForm;
}

class OrdersForm : public QWidget
{
    Q_OBJECT

public:
    explicit OrdersForm(const User& user, QWidget *parent = nullptr);
    ~OrdersForm();

    void setDarkTheme();
    void setLightTheme();

signals:
    void statusMessage(const QString& message);

private slots:
    void on_searchEdit_textChanged(const QString &text);
    void on_statusFilter_currentTextChanged(const QString &text);
    void on_deletePushButton_clicked();
    void on_updateStatusPushButton_clicked();
    void on_detailsPushButton_clicked();
    void on_refreshPushButton_clicked();
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void autoRefresh();

private:
    void setupTable();
    void setupContextMenu();
    void refreshStatusCombo();
    QString getStatusText(const QString& status);
    QString getStatusColor(const QString& status);
    void loadOrders();
    void updateTable(const QList<Order>& orders);
    void filterOrders(const QString& text);
    void showOrderDetails(int orderId);

    Ui::OrdersForm *ui;
    User m_currentUser;
    QList<Order> m_currentOrders;
    QMenu *m_contextMenu;
    QTimer *m_refreshTimer;
};

#endif // ORDERSFORM_H