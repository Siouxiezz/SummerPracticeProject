#include "MenuForm.h"
#include "ui_menuform.h"
#include "Dish.h"
#include "Order.h"
#include "Table.h"
#include "User.h"
#include "DatabaseManager.h"
#include "AddToCartDialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QMenu>
#include <QAction>
#include <QSqlQuery>
#include <QDateTime>
#include <QLineEdit>

MenuForm::MenuForm(const User& user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuForm)
    , m_currentUser(user)
{
    ui->setupUi(this);

    QString menuformButtonStyle =
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
        btn->setStyleSheet(menuformButtonStyle);
    }

    setupTable();
    setupContextMenu();
    loadDishes();

    bool isAdmin = (m_currentUser.getRole() == "Admin");
    ui->pushButtonAdd->setVisible(isAdmin);
    ui->pushButtonEdit->setVisible(isAdmin);
    ui->pushButtonDelete->setVisible(isAdmin);
    ui->pushButtonImport->setVisible(isAdmin);

    connect(ui->sortCategoryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuForm::on_sortCategoryComboBox_currentIndexChanged);

    ui->labelStatus->setText("Готово");
}

MenuForm::~MenuForm()
{
    delete ui;
}

void MenuForm::setupTable()
{
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "ID" << "Назва" << "Категорія" << "Ціна" << "Вага" << "Опис");

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 120);
    ui->tableWidget->setColumnWidth(3, 80);
    ui->tableWidget->setColumnWidth(4, 80);
}

void MenuForm::populateCategoryComboBox()
{
    ui->sortCategoryComboBox->blockSignals(true);

    ui->sortCategoryComboBox->clear();
    ui->sortCategoryComboBox->addItem("Всі категорії");

    QStringList categories;
    for (const Dish& dish : m_currentDishes) {
        if (!categories.contains(dish.getCategory())) {
            categories.append(dish.getCategory());
        }
    }
    categories.sort();
    ui->sortCategoryComboBox->addItems(categories);

    ui->sortCategoryComboBox->setCurrentIndex(0);
    ui->sortCategoryComboBox->blockSignals(false);
}

void MenuForm::on_sortCategoryComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    applyFilters();
}

void MenuForm::applyFilters()
{
    QString searchText = ui->searchEdit->text().trimmed();
    QString selectedCategory = ui->sortCategoryComboBox->currentText();

    QList<Dish> filtered;

    for (const Dish& dish : m_currentDishes) {
        if (selectedCategory != "Всі категорії" && dish.getCategory() != selectedCategory) {
            continue;
        }

        if (!searchText.isEmpty()) {
            bool match = dish.getName().contains(searchText, Qt::CaseInsensitive) ||
                         dish.getCategory().contains(searchText, Qt::CaseInsensitive) ||
                         dish.getDescription().contains(searchText, Qt::CaseInsensitive);
            if (!match) continue;
        }

        filtered.append(dish);
    }

    updateTable(filtered);
    ui->labelStatus->setText(QString("Знайдено %1 страв").arg(filtered.size()));
}

void MenuForm::applyTheme(bool isDark)
{
    if (isDark)
        setDarkTheme();
    else
        setLightTheme();
}

// ---------- ТЕМНА ТЕМА ----------
void MenuForm::setDarkTheme()
{
    // Головне вікно
    this->setStyleSheet(
        "QWidget { background-color: #1a1a1e; color: #ffffff; }"
        );

    // Верхній фрейм
    if (ui->frame) {
        ui->frame->setStyleSheet(
            "QFrame { background-color: #111114; border: 1px solid #444444; border-radius: 8px; }"
            );
    }

    // Поля вводу (QLineEdit)
    if (ui->searchEdit) {
        ui->searchEdit->setStyleSheet(
            "QLineEdit { background-color: #242428; color: #ffffff; border: 1px solid #555555; border-radius: 6px; padding-left: 10px; }"
            "QLineEdit:focus { border: 1px solid #D1B26D; }"
            "QLineEdit::placeholder { color: #aaaaaa; }"
            );
    }

    if (ui->nameLabel) {
        ui->nameLabel->setStyleSheet("QLabel { color: #cccccc; }");
    }
    if (ui->labelStatus) {
        ui->labelStatus->setStyleSheet("QLabel { color: #cccccc; }");
    }
    if (ui->label) {
        ui->label->setStyleSheet("QLabel { color: #D4AF37; }");
    }

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; border-radius: 6px; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }"
        "QTableWidget::item { padding: 6px; }"
        "QTableWidget::item:selected { background-color: #D1B26D; color: #1a1a1e; }"
        "QTableWidget::item:selected:!active { background-color: #D1B26D; color: #1a1a1e; }";
    if (ui->tableWidget) {
        ui->tableWidget->setStyleSheet(tableStyle);
    }

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #1a1a1e;"
        "  color: #D1B26D;"
        "  border: 1px solid #D1B26D;"
        "  border-radius: 6px;"
        "  padding: 5px 15px;"
        "  font-family: 'Segoe UI';"
        "  font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "  background-color: #D1B26D;"
        "  color: #1a1a1e;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #bfa363;"
        "  color: #1a1a1e;"
        "}";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }

    if (ui->pushButtonCart) {
        ui->pushButtonCart->setStyleSheet(buttonStyle +
                                          "QPushButton { border: 2px solid #C5A028; font-weight: bold; }");
    }
    if (ui->pushButtonOrder) {
        ui->pushButtonOrder->setStyleSheet(buttonStyle +
                                           "QPushButton { border: 2px solid #C5A028; font-weight: bold; }");
    }
}

