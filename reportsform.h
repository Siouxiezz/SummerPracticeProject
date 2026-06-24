#ifndef REPORTSFORM_H
#define REPORTSFORM_H

#include <QWidget>
#include <QDate>
#include "User.h"

namespace Ui {
class ReportsForm;
}

class ReportsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsForm(const User& user, QWidget *parent = nullptr);
    ~ReportsForm();

    void setDarkTheme();
    void setLightTheme();

signals:
    void statusMessage(const QString& message);

private slots:
    void on_pushButtonRevenue_clicked();
    void on_pushButtonPopular_clicked();
    void on_pushButtonTablesStats_clicked();

    void on_btnToday_clicked();
    void on_btnWeek_clicked();
    void on_btnMonth_clicked();
    void on_btnYear_clicked();

    void on_btnExportRevenue_clicked();
    void on_btnExportPopular_clicked();
    void on_tablesStatsRefreshPushButton_clicked();
    void on_tablesStatsExportPushButton_clicked();

private:
    void setupUiDesign();
    void calculateRevenue(const QDate& startDate, const QDate& endDate, const QString& periodName);
    void calculatePopularDishes();
    void calculateTablesStats();

    Ui::ReportsForm *ui;
    User m_currentUser;
    QDate m_currentStartDate;
    QDate m_currentEndDate;
};

#endif // REPORTSFORM_H