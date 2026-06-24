#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QButtonGroup>
#include <QLabel>
#include "User.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const User& user, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Меню та Палітра кольорів
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionSwitchTheme_triggered();

    void on_themeCheckBox_stateChanged(int state);

    void on_btnMenu_clicked();
    void on_btnOrders_clicked();
    void on_btnBookings_clicked();
    void on_btnAdmin_clicked();
    void on_btnReports_clicked();
    void on_btnInfo_clicked();

    void updateStatusBar(const QString& message);

private:
    void loadSettings();
    void saveSettings();
    void applyTheme(const QString& themeMode, const QString& customColor = "");
    QButtonGroup *navGroup;

    Ui::MainWindow *ui;
    User m_currentUser;
    QSettings *m_settings;
    QLabel *m_userStatusLabel;
};

#endif // MAINWINDOW_H