void MenuForm::setLightTheme()
{

    this->setStyleSheet(
        "QWidget { background-color: #f5f5f7; color: #1a1a1e; }"
        );

    if (ui->frame) {
        ui->frame->setStyleSheet(
            "QFrame { background-color: #ffffff; border: 1px solid #dddddd; border-radius: 8px; }"
            );
    }

    if (ui->searchEdit) {
        ui->searchEdit->setStyleSheet(
            "QLineEdit { background-color: #ffffff; color: #1a1a1e; border: 1px solid #cccccc; border-radius: 6px; padding-left: 10px; }"
            "QLineEdit:focus { border: 1px solid #e0a96d; }"
            "QLineEdit::placeholder { color: #888888; }"
            );
    }

    if (ui->nameLabel) {
        ui->nameLabel->setStyleSheet("QLabel { color: #333333; }");
    }
    if (ui->labelStatus) {
        ui->labelStatus->setStyleSheet("QLabel { color: #333333; }");
    }

    if (ui->label) {
        ui->label->setStyleSheet("QLabel { color: #B8860B; }");
    }

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; border-radius: 6px; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }"
        "QTableWidget::item { padding: 6px; }"
        "QTableWidget::item:selected { background-color: #e0a96d; color: #ffffff; }"
        "QTableWidget::item:selected:!active { background-color: #e0a96d; color: #ffffff; }";
    if (ui->tableWidget) {
        ui->tableWidget->setStyleSheet(tableStyle);
    }

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #ffffff;"
        "  color: #e0a96d;"
        "  border: 1px solid #e0a96d;"
        "  border-radius: 6px;"
        "  padding: 5px 15px;"
        "  font-family: 'Segoe UI';"
        "  font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e0a96d;"
        "  color: #ffffff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #c48f56;"
        "  color: #ffffff;"
        "}";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }

    if (ui->pushButtonCart) {
        ui->pushButtonCart->setStyleSheet(buttonStyle +
                                          "QPushButton { border: 2px solid #c48f56; font-weight: bold; }");
    }
    if (ui->pushButtonOrder) {
        ui->pushButtonOrder->setStyleSheet(buttonStyle +
                                           "QPushButton { border: 2px solid #c48f56; font-weight: bold; }");
    }
}


void MenuForm::setupContextMenu()
{
    m_contextMenu = new QMenu(this);

    QAction *editAction = new QAction("Редагувати", this);
    connect(editAction, &QAction::triggered, this, &MenuForm::on_pushButtonEdit_clicked);
    m_contextMenu->addAction(editAction);

    QAction *deleteAction = new QAction("Видалити", this);
    connect(deleteAction, &QAction::triggered, this, &MenuForm::on_pushButtonDelete_clicked);
    m_contextMenu->addAction(deleteAction);

    m_contextMenu->addSeparator();

    QAction *orderAction = new QAction("Додати до кошика", this);
    connect(orderAction, &QAction::triggered, this, &MenuForm::on_pushButtonOrder_clicked);
    m_contextMenu->addAction(orderAction);

    if (m_currentUser.getRole() != "Admin") {
        editAction->setVisible(false);
        deleteAction->setVisible(false);
    }
}

void MenuForm::loadDishes()
{
    m_currentDishes = Dish::getAllDishes();
    populateCategoryComboBox();
    applyFilters();
    ui->labelStatus->setText(QString("Завантажено %1 страв").arg(m_currentDishes.size()));
}

