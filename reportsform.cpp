#include "reportsform.h"
#include "ui_reportsform.h"
#include "DatabaseManager.h"
#include "Order.h"
#include "Dish.h"
#include "User.h"
#include "Table.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QMap>
#include <QHeaderView>
#include <QSqlQuery>

ReportsForm::ReportsForm(const User& user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportsForm)
    , m_currentUser(user)
{
    ui->setupUi(this);
    setupUiDesign();

    ui->stackedWidget->setCurrentWidget(ui->pageRevenue);
    on_btnToday_clicked();
    calculatePopularDishes();
    calculateTablesStats();

    setDarkTheme();
}

ReportsForm::~ReportsForm()
{
    delete ui;
}

void ReportsForm::setDarkTheme()
{
    this->setStyleSheet("background-color: #1a1a1e; color: #ffffff;");
    ui->navFrame->setStyleSheet("background-color: #111114; border-bottom: 1px solid #333;");

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #D1B26D; color: #1a1a1e; }";
    ui->tableRevenue->setStyleSheet(tableStyle);
    ui->tablePopular->setStyleSheet(tableStyle);
    ui->tablesStatsTableWidget->setStyleSheet(tableStyle);

    QString labelStyle = "QLabel { color: #ffffff; }";
    ui->lblPeriodInfo->setStyleSheet(labelStyle);
    ui->lblTotalRevenue->setStyleSheet(labelStyle);
    ui->lblOrdersCount->setStyleSheet(labelStyle);

    QString buttonStyle =
        "QPushButton { background-color: #242428; color: #D1B26D; border: 1px solid #D1B26D; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #D1B26D; color: #1a1a1e; }"
        "QPushButton:pressed { background-color: #bfa363; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}

void ReportsForm::setLightTheme()
{
    this->setStyleSheet("background-color: #f5f5f7; color: #1a1a1e;");
    ui->navFrame->setStyleSheet("background-color: #ffffff; border-bottom: 1px solid #dddddd;");

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #e0a96d; color: #ffffff; }";
    ui->tableRevenue->setStyleSheet(tableStyle);
    ui->tablePopular->setStyleSheet(tableStyle);
    ui->tablesStatsTableWidget->setStyleSheet(tableStyle);

    QString labelStyle = "QLabel { color: #1a1a1e; }";
    ui->lblPeriodInfo->setStyleSheet(labelStyle);
    ui->lblTotalRevenue->setStyleSheet(labelStyle);
    ui->lblOrdersCount->setStyleSheet(labelStyle);

    QString buttonStyle =
        "QPushButton { background-color: #ffffff; color: #e0a96d; border: 1px solid #e0a96d; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #e0a96d; color: #ffffff; }"
        "QPushButton:pressed { background-color: #c48f56; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}

void ReportsForm::setupUiDesign()
{
    ui->tableRevenue->setColumnCount(4);
    ui->tableRevenue->setHorizontalHeaderLabels({"ID Замовлення", "Дата", "Офіціант", "Сума (грн)"});
    ui->tableRevenue->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableRevenue->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tablePopular->setColumnCount(3);
    ui->tablePopular->setHorizontalHeaderLabels({"Назва страви", "Порцій замовлено", "Дохід (грн)"});
    ui->tablePopular->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablePopular->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tablesStatsTableWidget->setColumnCount(4);
    ui->tablesStatsTableWidget->setHorizontalHeaderLabels({"Столик", "К-сть замовлень", "Загальна виручка", "Середній чек"});
    ui->tablesStatsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tablesStatsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ReportsForm::on_pushButtonRevenue_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageRevenue);
    emit statusMessage("Відображено фінансову виручку");
}

void ReportsForm::on_pushButtonPopular_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pagePopular);
    calculatePopularDishes();
    emit statusMessage("Відображено аналітику популярних страв");
}

void ReportsForm::on_pushButtonTablesStats_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTablesStats);
    calculateTablesStats();
    emit statusMessage("Відображено статистику по столиках");
}

void ReportsForm::on_btnToday_clicked()
{
    m_currentStartDate = QDate::currentDate();
    m_currentEndDate = QDate::currentDate();
    calculateRevenue(m_currentStartDate, m_currentEndDate, "Сьогодні");
}

