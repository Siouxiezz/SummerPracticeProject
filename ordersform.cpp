#include "OrdersForm.h"
#include "ui_ordersform.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Dish.h"
#include "Table.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QSqlQuery>
#include <QDateTime>

OrdersForm::OrdersForm(const User& user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OrdersForm)
    , m_currentUser(user)
{
    ui->setupUi(this);

    QString ordersformButtonStyle =
        "QPushButton {"
        "  background-color: #1a1a1e;"
        "  color: #D1B26D;"
        "  border: 1px solid #D1B26D;"
        "  border-radius: 6px;"
        "  padding: 5px 15px;"
        "  font-family: 'Segoe UI';"
        "  font-size: 11pt;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background-color: #D1B26D;"
        "  color: #1a1a1e;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #bfa363;"
        "}";

    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(ordersformButtonStyle);
    }

    bool isClient = (m_currentUser.getRole() == "Client");
    ui->deletePushButton->setVisible(!isClient);
    ui->updateStatusPushButton->setVisible(!isClient);

    setupTable();
    setupContextMenu();
    refreshStatusCombo();

    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &OrdersForm::on_searchEdit_textChanged);
    connect(ui->statusFilter, &QComboBox::currentTextChanged,
            this, &OrdersForm::on_statusFilter_currentTextChanged);

    loadOrders();

    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &OrdersForm::autoRefresh);
    m_refreshTimer->start(10000);

    setDarkTheme();
}

OrdersForm::~OrdersForm()
{
    if (m_refreshTimer) {
        m_refreshTimer->stop();
    }
    delete ui;
}

void OrdersForm::setDarkTheme()
{
    this->setStyleSheet("background-color: #1a1a1e; color: #ffffff;");
    if (ui->frame) ui->frame->setStyleSheet("background-color: #111114; border: 1px solid #333; border-radius: 6px;");

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #D1B26D; color: #1a1a1e; }";
    ui->tableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #242428; color: #ffffff; border: 1px solid #555555; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #242428; color: #ffffff; selection-background-color: #D1B26D; selection-color: #1a1a1e; }";
    ui->searchEdit->setStyleSheet(inputStyle);
    ui->statusFilter->setStyleSheet(inputStyle);

    QString labelStyle = "QLabel { color: #cccccc; }";
    ui->serachLabel->setStyleSheet(labelStyle);
    ui->statusLabel->setStyleSheet(labelStyle);

    QString buttonStyle =
        "QPushButton { background-color: #242428; color: #D1B26D; border: 1px solid #D1B26D; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #D1B26D; color: #1a1a1e; }"
        "QPushButton:pressed { background-color: #bfa363; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}

void OrdersForm::setLightTheme()
{
    this->setStyleSheet("background-color: #f5f5f7; color: #1a1a1e;");
    if (ui->frame) ui->frame->setStyleSheet("background-color: #ffffff; border: 1px solid #dddddd; border-radius: 6px;");

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #e0a96d; color: #ffffff; }";
    ui->tableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a1a1e; border: 1px solid #cccccc; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a1a1e; selection-background-color: #e0a96d; selection-color: #ffffff; }";
    ui->searchEdit->setStyleSheet(inputStyle);
    ui->statusFilter->setStyleSheet(inputStyle);

    QString labelStyle = "QLabel { color: #333333; }";
    ui->serachLabel->setStyleSheet(labelStyle);
    ui->statusLabel->setStyleSheet(labelStyle);

    QString buttonStyle =
        "QPushButton { background-color: #ffffff; color: #e0a96d; border: 1px solid #e0a96d; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #e0a96d; color: #ffffff; }"
        "QPushButton:pressed { background-color: #c48f56; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}

void OrdersForm::setupTable()
{
    ui->tableWidget->setColumnCount(9);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "ID" << "Столик" << "Клієнт" << "Офіціант"
                                               << "Страви" << "К-сть" << "Сума" << "Дата" << "Статус");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 70);
    ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(3, 150);
    ui->tableWidget->setColumnWidth(4, 200);
    ui->tableWidget->setColumnWidth(5, 60);
    ui->tableWidget->setColumnWidth(6, 80);
    ui->tableWidget->setColumnWidth(7, 150);
}

