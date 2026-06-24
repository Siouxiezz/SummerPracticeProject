#include "BookingDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "Booking.h"

BookingDialog::BookingDialog(const User& currentUser, int bookingId, QWidget *parent)
    : QDialog(parent), m_currentUser(currentUser), m_bookingId(bookingId)
{
    setWindowTitle(m_bookingId == -1 ? "Додати бронювання" : "Редагувати бронювання");
    resize(350, 250);
    QFormLayout *layout = new QFormLayout(this);

    QComboBox *clientCombo = new QComboBox(this);
    QComboBox *tableCombo = new QComboBox(this);
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate(), this);
    QTimeEdit *timeEdit = new QTimeEdit(QTime::currentTime(), this);
    QSpinBox *durationSpin = new QSpinBox(this);

    dateEdit->setCalendarPopup(true);
    durationSpin->setRange(1, 12);
    durationSpin->setSuffix(" год.");
    durationSpin->setValue(2);

    if (m_currentUser.getRole() == "Client") {
        clientCombo->addItem(m_currentUser.getName(), m_currentUser.getId());
        clientCombo->setEnabled(false);
    } else {
        // Офіціант або Адмін вибирають серед усіх клієнтів
        QList<User> clients = User::getByRole("Client");
        for (const auto& client : clients) {
            clientCombo->addItem(client.getName(), client.getId());
        }
    }
    clientCombo->setObjectName("clientCombo");

    QList<Table> tables = Table::getAvailableTables();
    for (const auto& table : tables) {
        tableCombo->addItem(QString("Столик №%1 (%2 місць)").arg(table.getNumber()).arg(table.getCapacity()), table.getId());
    }
    tableCombo->setObjectName("tableCombo");

    if (m_bookingId != -1) {
        Booking b = Booking::getById(m_bookingId);
        dateEdit->setDate(b.getDate());
        timeEdit->setTime(b.getTime());
        durationSpin->setValue(b.getDuration());

        Table currentTable = Table::getById(b.getTableId());
        if (tableCombo->findData(currentTable.getId()) == -1) {
            tableCombo->addItem(QString("Столик №%1 (%2 місць)").arg(currentTable.getNumber()).arg(currentTable.getCapacity()), currentTable.getId());
        }

        int cIdx = clientCombo->findData(b.getClientId());
        if (cIdx != -1) clientCombo->setCurrentIndex(cIdx);

        int tIdx = tableCombo->findData(b.getTableId());
        if (tIdx != -1) tableCombo->setCurrentIndex(tIdx);
    }

    dateEdit->setObjectName("dateEdit");
    timeEdit->setObjectName("timeEdit");
    durationSpin->setObjectName("durationSpin");

    layout->addRow("Клієнт:", clientCombo);
    layout->addRow("Столик:", tableCombo);
    layout->addRow("Дата:", dateEdit);
    layout->addRow("Час:", timeEdit);
    layout->addRow("Тривалість:", durationSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this, tableCombo, clientCombo]() {
        if (tableCombo->currentIndex() == -1) {
            QMessageBox::warning(this, "Помилка", "Немає вільних столиків для вибору!");
            return;
        }
        if (clientCombo->currentIndex() == -1) {
            QMessageBox::warning(this, "Помилка", "Не вибрано клієнта!");
            return;
        }
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

BookingDialog::~BookingDialog()
{

}

int BookingDialog::getSelectedTableId() const
{
    return findChild<QComboBox*>("tableCombo")->currentData().toInt();
}
int BookingDialog::getSelectedClientId() const
{
    return findChild<QComboBox*>("clientCombo")->currentData().toInt();
}
QDate BookingDialog::getSelectedDate() const
{
    return findChild<QDateEdit*>("dateEdit")->date();
}
QTime BookingDialog::getSelectedTime() const
{
    return findChild<QTimeEdit*>("timeEdit")->time();
}
int BookingDialog::getSelectedDuration() const
{
    return findChild<QSpinBox*>("durationSpin")->value();
}