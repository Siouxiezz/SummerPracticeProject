#include "AdminForm.h"
#include "ui_adminform.h"
#include "DatabaseManager.h"
#include "Booking.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QSqlQuery>
#include <QHeaderView>
#include <QLineEdit>

AdminForm::AdminForm(const User& currentUser, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminForm)
    , m_currentUser(currentUser)
{
    ui->setupUi(this);

    QString adminButtonStyle =
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
        btn->setStyleSheet(adminButtonStyle);
    }

    setupUsersTab();
    setupUsersContextMenu();
    loadUsers();

    setupTablesTab();
    setupTablesContextMenu();
    loadTables();

    ui->stackedWidget->setCurrentWidget(ui->pageUsers);

    setDarkTheme();
}

AdminForm::~AdminForm()
{
    delete ui;
}

void AdminForm::setDarkTheme()
{
    this->setStyleSheet("background-color: #1a1a1e; color: #ffffff;");
    ui->navFrame->setStyleSheet("background-color: #111114; border-bottom: 1px solid #333;");

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #D1B26D; color: #1a1a1e; }";
    ui->usersTableWidget->setStyleSheet(tableStyle);
    ui->tablesTableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #242428; color: #ffffff; border: 1px solid #555555; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #242428; color: #ffffff; selection-background-color: #D1B26D; selection-color: #1a1a1e; }";
    ui->usersSearchEdit->setStyleSheet(inputStyle);
    ui->usersRoleFilter->setStyleSheet(inputStyle);

    QString buttonStyle =
        "QPushButton { background-color: #242428; color: #D1B26D; border: 1px solid #D1B26D; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #D1B26D; color: #1a1a1e; }"
        "QPushButton:pressed { background-color: #bfa363; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }

    QString labelStyle = "QLabel { color: #cccccc; }";
    ui->labelSearchTitle->setStyleSheet(labelStyle);
    ui->labelRoleTitle->setStyleSheet(labelStyle);
}

void AdminForm::setLightTheme()
{
    this->setStyleSheet("background-color: #f5f5f7; color: #1a1a1e;");
    ui->navFrame->setStyleSheet("background-color: #ffffff; border-bottom: 1px solid #dddddd;");

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #e0a96d; color: #ffffff; }";
    ui->usersTableWidget->setStyleSheet(tableStyle);
    ui->tablesTableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a1a1e; border: 1px solid #cccccc; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a1a1e; selection-background-color: #e0a96d; selection-color: #ffffff; }";
    ui->usersSearchEdit->setStyleSheet(inputStyle);
    ui->usersRoleFilter->setStyleSheet(inputStyle);

    QString buttonStyle =
        "QPushButton { background-color: #ffffff; color: #e0a96d; border: 1px solid #e0a96d; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #e0a96d; color: #ffffff; }"
        "QPushButton:pressed { background-color: #c48f56; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }

    QString labelStyle = "QLabel { color: #333333; }";
    ui->labelSearchTitle->setStyleSheet(labelStyle);
    ui->labelRoleTitle->setStyleSheet(labelStyle);
}

void AdminForm::on_pushButtonUsersTab_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageUsers);
}

void AdminForm::on_pushButtonTablesTab_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTables);
}

void AdminForm::setupUsersTab()
{
    ui->usersTableWidget->setColumnCount(6);
    ui->usersTableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Логін" << "ПІБ" << "Телефон" << "Роль" << "");
    ui->usersTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->usersTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->usersTableWidget->setAlternatingRowColors(true);
    ui->usersTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->usersTableWidget->setColumnWidth(0, 40);
    ui->usersTableWidget->setColumnWidth(1, 130);
    ui->usersTableWidget->setColumnWidth(2, 180);
    ui->usersTableWidget->setColumnWidth(3, 130);
    ui->usersTableWidget->setColumnWidth(4, 100);

    ui->usersRoleFilter->clear();
    ui->usersRoleFilter->addItem("Всі ролі");
    ui->usersRoleFilter->addItem("Client");
    ui->usersRoleFilter->addItem("Waiter");
    ui->usersRoleFilter->addItem("Admin");
}

void AdminForm::setupUsersContextMenu()
{
    m_usersContextMenu = new QMenu(this);
    QAction *editAction = new QAction("Редагувати", this);
    connect(editAction, &QAction::triggered, this, &AdminForm::on_usersEditPushButton_clicked);
    m_usersContextMenu->addAction(editAction);

    QAction *deleteAction = new QAction("Видалити", this);
    connect(deleteAction, &QAction::triggered, this, &AdminForm::on_usersDeletePushButton_clicked);
    m_usersContextMenu->addAction(deleteAction);
}

