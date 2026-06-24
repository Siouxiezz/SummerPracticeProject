#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H
#include <QDialog>
#include <QDate>
#include <QTime>
#include "User.h"
#include "Table.h"
class BookingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BookingDialog(const User& currentUser, int bookingId = -1, QWidget *parent = nullptr);
    ~BookingDialog();
    int getSelectedTableId() const;
    int getSelectedClientId() const;
    QDate getSelectedDate() const;
    QTime getSelectedTime() const;
    int getSelectedDuration() const;
private:
    User m_currentUser;
    int m_bookingId;
};
#endif // BOOKINGDIALOG_H