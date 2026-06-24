#ifndef BOOKING_H
#define BOOKING_H

#include "Entity.h"
#include <QDate>
#include <QTime>
#include <QString>
#include <QList>

class Booking : public Entity {
public:
    Booking(int id = -1, int tableId = -1, int clientId = -1,
            const QDate& date = QDate::currentDate(),
            const QTime& time = QTime::currentTime(),
            int duration = 2, const QString& status = "Pending");

    int getId() const override { return m_id; }
    int getTableId() const { return m_tableId; }
    int getClientId() const { return m_clientId; }
    QDate getDate() const { return m_date; }
    QTime getTime() const { return m_time; }
    int getDuration() const { return m_duration; }
    QString getStatus() const { return m_status; }

    void setId(int id) { m_id = id; }
    void setTableId(int tableId) { m_tableId = tableId; }
    void setClientId(int clientId) { m_clientId = clientId; }
    void setDate(const QDate& date) { m_date = date; }
    void setTime(const QTime& time) { m_time = time; }
    void setDuration(int duration) { m_duration = duration; }
    void setStatus(const QString& status) { m_status = status; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    static QList<Booking> getAllBookings();
    static QList<Booking> getBookingsByClient(int clientId);
    static QList<Booking> getBookingsByTable(int tableId);
    static QList<Booking> getBookingsByStatus(const QString& status);
    static Booking getById(int id);
    static QList<Booking> getBookingsWithDetails();

private:
    int m_id;
    int m_tableId;
    int m_clientId;
    QDate m_date;
    QTime m_time;
    int m_duration;
    QString m_status;
};

#endif // BOOKING_H