void AdminForm::loadUsers()
{
    m_currentUsers = User::getAllUsers();
    updateUsersTable(m_currentUsers);
}

void AdminForm::updateUsersTable(const QList<User>& users)
{
    ui->usersTableWidget->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        ui->usersTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(user.getId())));
        ui->usersTableWidget->setItem(i, 1, new QTableWidgetItem(user.getUsername()));
        ui->usersTableWidget->setItem(i, 2, new QTableWidgetItem(user.getName()));
        ui->usersTableWidget->setItem(i, 3, new QTableWidgetItem(user.getPhone()));
        ui->usersTableWidget->setItem(i, 4, new QTableWidgetItem(user.getRole()));

        QString marker = (user.getId() == m_currentUser.getId()) ? "Це ви" : "";
        ui->usersTableWidget->setItem(i, 5, new QTableWidgetItem(marker));
    }
}

void AdminForm::on_usersSearchEdit_textChanged(const QString &text)
{
    Q_UNUSED(text);
    filterUsers();
}

void AdminForm::on_usersRoleFilter_currentTextChanged(const QString &text)
{
    Q_UNUSED(text);
    filterUsers();
}

void AdminForm::filterUsers()
{
    QString searchText = ui->usersSearchEdit->text().trimmed().toLower();
    QString roleFilter = ui->usersRoleFilter->currentText();
    bool allRoles = (roleFilter == "Всі ролі");

    QList<User> filtered;
    for (const User& user : m_currentUsers) {
        if (!allRoles && user.getRole() != roleFilter) continue;
        if (!searchText.isEmpty()) {
            bool matches = user.getName().contains(searchText, Qt::CaseInsensitive) ||
                           user.getUsername().contains(searchText, Qt::CaseInsensitive) ||
                           user.getPhone().contains(searchText);
            if (!matches) continue;
        }
        filtered.append(user);
    }
    updateUsersTable(filtered);
}

void AdminForm::on_usersEditPushButton_clicked()
{
    int row = ui->usersTableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть користувача для редагування!");
        return;
    }

    int id = ui->usersTableWidget->item(row, 0)->text().toInt();
    User user = User::getById(id);

    bool ok = false;
    QString name = QInputDialog::getText(this, "Редагувати користувача", "ПІБ:", QLineEdit::Normal, user.getName(), &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QString phone = QInputDialog::getText(this, "Редагувати користувача", "Телефон:", QLineEdit::Normal, user.getPhone(), &ok);
    if (!ok) return;

    user.setName(name);
    user.setPhone(phone);

    if (user.save()) {
        loadUsers();
        emit statusMessage("Дані користувача оновлено!");
    }
}

void AdminForm::on_usersDeletePushButton_clicked()
{
    int row = ui->usersTableWidget->currentRow();
    if (row < 0) return;

    int id = ui->usersTableWidget->item(row, 0)->text().toInt();
    QString name = ui->usersTableWidget->item(row, 2)->text();

    if (id == m_currentUser.getId()) {
        QMessageBox::warning(this, "Помилка", "Ви не можете видалити власний акаунт!");
        return;
    }

    if (QMessageBox::question(this, "Видалення", QString("Видалити '%1'?").arg(name), QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;

    User user = User::getById(id);
    if (user.remove()) {
        loadUsers();
    }
}

void AdminForm::on_usersRefreshPushButton_clicked() { loadUsers(); }
void AdminForm::on_usersTableWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->usersTableWidget->currentRow() < 0) return;
    m_usersContextMenu->exec(ui->usersTableWidget->viewport()->mapToGlobal(pos));
}
void AdminForm::on_usersTableWidget_doubleClicked(const QModelIndex &index) { Q_UNUSED(index); on_usersEditPushButton_clicked(); }

void AdminForm::setupTablesTab()
{
    ui->tablesTableWidget->setColumnCount(4);
    ui->tablesTableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Номер" << "Місткість" << "Статус");
    ui->tablesTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tablesTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tablesTableWidget->setAlternatingRowColors(true);
    ui->tablesTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->tablesTableWidget->setColumnWidth(0, 40);
    ui->tablesTableWidget->setColumnWidth(1, 100);
    ui->tablesTableWidget->setColumnWidth(2, 100);
}