void MenuForm::updateTable(const QList<Dish>& dishes)
{
    ui->tableWidget->setRowCount(dishes.size());

    for (int i = 0; i < dishes.size(); ++i) {
        const Dish& dish = dishes[i];

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(dish.getId())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(dish.getName()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(dish.getCategory()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dish.getPrice(), 'f', 2)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(dish.getWeight(), 'f', 0)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(dish.getDescription()));
    }
}

void MenuForm::on_searchEdit_textChanged(const QString &text)
{
    filterDishes(text);
}

void MenuForm::filterDishes(const QString& text)
{
    if (text.isEmpty()) {
        loadDishes();
        return;
    }

    QList<Dish> filtered;

    for (const Dish& dish : m_currentDishes) {
        if (dish.getName().contains(text, Qt::CaseInsensitive) ||
            dish.getCategory().contains(text, Qt::CaseInsensitive) ||
            dish.getDescription().contains(text, Qt::CaseInsensitive)) {
            filtered.append(dish);
        }
    }

    updateTable(filtered);
    ui->labelStatus->setText(QString("Знайдено %1 страв").arg(filtered.size()));
    applyFilters();
}

void MenuForm::on_pushButtonAdd_clicked()
{
    bool ok = false;

    QString name = QInputDialog::getText(this, "Додати страву", "Назва страви:",
                                         QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    double price = QInputDialog::getDouble(this, "Додати страву", "Ціна (грн):",
                                           0, 0, 10000, 2, &ok);
    if (!ok || price <= 0) return;

    double weight = QInputDialog::getDouble(this, "Додати страву", "Вага (г):",
                                            0, 0, 10000, 0, &ok);
    if (!ok) return;

    QString category = QInputDialog::getText(this, "Додати страву", "Категорія:",
                                             QLineEdit::Normal, "", &ok);
    if (!ok || category.trimmed().isEmpty()) return;

    QString description = QInputDialog::getText(this, "Додати страву", "Опис:",
                                                QLineEdit::Normal, "", &ok);

    Dish newDish(-1, name, description, category, price, weight);
    if (newDish.save()) {
        loadDishes();
        emit statusMessage("Страву додано успішно!");
        QMessageBox::information(this, "Успіх", "Страву додано!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося додати страву!");
    }
}

void MenuForm::on_pushButtonEdit_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть страву для редагування!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->text().toInt();
    Dish dish = Dish::getById(id);

    bool ok = false;

    QString name = QInputDialog::getText(this, "Редагувати страву",
                                         "Назва страви:", QLineEdit::Normal, dish.getName(), &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    double price = QInputDialog::getDouble(this, "Редагувати страву",
                                           "Ціна (грн):", dish.getPrice(), 0, 10000, 2, &ok);
    if (!ok || price <= 0) return;

    double weight = QInputDialog::getDouble(this, "Редагувати страву",
                                            "Вага (г):", dish.getWeight(), 0, 10000, 0, &ok);
    if (!ok) return;

    QString category = QInputDialog::getText(this, "Редагувати страву",
                                             "Категорія:", QLineEdit::Normal, dish.getCategory(), &ok);
    if (!ok || category.trimmed().isEmpty()) return;

    QString description = QInputDialog::getText(this, "Редагувати страву",
                                                "Опис:", QLineEdit::Normal, dish.getDescription(), &ok);

    dish.setName(name);
    dish.setPrice(price);
    dish.setWeight(weight);
    dish.setCategory(category);
    dish.setDescription(description);

    if (dish.save()) {
        loadDishes();
        emit statusMessage("Страву оновлено!");
        QMessageBox::information(this, "Успіх", "Страву оновлено!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося оновити страву!");
    }
}

void MenuForm::on_pushButtonDelete_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть страву для видалення!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->text().toInt();
    QString name = ui->tableWidget->item(row, 1)->text();

    if (QMessageBox::question(this, "Видалення",
                              QString("Ви впевнені, що хочете видалити страву '%1'?")
                                  .arg(name),
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    Dish dish = Dish::getById(id);
    if (dish.remove()) {
        loadDishes();
        emit statusMessage("Страву видалено!");
        QMessageBox::information(this, "Успіх", "Страву видалено!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося видалити страву!");
    }
}

int MenuForm::pickClientId(bool& ok)
{
    if (m_currentUser.getRole() == "Client") {
        ok = true;
        return m_currentUser.getId();
    }

    QList<User> clients = User::getByRole("Client");
    if (clients.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "У базі немає жодного клієнта!");
        ok = false;
        return -1;
    }

    QStringList names;
    for (const User& client : clients) {
        names << QString("%1 (%2)").arg(client.getName(), client.getPhone());
    }

    QString selected = QInputDialog::getItem(this, "Вибір клієнта",
                                             "Клієнт:", names, 0, false, &ok);
    if (!ok || selected.isEmpty()) {
        ok = false;
        return -1;
    }

    int index = names.indexOf(selected);
    ok = true;
    return clients[index].getId();
}

int MenuForm::assignLeastBusyWaiter()
{
    QList<User> waiters = User::getByRole("Waiter");
    if (waiters.isEmpty()) {
        return -1;
    }

    int bestWaiterId = -1;
    int bestLoad = -1;

    for (const User& waiter : waiters) {
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM Orders WHERE WaiterID = ? AND Status IN ('New', 'InProgress')");
        query.addBindValue(waiter.getId());

        int currentLoad = 0;
        if (query.exec() && query.next()) {
            currentLoad = query.value(0).toInt();
        }

        if (bestWaiterId == -1 || currentLoad < bestLoad) {
            bestWaiterId = waiter.getId();
            bestLoad = currentLoad;
        }
    }

    return bestWaiterId;
}

void MenuForm::on_pushButtonOrder_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть страву для додавання в кошик!");
        return;
    }

    int dishId = ui->tableWidget->item(row, 0)->text().toInt();
    QString dishName = ui->tableWidget->item(row, 1)->text();
    double price = ui->tableWidget->item(row, 3)->text().toDouble();

    AddToCartDialog dialog(dishName, this);
    if (dialog.exec() != QDialog::Accepted) return;

    int quantity = dialog.getQuantity();
    bool dineIn = dialog.isDineIn();

    bool found = false;
    for (auto& item : m_cartItems) {
        if (item.dishId == dishId) {
            item.quantity += quantity;
            found = true;
            break;
        }
    }

    if (!found) {
        m_cartItems.append({dishId, dishName, quantity, price});
    }

    QString serveText = dineIn ? "на місці" : "на виніс";
    QString statusStr = QString("Додано: %1 (x%2, %3). Всього у кошику страв: %4")
                            .arg(dishName).arg(quantity).arg(serveText).arg(m_cartItems.size());
    ui->labelStatus->setText(statusStr);
    emit statusMessage(statusStr);

    QMessageBox::information(this, "Кошик оновлено",
                             QString("'%1' у кількості %2 порцій (%3) успішно додано до кошика!\n\n"
                                     "Натисніть кнопку 'Мій кошик' внизу екрану для відправки замовлення.")
                                 .arg(dishName).arg(quantity).arg(serveText));
}

