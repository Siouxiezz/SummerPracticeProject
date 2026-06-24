#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include "User.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

    User getCurrentUser() const { return m_currentUser; }

private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonRegister_clicked();
    void on_pushButtonExit_clicked();
    void on_rememberMeCheckBox_stateChanged(int state);

private:
    void loadSettings();
    void saveSettings();
    void setStatus(const QString& message, const QString& type = "info");

    Ui::LoginForm *ui;
    User m_currentUser;
};

#endif // LOGINFORM_H