void AdminForm::setupTablesContextMenu()
{
    m_tablesContextMenu = new QMenu(this);
    QAction *editAction = new QAction("Редагувати", this);
    connect(editAction, &QAction::triggered, this, &AdminForm::on_tablesEditPushButton_clicked);
    m_tablesContextMenu->addAction(editAction);

    QAction *deleteAction = new QAction("Видалити", this);
    connect(deleteAction, &QAction::triggered, this, &AdminForm::on_tablesDeletePushButton_clicked);
    m_tablesContextMenu->addAction(deleteAction);
}

void AdminForm::loadTables()
{
    m_currentTables = Table::getAllTables();
    updateTablesTable(m_currentTables);
}

void AdminForm::updateTablesTable(const QList<Table>& tables)
{
    ui->tablesTableWidget->setRowCount(tables.size());
    for (int i = 0; i < tables.size(); ++i) {
        const Table& table = tables[i];
        ui->tablesTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(table.getId())));
        ui->tablesTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(table.getNumber())));
        ui->tablesTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(table.getCapacity())));

        QString statusText = table.isAvailable() ? "Вільний" : "Зайнятий";
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setBackground(QColor(table.isAvailable() ? "#27ae60" : "#e74c3c"));
        statusItem->setForeground(QBrush(Qt::white));
        ui->tablesTableWidget->setItem(i, 3, statusItem);
    }
}

void AdminForm::on_tablesAddPushButton_clicked()
{
    bool ok = false;
    int number = QInputDialog::getInt(this, "Новий столик", "Номер столика:", 1, 1, 999, 1, &ok);
    if (!ok) return;

    for (const Table& t : m_currentTables) {
        if (t.getNumber() == number) {
            QMessageBox::warning(this, "Помилка", "Столик вже існує!");
            return;
        }
    }

    int capacity = QInputDialog::getInt(this, "Новий столик", "Місткість:", 4, 1, 50, 1, &ok);
    if (!ok) return;

    Table newTable(-1, number, capacity, true);
    if (newTable.save()) loadTables();
}

void AdminForm::on_tablesEditPushButton_clicked()
{
    int row = ui->tablesTableWidget->currentRow();
    if (row < 0) return;

    int id = ui->tablesTableWidget->item(row, 0)->text().toInt();
    Table table = Table::getById(id);

    bool ok = false;
    int number = QInputDialog::getInt(this, "Редагувати столик", "Номер столика:", table.getNumber(), 1, 999, 1, &ok);
    if (!ok) return;

    int capacity = QInputDialog::getInt(this, "Редагувати столик", "Місткість:", table.getCapacity(), 1, 50, 1, &ok);
    if (!ok) return;

    table.setNumber(number);
    table.setCapacity(capacity);
    if (table.save()) loadTables();
}

void AdminForm::on_tablesDeletePushButton_clicked()
{
    int row = ui->tablesTableWidget->currentRow();
    if (row < 0) return;

    int id = ui->tablesTableWidget->item(row, 0)->text().toInt();
    int number = ui->tablesTableWidget->item(row, 1)->text().toInt();

    QString reason;
    if (!isTableDeletable(id, reason)) {
        QMessageBox::warning(this, "Неможливо видалити", reason);
        return;
    }

    if (QMessageBox::question(this, "Видалення", QString("Видалити столик №%1?").arg(number), QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) return;

    Table table = Table::getById(id);
    if (table.remove()) loadTables();
}

void AdminForm::on_tablesRefreshPushButton_clicked() { loadTables(); }
void AdminForm::on_tablesTableWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tablesTableWidget->currentRow() < 0) return;
    m_tablesContextMenu->exec(ui->tablesTableWidget->viewport()->mapToGlobal(pos));
}
void AdminForm::on_tablesTableWidget_doubleClicked(const QModelIndex &index) { Q_UNUSED(index); on_tablesEditPushButton_clicked(); }

bool AdminForm::isTableDeletable(int tableId, QString& reasonIfNot)
{
    QList<Booking> bookings = Booking::getBookingsByTable(tableId);
    for (const Booking& booking : bookings) {
        if (booking.getStatus() == "Pending" || booking.getStatus() == "Confirmed") {
            reasonIfNot = "У столика є активні бронювання!";
            return false;
        }
    }
    return true;
}