void MenuForm::on_pushButtonCart_clicked()
{
    if (m_cartItems.isEmpty()) {
        QMessageBox::warning(this, "Кошик порожній", "Ви ще не додали жодної страви до кошика!");
        return;
    }

    QString cartContent = "Вміст вашого поточного кошика:\n\n";
    double grandTotal = 0.0;

    for (const auto& item : m_cartItems) {
        double subtotal = item.price * item.quantity;
        cartContent += QString("• %1  —  %2 шт. х %3 грн = %4 грн\n")
                           .arg(item.dishName)
                           .arg(item.quantity)
                           .arg(QString::number(item.price, 'f', 2))
                           .arg(QString::number(subtotal, 'f', 2));
        grandTotal += subtotal;
    }

    cartContent += QString("\nЗагальна вартість: %1 грн\n\nБажаєте надіслати замовлення на кухню?").arg(QString::number(grandTotal, 'f', 2));

    auto reply = QMessageBox::question(this, "Підтвердження замовлення", cartContent,
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Reset);

    if (reply == QMessageBox::Reset) {
        m_cartItems.clear();
        ui->labelStatus->setText("Кошик очищено");
        QMessageBox::information(this, "Кошик", "Кошик повністю очищено.");
        return;
    }

    if (reply != QMessageBox::Yes) return;

    bool ok = false;
    int tableNumber = QInputDialog::getInt(this, "Оформлення",
                                           "Введіть номер вашого столика:", 1, 1, 99, 1, &ok);
    if (!ok || tableNumber <= 0) return;

    QSqlQuery query;
    query.prepare("SELECT ID_Table, IsAvailable FROM Tables WHERE TableNumber = ?");
    query.addBindValue(tableNumber);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Помилка", "Столик з номером " + QString::number(tableNumber) + " не знайдено!");
        return;
    }
    int tableId = query.value(0).toInt();
    bool tableAvailable = query.value(1).toBool();

    if (!tableAvailable) {
        if (QMessageBox::question(this, "Столик зайнятий",
                                  QString("Столик №%1 позначений як зайнятий. Продовжити оформлення?")
                                      .arg(tableNumber),
                                  QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
            return;
        }
    }

    int clientId = pickClientId(ok);
    if (!ok) return;

    int waiterId;
    if (m_currentUser.getRole() == "Client") {
        waiterId = assignLeastBusyWaiter();
        if (waiterId == -1) {
            QMessageBox::warning(this, "Помилка",
                                 "У системі немає жодного офіціанта для обслуговування замовлення!\n"
                                 "Зверніться до адміністратора.");
            return;
        }
    } else {
        waiterId = m_currentUser.getId();
    }

    Order newOrder(-1, tableId, clientId, waiterId, QDateTime::currentDateTime(), "New", 0.0);

    if (!newOrder.save()) {
        QMessageBox::critical(this, "Помилка", "Критична помилка бази даних! Не вдалося створити замовлення.");
        return;
    }

    for (const auto& item : m_cartItems) {
        newOrder.addItem(item.dishId, item.quantity, item.price);
    }

    newOrder.setTotal(grandTotal);
    newOrder.save();

    QString waiterName = User::getById(waiterId).getName();
    int itemsCount = m_cartItems.size();
    m_cartItems.clear();
    ui->labelStatus->setText("Замовлення успішно надіслано!");

    emit statusMessage(QString("Замовлення #%1 успішно згенеровано для столика №%2! Офіціант: %3")
                           .arg(newOrder.getId()).arg(tableNumber).arg(waiterName));

    QMessageBox::information(this, "Успіх",
                             QString("Вітаємо! Ваше пакетне замовлення #%1 успішно зареєстровано.\n"
                                     "Кількість унікальних страв: %2\n"
                                     "Загальна сума до сплати: %3 грн.\n"
                                     "Вас обслуговуватиме офіціант: %4\n"
                                     "Очікуйте на подачу за столиком №%5!")
                                 .arg(newOrder.getId())
                                 .arg(itemsCount)
                                 .arg(QString::number(grandTotal, 'f', 2))
                                 .arg(waiterName)
                                 .arg(tableNumber));
}

