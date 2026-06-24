#ifndef BOOKINGFORM_H
#define BOOKINGFORM_H

#include <QWidget>
#include <QMenu>
#include "User.h"
#include "Booking.h"

namespace Ui {
class BookingForm;
}

class BookingForm : public QWidget
{
    Q_OBJECT

public:
    explicit BookingForm(const User& user, QWidget *parent = nullptr);
    ~BookingForm();

    void setDarkTheme();
    void setLightTheme();

signals:
    void statusMessage(const QString& message);

private slots:
    void on_searchEdit_textChanged(const QString &text);
    void on_statusComboBox_currentTextChanged(const QString &text);
    void on_addPushButton_clicked();
    void on_exportPushButton_clicked();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void onConfirmBooking();
    void onCancelBooking();
    void onDeleteBooking();

private:
    void setupTable();
    void setupContextMenu();
    void loadBookings();
    void updateTable(const QList<Booking>& bookings);
    void filterBookings();
    void onAddBooking();
    QString getStatusText(const QString& status);

    Ui::BookingForm *ui;
    User m_currentUser;
    QMenu *m_contextMenu;
};

#endif // BOOKINGFORM_H