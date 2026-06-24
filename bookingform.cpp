#include "BookingForm.h"
#include "ui_bookingform.h"
#include "BookingDialog.h"
#include "Table.h"
#include "User.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QAction>
#include <QHeaderView>

BookingForm::BookingForm(const User& user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookingForm)
    , m_currentUser(user)
{
    ui->setupUi(this);

    setupTable();
    setupContextMenu();

    ui->statusComboBox->clear();
    ui->statusComboBox->addItems(QStringList() << "Всі статуси" << "Очікує" << "Підтверджено" << "Скасовано");

    if (m_currentUser.getRole() == "Client") {
        ui->statusComboBox->setCurrentText("Усі");
    }

    loadBookings();
    setDarkTheme();
}

BookingForm::~BookingForm()
{
    delete ui;
}

void BookingForm::setDarkTheme()
{
    this->setStyleSheet("background-color: #1a1a1e; color: #ffffff;");
    if (ui->frame) ui->frame->setStyleSheet("background-color: #111114; border-bottom: 1px solid #333;");

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #D1B26D; color: #1a1a1e; }";
    ui->tableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #242428; color: #ffffff; border: 1px solid #555555; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #242428; color: #ffffff; selection-background-color: #D1B26D; selection-color: #1a1a1e; }";
    ui->searchEdit->setStyleSheet(inputStyle);
    ui->statusComboBox->setStyleSheet(inputStyle);

    QString labelStyle = "QLabel { color: #cccccc; }";
    ui->label->setStyleSheet(labelStyle);
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

void BookingForm::setLightTheme()
{
    this->setStyleSheet("background-color: #f5f5f7; color: #1a1a1e;");
    if (ui->frame) ui->frame->setStyleSheet("background-color: #ffffff; border-bottom: 1px solid #dddddd;");

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: #e0a96d; color: #ffffff; }";
    ui->tableWidget->setStyleSheet(tableStyle);

    QString inputStyle =
        "QLineEdit, QComboBox { background-color: #ffffff; color: #1a1a1e; border: 1px solid #cccccc; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; color: #1a1a1e; selection-background-color: #e0a96d; selection-color: #ffffff; }";
    ui->searchEdit->setStyleSheet(inputStyle);
    ui->statusComboBox->setStyleSheet(inputStyle);

    QString labelStyle = "QLabel { color: #333333; }";
    ui->label->setStyleSheet(labelStyle);
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

void BookingForm::setupTable()
{
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels({
        "ID", "Стіл", "Клієнт", "Телефон", "Дата", "Час (Тр.)", "Статус"
    });
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void BookingForm::setupContextMenu()
{
    m_contextMenu = new QMenu(this);

    QAction *confirmAction = new QAction("Підтвердити бронювання", this);
    QAction *cancelAction = new QAction("Скасувати бронювання", this);
    QAction *deleteAction = new QAction("Видалити запис", this);

    connect(confirmAction, &QAction::triggered, this, &BookingForm::onConfirmBooking);
    connect(cancelAction, &QAction::triggered, this, &BookingForm::onCancelBooking);
    connect(deleteAction, &QAction::triggered, this, &BookingForm::onDeleteBooking);

    if (m_currentUser.getRole() != "Client") {
        m_contextMenu->addAction(confirmAction);
    }
    m_contextMenu->addAction(cancelAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(deleteAction);
}

void BookingForm::on_searchEdit_textChanged(const QString &text)
{
    Q_UNUSED(text);
    filterBookings();
}

void BookingForm::on_statusComboBox_currentTextChanged(const QString &text)
{
    Q_UNUSED(text);
    filterBookings();
}

void BookingForm::on_addPushButton_clicked()
{
    onAddBooking();
}

void BookingForm::on_exportPushButton_clicked()
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::warning(this, "Помилка", "Немає даних для експорту!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Експорт у CSV", "bookings_export.csv", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка", "Не вдалося створити файл для запису.");
        return;
    }

    QTextStream out(&file);
    out << u'\ufeff';
    out << "ID;Номер столика;Клієнт;Телефон;Дата;Час;Тривалість;Статус\n";

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (ui->tableWidget->isRowHidden(i)) continue;

        out << ui->tableWidget->item(i, 0)->text() << ";"
            << ui->tableWidget->item(i, 1)->text() << ";"
            << ui->tableWidget->item(i, 2)->text() << ";"
            << ui->tableWidget->item(i, 3)->text() << ";"
            << ui->tableWidget->item(i, 4)->text() << ";"
            << ui->tableWidget->item(i, 5)->text() << ";"
            << ui->tableWidget->item(i, 6)->text() << "\n";
    }

    file.close();
    emit statusMessage("Бронювання експортовано в " + fileName);
    QMessageBox::information(this, "Успіх", "Дані успішно експортовано!");
}

void BookingForm::on_tableWidget_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    ui->tableWidget->selectRow(row);
    onConfirmBooking();
}

void BookingForm::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tableWidget->itemAt(pos)) {
        m_contextMenu->exec(ui->tableWidget->mapToGlobal(pos));
    }
}