void MenuForm::on_pushButtonExport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Експорт меню",
                                                    "menu.csv", "CSV (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося створити файл!");
        return;
    }

    QTextStream out(&file);
    out << "ID;Назва;Категорія;Ціна;Вага;Опис\n";

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        out << ui->tableWidget->item(i, 0)->text() << ";"
            << ui->tableWidget->item(i, 1)->text() << ";"
            << ui->tableWidget->item(i, 2)->text() << ";"
            << ui->tableWidget->item(i, 3)->text() << ";"
            << ui->tableWidget->item(i, 4)->text() << ";"
            << ui->tableWidget->item(i, 5)->text() << "\n";
    }

    file.close();
    emit statusMessage("Меню експортовано в " + fileName);
    QMessageBox::information(this, "Успіх", "Меню експортовано!");
}

void MenuForm::on_pushButtonImport_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Імпорт меню",
                                                    "", "CSV (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося відкрити файл!");
        return;
    }

    QTextStream in(&file);
    QString header = in.readLine();
    Q_UNUSED(header);

    int count = 0;
    int skipped = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        QStringList parts = line.split(";");
        if (parts.size() < 6) {
            skipped++;
            continue;
        }

        bool priceOk = false, weightOk = false;
        double price = parts[3].toDouble(&priceOk);
        double weight = parts[4].toDouble(&weightOk);

        if (!priceOk || parts[1].trimmed().isEmpty()) {
            skipped++;
            continue;
        }
        if (!weightOk) {
            weight = 0.0;
        }

        Dish dish(-1, parts[1], parts[5], parts[2], price, weight);
        if (dish.save()) {
            count++;
        } else {
            skipped++;
        }
    }

    file.close();
    loadDishes();
    emit statusMessage(QString("Імпортовано %1 страв").arg(count));

    QString resultMsg = QString("Імпортовано %1 страв!").arg(count);
    if (skipped > 0) {
        resultMsg += QString("\nПропущено рядків з помилками: %1").arg(skipped);
    }
    QMessageBox::information(this, "Успіх", resultMsg);
}

void MenuForm::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tableWidget->currentRow() < 0) return;
    m_contextMenu->exec(ui->tableWidget->viewport()->mapToGlobal(pos));
}

void MenuForm::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if (m_currentUser.getRole() == "Admin") {
        on_pushButtonEdit_clicked();
    } else {
        on_pushButtonOrder_clicked();
    }
}