void OrdersForm::setupContextMenu()
{
    m_contextMenu = new QMenu(this);

    QAction *detailsAction = new QAction("Деталі", this);
    connect(detailsAction, &QAction::triggered, this, &OrdersForm::on_detailsPushButton_clicked);
    m_contextMenu->addAction(detailsAction);

    if (m_currentUser.getRole() != "Client") {
        m_contextMenu->addSeparator();

        QAction *statusAction = new QAction("Змінити статус", this);
        connect(statusAction, &QAction::triggered, this, &OrdersForm::on_updateStatusPushButton_clicked);
        m_contextMenu->addAction(statusAction);

        m_contextMenu->addSeparator();

        QAction *deleteAction = new QAction("Видалити", this);
        connect(deleteAction, &QAction::triggered, this, &OrdersForm::on_deletePushButton_clicked);
        m_contextMenu->addAction(deleteAction);
    }
}

void OrdersForm::refreshStatusCombo()
{
    ui->statusFilter->clear();
    ui->statusFilter->addItem("Всі");
    ui->statusFilter->addItem("🟡 Нове", "New");
    ui->statusFilter->addItem("🟠 Готується", "InProgress");
    ui->statusFilter->addItem("🟢 Оплачено", "Paid");
    ui->statusFilter->addItem("🔴 Скасовано", "Cancelled");
}

QString OrdersForm::getStatusText(const QString& status)
{
    if (status == "New") return "Нове";
    if (status == "InProgress") return "Готується";
    if (status == "Paid") return "Оплачено";
    if (status == "Cancelled") return "Скасовано";
    return status;
}

QString OrdersForm::getStatusColor(const QString& status)
{
    if (status == "New") return "#f39c12";
    if (status == "InProgress") return "#e67e22";
    if (status == "Paid") return "#27ae60";
    if (status == "Cancelled") return "#e74c3c";
    return "#95a5a6";
}

void OrdersForm::loadOrders()
{
    if (m_currentUser.getRole() == "Client") {
        m_currentOrders = Order::getOrdersByClient(m_currentUser.getId());
    } else {
        m_currentOrders = Order::getAllOrders();
    }
    updateTable(m_currentOrders);
}

void OrdersForm::updateTable(const QList<Order>& orders)
{
    ui->tableWidget->setRowCount(orders.size());

    for (int i = 0; i < orders.size(); ++i) {
        const Order& order = orders[i];

        Table table = Table::getById(order.getTableId());
        User client = User::getById(order.getClientId());

        QString waiterName = "не призначено";
        if (order.getWaiterId() != -1) {
            waiterName = User::getById(order.getWaiterId()).getName();
        }

        QString dishesStr;
        int totalItems = 0;
        double total = 0;

        for (const OrderItem& item : order.getItems()) {
            Dish dish = Dish::getById(item.getDishId());
            dishesStr += dish.getName() + " ×" + QString::number(item.getQuantity()) + ", ";
            totalItems += item.getQuantity();
            total += item.getQuantity() * item.getPriceAtOrder();
        }
        if (!dishesStr.isEmpty()) {
            dishesStr.chop(2);
        }

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(order.getId())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(table.getNumber())));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(client.getName()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(waiterName));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(dishesStr));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(totalItems)));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(total, 'f', 2)));
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(order.getDate().toString("dd.MM.yyyy HH:mm")));
        ui->tableWidget->setItem(i, 8, new QTableWidgetItem(getStatusText(order.getStatus())));

        QTableWidgetItem* statusItem = ui->tableWidget->item(i, 8);
        statusItem->setBackground(QColor(getStatusColor(order.getStatus())));
        statusItem->setForeground(QBrush(Qt::white));
    }
}