void ReportsForm::on_btnWeek_clicked()
{
    m_currentStartDate = QDate::currentDate().addDays(-6);
    m_currentEndDate = QDate::currentDate();
    calculateRevenue(m_currentStartDate, m_currentEndDate, "Останні 7 днів");
}

void ReportsForm::on_btnMonth_clicked()
{
    m_currentStartDate = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    m_currentEndDate = QDate::currentDate();
    calculateRevenue(m_currentStartDate, m_currentEndDate, "Поточний місяць");
}

void ReportsForm::on_btnYear_clicked()
{
    m_currentStartDate = QDate(QDate::currentDate().year(), 1, 1);
    m_currentEndDate = QDate::currentDate();
    calculateRevenue(m_currentStartDate, m_currentEndDate, "Поточний рік");
}

void ReportsForm::calculateRevenue(const QDate& startDate, const QDate& endDate, const QString& periodName)
{
    ui->tableRevenue->setRowCount(0);
    ui->lblPeriodInfo->setText(QString("Період: %1 (%2 - %3)")
                                   .arg(periodName)
                                   .arg(startDate.toString("dd.MM.yyyy"))
                                   .arg(endDate.toString("dd.MM.yyyy")));

    QList<Order> allOrders = Order::getAllOrders();
    double totalRevenue = 0.0;
    int ordersCount = 0;

    for (const auto& order : allOrders) {
        if (order.getDate().date() >= startDate && order.getDate().date() <= endDate) {
            int row = ui->tableRevenue->rowCount();
            ui->tableRevenue->insertRow(row);

            QString waiterName = "не призначено";
            if (order.getWaiterId() != -1) {
                User waiter = User::getById(order.getWaiterId());
                waiterName = waiter.getName().isEmpty() ? "не призначено" : waiter.getName();
            }

            ui->tableRevenue->setItem(row, 0, new QTableWidgetItem(QString::number(order.getId())));
            ui->tableRevenue->setItem(row, 1, new QTableWidgetItem(order.getDate().toString("dd.MM.yyyy")));
            ui->tableRevenue->setItem(row, 2, new QTableWidgetItem(waiterName));
            ui->tableRevenue->setItem(row, 3, new QTableWidgetItem(QString::number(order.getTotal(), 'f', 2)));

            if (order.getStatus() != "Cancelled") {
                totalRevenue += order.getTotal();
            }
            ordersCount++;
        }
    }

    ui->lblTotalRevenue->setText(QString("Загальна виручка: %1 грн").arg(QString::number(totalRevenue, 'f', 2)));
    ui->lblOrdersCount->setText(QString("Кількість замовлень: %1").arg(ordersCount));

    emit statusMessage(QString("Сформовано фінансовий звіт за період: %1").arg(periodName));
}

void ReportsForm::calculatePopularDishes()
{
    ui->tablePopular->setRowCount(0);

    struct DishStats { int count = 0; double income = 0.0; };
    QMap<int, DishStats> statsMap;

    QList<Order> allOrders = Order::getAllOrders();

    for (const auto& order : allOrders) {
        for (const auto& item : order.getItems()) {
            int dishId = item.getDishId();
            int quantity = item.getQuantity();

            statsMap[dishId].count += quantity;
            statsMap[dishId].income += item.getSubtotal();
        }
    }

    for (auto it = statsMap.constBegin(); it != statsMap.constEnd(); ++it) {
        int row = ui->tablePopular->rowCount();
        ui->tablePopular->insertRow(row);

        Dish dish = Dish::getById(it.key());
        QString dishName = dish.getName().isEmpty() ? QString("Страва №%1 (Видалена)").arg(it.key()) : dish.getName();

        ui->tablePopular->setItem(row, 0, new QTableWidgetItem(dishName));
        ui->tablePopular->setItem(row, 1, new QTableWidgetItem(QString::number(it.value().count)));
        ui->tablePopular->setItem(row, 2, new QTableWidgetItem(QString::number(it.value().income, 'f', 2)));
    }

    ui->tablePopular->sortItems(1, Qt::DescendingOrder);
}

