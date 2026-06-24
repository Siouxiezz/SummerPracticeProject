#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QDialog>
#include "User.h"

namespace Ui {
class RegisterForm;
}

class RegisterForm : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterForm(QWidget *parent = nullptr);
    ~RegisterForm();

    QString getUsername() const;

private slots:
    void on_pushButtonRegister_clicked();
    void on_pushButtonCancel_clicked();
    void on_lineEditUsername_textChanged(const QString &text);
    void on_lineEditPassword_textChanged(const QString &text);
    void on_lineEditConfirm_textChanged(const QString &text);
    void on_lineEditName_textChanged(const QString &text);

private:
    bool validateInput();
    bool isUsernameExists(const QString& username);
    void validateFields();

    Ui::RegisterForm *ui;
};

#endif // REGISTERFORM_H