void OrdersForm::autoRefresh()
{
    if (!ui->searchEdit->text().isEmpty()) {
        loadOrders();
        filterOrders(ui->searchEdit->text());
    } else if (ui->statusFilter->currentData().toString() != "") {
        on_statusFilter_currentTextChanged(ui->statusFilter->currentText());
    } else {
        loadOrders();
    }
}

void OrdersForm::on_searchEdit_textChanged(const QString &text)
{
    filterOrders(text);
}

void OrdersForm::filterOrders(const QString& text)
{
    if (text.isEmpty()) {
        on_statusFilter_currentTextChanged(ui->statusFilter->currentText());
        return;
    }

    QList<Order> baseList;
    if (ui->statusFilter->currentData().toString().isEmpty() ||
        ui->statusFilter->currentText() == "Всі") {
        baseList = m_currentOrders;
    } else {
        QString statusFilter = ui->statusFilter->currentData().toString();
        for (const Order& order : m_currentOrders) {
            if (order.getStatus() == statusFilter) {
                baseList.append(order);
            }
        }
    }

    QList<Order> filtered;
    QString filterText = text.toLower();

    for (const Order& order : baseList) {
        Table table = Table::getById(order.getTableId());
        User client = User::getById(order.getClientId());

        QString waiterName = "не призначено";
        if (order.getWaiterId() != -1) {
            waiterName = User::getById(order.getWaiterId()).getName();
        }

        QString dishesStr;
        for (const OrderItem& item : order.getItems()) {
            Dish dish = Dish::getById(item.getDishId());
            dishesStr += dish.getName() + " ";
        }

        if (QString::number(table.getNumber()).contains(filterText) ||
            client.getName().contains(filterText, Qt::CaseInsensitive) ||
            waiterName.contains(filterText, Qt::CaseInsensitive) ||
            dishesStr.contains(filterText, Qt::CaseInsensitive) ||
            getStatusText(order.getStatus()).contains(filterText, Qt::CaseInsensitive)) {
            filtered.append(order);
        }
    }

    updateTable(filtered);
}

void OrdersForm::on_statusFilter_currentTextChanged(const QString &text)
{
    if (text == "Всі" || text.isEmpty()) {
        if (ui->searchEdit->text().isEmpty()) {
            loadOrders();
        } else {
            filterOrders(ui->searchEdit->text());
        }
        return;
    }

    QString statusValue = ui->statusFilter->currentData().toString();
    QList<Order> filtered;
    for (const Order& order : m_currentOrders) {
        if (order.getStatus() == statusValue) {
            filtered.append(order);
        }
    }

    if (!ui->searchEdit->text().isEmpty()) {
        QList<Order> textFiltered;
        QString filterText = ui->searchEdit->text().toLower();
        for (const Order& order : filtered) {
            Table table = Table::getById(order.getTableId());
            User client = User::getById(order.getClientId());
            QString waiterName = (order.getWaiterId() != -1) ? User::getById(order.getWaiterId()).getName() : "";
            if (QString::number(table.getNumber()).contains(filterText) ||
                client.getName().contains(filterText, Qt::CaseInsensitive) ||
                waiterName.contains(filterText, Qt::CaseInsensitive) ||
                getStatusText(order.getStatus()).contains(filterText, Qt::CaseInsensitive)) {
                textFiltered.append(order);
            }
        }
        filtered = textFiltered;
    }

    updateTable(filtered);
}