void ReportsForm::calculateTablesStats()
{
    QSqlQuery query(
        "SELECT t.ID_Table, t.TableNumber, "
        "COUNT(o.ID_Order) as orderCount, "
        "COALESCE(SUM(CASE WHEN o.Status != 'Cancelled' THEN o.Total ELSE 0 END), 0) as totalRevenue "
        "FROM Tables t "
        "LEFT JOIN Orders o ON o.TableID = t.ID_Table "
        "GROUP BY t.ID_Table, t.TableNumber "
        "ORDER BY totalRevenue DESC"
        );

    ui->tablesStatsTableWidget->setRowCount(0);

    while (query.next()) {
        int tableNumber = query.value("TableNumber").toInt();
        int orderCount = query.value("orderCount").toInt();
        double totalRevenue = query.value("totalRevenue").toDouble();
        double avgCheck = (orderCount > 0) ? (totalRevenue / orderCount) : 0.0;

        int row = ui->tablesStatsTableWidget->rowCount();
        ui->tablesStatsTableWidget->insertRow(row);

        ui->tablesStatsTableWidget->setItem(row, 0, new QTableWidgetItem(QString("№%1").arg(tableNumber)));
        ui->tablesStatsTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(orderCount)));
        ui->tablesStatsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(totalRevenue, 'f', 2)));
        ui->tablesStatsTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(avgCheck, 'f', 2)));
    }
}

void ReportsForm::on_tablesStatsRefreshPushButton_clicked()
{
    calculateTablesStats();
    emit statusMessage("Статистику по столиках оновлено");
}

void ReportsForm::on_btnExportRevenue_clicked()
{
    if (ui->tableRevenue->rowCount() == 0) {
        QMessageBox::warning(this, "Помилка", "Немає даних для експорту за обраний період!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Експорт виручки", "revenue_report.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося відкрити файл для запису!");
        return;
    }

    QTextStream out(&file);
    out << u'\ufeff';
    out << "ID Замовлення;Дата;Офіціант;Сума (грн)\n";

    for (int i = 0; i < ui->tableRevenue->rowCount(); ++i) {
        out << ui->tableRevenue->item(i, 0)->text() << ";"
            << ui->tableRevenue->item(i, 1)->text() << ";"
            << ui->tableRevenue->item(i, 2)->text() << ";"
            << ui->tableRevenue->item(i, 3)->text() << "\n";
    }
    file.close();

    QMessageBox::information(this, "Успіх", "Звіт з виручки успішно експортовано!");
    emit statusMessage("Дані виручки успішно експортовано в CSV файл.");
}

void ReportsForm::on_btnExportPopular_clicked()
{
    if (ui->tablePopular->rowCount() == 0) {
        QMessageBox::warning(this, "Помилка", "Немає даних для експорту!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Експорт популярних страв", "popular_dishes.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося відкрити файл для запису!");
        return;
    }

    QTextStream out(&file);
    out << u'\ufeff';
    out << "Назва страви;Порцій замовлено;Дохід (грн)\n";

    for (int i = 0; i < ui->tablePopular->rowCount(); ++i) {
        out << ui->tablePopular->item(i, 0)->text() << ";"
            << ui->tablePopular->item(i, 1)->text() << ";"
            << ui->tablePopular->item(i, 2)->text() << "\n";
    }
    file.close();

    QMessageBox::information(this, "Успіх", "Аналітику страв успішно експортовано!");
    emit statusMessage("Статистику страв успішно збережено у вибрану директорію.");
}

void ReportsForm::on_tablesStatsExportPushButton_clicked()
{
    if (ui->tablesStatsTableWidget->rowCount() == 0) {
        QMessageBox::warning(this, "Помилка", "Немає даних для експорту!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Експорт статистики по столиках", "tables_stats.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося відкрити файл для запису!");
        return;
    }

    QTextStream out(&file);
    out << u'\ufeff';
    out << "Столик;К-сть замовлень;Загальна виручка;Середній чек\n";

    for (int i = 0; i < ui->tablesStatsTableWidget->rowCount(); ++i) {
        out << ui->tablesStatsTableWidget->item(i, 0)->text() << ";"
            << ui->tablesStatsTableWidget->item(i, 1)->text() << ";"
            << ui->tablesStatsTableWidget->item(i, 2)->text() << ";"
            << ui->tablesStatsTableWidget->item(i, 3)->text() << "\n";
    }
    file.close();

    QMessageBox::information(this, "Успіх", "Статистику по столиках успішно експортовано!");
    emit statusMessage("Статистику по столиках збережено у вибрану директорію.");
}