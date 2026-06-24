#include "Booking.h"
#include "DatabaseManager.h"
#include "User.h"
#include "Table.h"
#include <QSqlQuery>
#include <QVariant>

Booking::Booking(int id, int tableId, int clientId,
                 const QDate& date, const QTime& time,
                 int duration, const QString& status)
    : m_id(id), m_tableId(tableId), m_clientId(clientId),
    m_date(date), m_time(time), m_duration(duration), m_status(status) {}

bool Booking::save() {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;

    if (m_id == -1) {
        query.prepare("INSERT INTO Bookings (TableID, ClientID, BookingDate, BookingTime, Duration, Status) "
                      "VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(m_tableId);
        query.addBindValue(m_clientId);
        query.addBindValue(m_date.toString("yyyy-MM-dd"));
        query.addBindValue(m_time.toString("HH:mm"));
        query.addBindValue(m_duration);
        query.addBindValue(m_status);

        if (!query.exec()) {
            qDebug() << "Помилка збереження бронювання:" << query.lastError().text();
            return false;
        }
        m_id = query.lastInsertId().toInt();
    } else {
        query.prepare("UPDATE Bookings SET TableID=?, ClientID=?, BookingDate=?, BookingTime=?, "
                      "Duration=?, Status=? WHERE ID_Booking=?");
        query.addBindValue(m_tableId);
        query.addBindValue(m_clientId);
        query.addBindValue(m_date.toString("yyyy-MM-dd"));
        query.addBindValue(m_time.toString("HH:mm"));
        query.addBindValue(m_duration);
        query.addBindValue(m_status);
        query.addBindValue(m_id);

        if (!query.exec()) {
            qDebug() << "Помилка оновлення бронювання:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool Booking::load(int id) {
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Bookings WHERE ID_Booking = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    m_id = query.value("ID_Booking").toInt();
    m_tableId = query.value("TableID").toInt();
    m_clientId = query.value("ClientID").toInt();
    m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
    m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
    m_duration = query.value("Duration").toInt();
    m_status = query.value("Status").toString();

    return true;
}

bool Booking::remove() {
    if (m_id == -1) return false;

    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("DELETE FROM Bookings WHERE ID_Booking = ?");
    query.addBindValue(m_id);

    return query.exec();
}

QString Booking::toString() const {
    return QString("Бронювання #%1: столик %2, %3 о %4")
        .arg(m_id).arg(m_tableId).arg(m_date.toString("dd.MM.yyyy")).arg(m_time.toString("HH:mm"));
}

QList<Booking> Booking::getAllBookings() {
    QList<Booking> bookings;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query("SELECT * FROM Bookings ORDER BY BookingDate, BookingTime");

    while (query.next()) {
        Booking booking;
        booking.m_id = query.value("ID_Booking").toInt();
        booking.m_tableId = query.value("TableID").toInt();
        booking.m_clientId = query.value("ClientID").toInt();
        booking.m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
        booking.m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
        booking.m_duration = query.value("Duration").toInt();
        booking.m_status = query.value("Status").toString();
        bookings.append(booking);
    }
    return bookings;
}

QList<Booking> Booking::getBookingsByClient(int clientId) {
    QList<Booking> bookings;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Bookings WHERE ClientID = ? ORDER BY BookingDate, BookingTime");
    query.addBindValue(clientId);
    query.exec();

    while (query.next()) {
        Booking booking;
        booking.m_id = query.value("ID_Booking").toInt();
        booking.m_tableId = query.value("TableID").toInt();
        booking.m_clientId = query.value("ClientID").toInt();
        booking.m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
        booking.m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
        booking.m_duration = query.value("Duration").toInt();
        booking.m_status = query.value("Status").toString();
        bookings.append(booking);
    }
    return bookings;
}

QList<Booking> Booking::getBookingsByTable(int tableId) {
    QList<Booking> bookings;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Bookings WHERE TableID = ? ORDER BY BookingDate, BookingTime");
    query.addBindValue(tableId);
    query.exec();

    while (query.next()) {
        Booking booking;
        booking.m_id = query.value("ID_Booking").toInt();
        booking.m_tableId = query.value("TableID").toInt();
        booking.m_clientId = query.value("ClientID").toInt();
        booking.m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
        booking.m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
        booking.m_duration = query.value("Duration").toInt();
        booking.m_status = query.value("Status").toString();
        bookings.append(booking);
    }
    return bookings;
}

QList<Booking> Booking::getBookingsByStatus(const QString& status) {
    QList<Booking> bookings;
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT * FROM Bookings WHERE Status = ? ORDER BY BookingDate, BookingTime");
    query.addBindValue(status);
    query.exec();

    while (query.next()) {
        Booking booking;
        booking.m_id = query.value("ID_Booking").toInt();
        booking.m_tableId = query.value("TableID").toInt();
        booking.m_clientId = query.value("ClientID").toInt();
        booking.m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
        booking.m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
        booking.m_duration = query.value("Duration").toInt();
        booking.m_status = query.value("Status").toString();
        bookings.append(booking);
    }
    return bookings;
}

Booking Booking::getById(int id) {
    Booking booking;
    booking.load(id);
    return booking;
}

QList<Booking> Booking::getBookingsWithDetails() {
    QList<Booking> bookings;
    DatabaseManager& db = DatabaseManager::instance();

    QSqlQuery query(
        "SELECT b.ID_Booking, b.BookingDate, b.BookingTime, b.Duration, b.Status, "
        "t.TableNumber, t.Capacity, "
        "u.Name as ClientName, u.Phone as ClientPhone "
        "FROM Bookings b "
        "JOIN Tables t ON b.TableID = t.ID_Table "
        "JOIN Users u ON b.ClientID = u.ID_User "
        "ORDER BY b.BookingDate, b.BookingTime"
        );

    while (query.next()) {
        Booking booking;
        booking.m_id = query.value("ID_Booking").toInt();
        booking.m_tableId = query.value("ID_Booking").toInt(); // тимчасово
        booking.m_clientId = query.value("ID_Booking").toInt(); // тимчасово
        booking.m_date = QDate::fromString(query.value("BookingDate").toString(), "yyyy-MM-dd");
        booking.m_time = QTime::fromString(query.value("BookingTime").toString(), "HH:mm");
        booking.m_duration = query.value("Duration").toInt();
        booking.m_status = query.value("Status").toString();
        bookings.append(booking);
    }
    return bookings;
}