void BookingForm::loadBookings()
{
    QList<Booking> bookings;
    if (m_currentUser.getRole() == "Client") {
        bookings = Booking::getBookingsByClient(m_currentUser.getId());
    } else {
        bookings = Booking::getAllBookings();
    }
    updateTable(bookings);
}

void BookingForm::updateTable(const QList<Booking>& bookings)
{
    ui->tableWidget->setRowCount(0);

    for (const auto& booking : bookings) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        Table table = Table::getById(booking.getTableId());
        User client = User::getById(booking.getClientId());

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(booking.getId())));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString("№%1").arg(table.getNumber())));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(client.getName()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(client.getPhone()));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(booking.getDate().toString("dd.MM.yyyy")));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString("%1 (%2г)").arg(booking.getTime().toString("HH:mm")).arg(booking.getDuration())));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(getStatusText(booking.getStatus())));

        ui->tableWidget->item(row, 0)->setData(Qt::UserRole, booking.getId());
    }
    emit statusMessage(QString("Завантажено %1 записів.").arg(bookings.size()));
}

void BookingForm::filterBookings()
{
    QString searchText = ui->searchEdit->text().trimmed().toLower();
    QString filterStatus = ui->statusComboBox->currentText();

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        QString clientName = ui->tableWidget->item(i, 2)->text().toLower();
        QString currentStatusText = ui->tableWidget->item(i, 6)->text();

        bool matchesSearch = searchText.isEmpty() || clientName.contains(searchText);
        bool matchesStatus = (filterStatus == "Усі") || (currentStatusText == filterStatus);

        ui->tableWidget->setRowHidden(i, !(matchesSearch && matchesStatus));
    }
}

void BookingForm::onAddBooking()
{
    BookingDialog dialog(m_currentUser, -1, this);
    if (dialog.exec() == QDialog::Accepted) {
        Booking booking;
        booking.setTableId(dialog.getSelectedTableId());
        booking.setClientId(dialog.getSelectedClientId());
        booking.setDate(dialog.getSelectedDate());
        booking.setTime(dialog.getSelectedTime());
        booking.setDuration(dialog.getSelectedDuration());
        booking.setStatus("Pending");

        if (booking.save()) {
            emit statusMessage(QString("Бронювання #%1 створено!").arg(booking.getId()));
            QMessageBox::information(this, "Успіх", "Бронювання успішно надіслано!");
            loadBookings();
        } else {
            QMessageBox::critical(this, "Помилка", "Не вдалося зберегти в БД.");
        }
    }
}

void BookingForm::onConfirmBooking()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть бронювання для підтвердження!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();
    Booking booking = Booking::getById(id);

    if (booking.getStatus() == "Cancelled") {
        QMessageBox::warning(this, "Помилка", "Скасоване бронювання не можна підтвердити!");
        return;
    }
    if (booking.getStatus() == "Confirmed") {
        QMessageBox::information(this, "Інформація", "Бронювання вже підтверджено.");
        return;
    }

    booking.setStatus("Confirmed");
    if (booking.save()) {
        Table table = Table::getById(booking.getTableId());
        table.setAvailable(false);
        table.save();
        loadBookings();
        emit statusMessage(QString("Бронювання #%1 підтверджено, столик №%2 заброньовано")
                               .arg(id).arg(table.getNumber()));
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося підтвердити бронювання!");
    }
}

void BookingForm::onCancelBooking()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть бронювання для скасування!");
        return;
    }

    int id = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();
    Booking booking = Booking::getById(id);

    if (booking.getStatus() == "Cancelled") {
        QMessageBox::information(this, "Інформація", "Бронювання вже скасоване.");
        return;
    }

    if (QMessageBox::question(this, "Підтвердження",
                              QString("Скасувати бронювання #%1?").arg(id),
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    QString oldStatus = booking.getStatus();
    booking.setStatus("Cancelled");
    if (booking.save()) {
        if (oldStatus == "Confirmed") {
            Table table = Table::getById(booking.getTableId());
            table.setAvailable(true);
            table.save();
        }
        loadBookings();
        emit statusMessage("Бронювання #" + QString::number(id) + " скасовано!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося скасувати бронювання!");
    }
}

void BookingForm::onDeleteBooking()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Помилка", "Виберіть бронювання для видалення!");
        return;
    }

    if (QMessageBox::question(this, "Підтвердження", "Видалити це бронювання?") != QMessageBox::Yes) return;

    int id = ui->tableWidget->item(row, 0)->data(Qt::UserRole).toInt();
    Booking booking = Booking::getById(id);

    bool wasConfirmed = (booking.getStatus() == "Confirmed");
    int tableId = booking.getTableId();

    if (booking.remove()) {
        if (wasConfirmed) {
            Table table = Table::getById(tableId);
            table.setAvailable(true);
            table.save();
        }
        loadBookings();
        emit statusMessage("Бронювання #" + QString::number(id) + " видалено!");
    } else {
        QMessageBox::critical(this, "Помилка", "Не вдалося видалити бронювання!");
    }
}

QString BookingForm::getStatusText(const QString& status)
{
    if (status == "Pending") return "Очікує";
    if (status == "Confirmed") return "Підтверджено";
    if (status == "Cancelled") return "Скасовано";
    return status;
}