void OrdersForm::on_deletePushButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть замовлення для видалення!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->text().toInt();
    QString status = ui->tableWidget->item(row, 8)->text();

    if (status == "Оплачено") {
        QMessageBox::warning(this, "Помилка", "Не можна видалити оплачене замовлення!");
        return;
    }

    if (QMessageBox::question(this, "Видалення",
                              QString("Ви впевнені, що хочете видалити замовлення #%1?")
                                  .arg(id),
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    Order order = Order::getById(id);
    if (order.remove()) {
        loadOrders();
        emit statusMessage("Замовлення #" + QString::number(id) + " видалено!");
        QMessageBox::information(this, "Успіх", "Замовлення видалено!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося видалити замовлення!");
    }
}

void OrdersForm::on_updateStatusPushButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть замовлення для зміни статусу!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->text().toInt();
    QString currentStatus = ui->tableWidget->item(row, 8)->text();

    if (currentStatus == "Оплачено") {
        QMessageBox::warning(this, "Помилка", "Оплачене замовлення не можна змінювати!");
        return;
    }
    if (currentStatus == "Скасовано") {
        QMessageBox::warning(this, "Помилка", "Скасоване замовлення не можна змінювати!");
        return;
    }

    QStringList statuses = {"🟡 Нове", "🟠 Готується", "🟢 Оплачено", "🔴 Скасовано"};
    QStringList statusValues = {"New", "InProgress", "Paid", "Cancelled"};

    bool ok;
    QString selected = QInputDialog::getItem(this, "Зміна статусу",
                                             "Виберіть новий статус:", statuses,
                                             statuses.indexOf("🟡 " + currentStatus), false, &ok);
    if (!ok || selected.isEmpty()) return;

    QString newStatus = statusValues[statuses.indexOf(selected)];

    Order order = Order::getById(id);
    if (order.updateStatus(newStatus)) {
        loadOrders();
        emit statusMessage("Статус замовлення #" + QString::number(id) + " змінено на " + getStatusText(newStatus));
        QMessageBox::information(this, "Успіх",
                                 "Статус замовлення #" + QString::number(id) +
                                     " змінено на " + getStatusText(newStatus));
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося змінити статус!");
    }
}

void OrdersForm::on_detailsPushButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть замовлення для перегляду!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->text().toInt();
    showOrderDetails(id);
}

void OrdersForm::showOrderDetails(int orderId)
{
    Order order = Order::getById(orderId);

    Table table = Table::getById(order.getTableId());
    User client = User::getById(order.getClientId());

    QString waiterName = "не призначено";
    if (order.getWaiterId() != -1) {
        waiterName = User::getById(order.getWaiterId()).getName();
    }

    QString details = QString(
                          "ЗАМОВЛЕННЯ #%1\n"
                          "------------------------------------\n"
                          "Столик:    №%2\n"
                          "Клієнт:    %3\n"
                          "Телефон:   %4\n"
                          "Офіціант:  %5\n"
                          "Дата:      %6\n"
                          "Статус:    %7\n"
                          "------------------------------------\n"
                          "СТРАВИ:\n"
                          ).arg(orderId)
                          .arg(table.getNumber())
                          .arg(client.getName())
                          .arg(client.getPhone())
                          .arg(waiterName)
                          .arg(order.getDate().toString("dd.MM.yyyy HH:mm"))
                          .arg(getStatusText(order.getStatus()));

    double total = 0;
    for (const OrderItem& item : order.getItems()) {
        Dish dish = Dish::getById(item.getDishId());
        double subtotal = item.getQuantity() * item.getPriceAtOrder();
        details += QString("  %1 ×%2 = %3 грн\n")
                       .arg(dish.getName())
                       .arg(item.getQuantity())
                       .arg(subtotal, 0, 'f', 2);
        total += subtotal;
    }

    details += QString(
                   "------------------------------------\n"
                   "ВСЬОГО: %1 грн"
                   ).arg(total, 0, 'f', 2);

    QMessageBox::information(this, "Деталі замовлення", details);
}

void OrdersForm::on_refreshPushButton_clicked()
{
    loadOrders();
    if (this->palette().window().color().value() > 128) {
        setLightTheme();
    } else {
        setDarkTheme();
    }
}

void OrdersForm::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tableWidget->currentRow() < 0) return;
    m_contextMenu->exec(ui->tableWidget->viewport()->mapToGlobal(pos));
}

void OrdersForm::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    on_detailsPushButton_clicked();
}