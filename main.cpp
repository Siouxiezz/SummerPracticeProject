#include "mainwindow.h"
#include "LoginForm.h"
#include "databasemanager.h"
#include "User.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::instance().openDatabase()) {
        return 1;
    }

    LoginForm loginWindow;
    if (loginWindow.exec() == QDialog::Accepted) {
        User authorizedUser = loginWindow.getCurrentUser();
        MainWindow mainSystem(authorizedUser);
        mainSystem.show();
        return QApplication::exec();
    }

    return 0;
}