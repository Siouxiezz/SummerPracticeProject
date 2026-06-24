#include "RegisterForm.h"
#include "ui_registerform.h"
#include "DatabaseManager.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QSqlQuery>

RegisterForm::RegisterForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterForm)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->statusLabel->setText("Заповніть всі поля для реєстрації");

    ui->roleCombo->clear();
    ui->roleCombo->addItem("Клієнт", "Client");
    ui->roleCombo->addItem("Офіціант", "Waiter");

    QRegularExpressionValidator *phoneValidator = new QRegularExpressionValidator(
        QRegularExpression("^\\+?3?8?0?\\d{9}$"), this);
    ui->phoneEdit->setValidator(phoneValidator);

    ui->statusLabel->setText("Заповніть всі поля для реєстрації");

    connect(ui->loginEdit, &QLineEdit::textChanged,
            this, &RegisterForm::validateFields);
    connect(ui->passwordEdit, &QLineEdit::textChanged,
            this, &RegisterForm::validateFields);
    connect(ui->confirmEdit, &QLineEdit::textChanged,
            this, &RegisterForm::validateFields);
    connect(ui->nameEdit, &QLineEdit::textChanged,
            this, &RegisterForm::validateFields);
    connect(ui->phoneEdit, &QLineEdit::textChanged,
            this, &RegisterForm::validateFields);
}


RegisterForm::~RegisterForm()
{
    delete ui;
}

QString RegisterForm::getUsername() const
{
    return ui->loginEdit->text().trimmed();
}

void RegisterForm::on_pushButtonRegister_clicked()
{
    if (!validateInput()) {
        return;
    }

    User newUser;
    newUser.setUsername(ui->loginEdit->text().trimmed());
    newUser.setPassword(ui->passwordEdit->text());
    newUser.setName(ui->nameEdit->text().trimmed());
    newUser.setPhone(ui->phoneEdit->text().trimmed());
    newUser.setRole(ui->roleCombo->currentData().toString());

    if (!newUser.save()) {
        QMessageBox::critical(this, "Помилка", "Не вдалося зареєструвати користувача!");
        return;
    }

    QMessageBox::information(this, "Успіх",
                             QString("Реєстрація пройшла успішно!\n"
                                     "Логін: %1\n"
                                     "Роль: %2")
                                 .arg(newUser.getUsername())
                                 .arg(newUser.getRole()));

    accept();
}

void RegisterForm::on_pushButtonCancel_clicked()
{
    reject();
}

void RegisterForm::on_lineEditUsername_textChanged(const QString &text)
{
    Q_UNUSED(text);
    validateFields();
}

void RegisterForm::on_lineEditPassword_textChanged(const QString &text)
{
    Q_UNUSED(text);
    validateFields();
}

void RegisterForm::on_lineEditConfirm_textChanged(const QString &text)
{
    Q_UNUSED(text);
    validateFields();
}

void RegisterForm::on_lineEditName_textChanged(const QString &text)
{
    Q_UNUSED(text);
    validateFields();
}

void RegisterForm::validateFields()
{
    bool valid = !ui->loginEdit->text().trimmed().isEmpty() &&
                 !ui->passwordEdit->text().isEmpty() &&
                 !ui->confirmEdit->text().isEmpty() &&
                 !ui->nameEdit->text().trimmed().isEmpty() &&
                 ui->passwordEdit->text() == ui->confirmEdit->text() &&
                 ui->passwordEdit->text().length() >= 6;

    if (valid && isUsernameExists(ui->loginEdit->text().trimmed())) {
        valid = false;
        ui->loginEdit->setStyleSheet("border: 2px solid red;");
    } else {
        ui->loginEdit->setStyleSheet("");
    }

    ui->pushButtonRegister->setEnabled(valid);
    ui->pushButtonRegister->setToolTip(valid ? "Натисніть для завершення реєстрації"
                                             : "Заповніть всі поля правильно");
}

bool RegisterForm::validateInput()
{
    QString username = ui->loginEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString confirm = ui->confirmEdit->text();
    QString name = ui->nameEdit->text().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Введіть логін!");
        ui->loginEdit->setFocus();
        return false;
    }

    if (username.length() < 3) {
        QMessageBox::warning(this, "Помилка", "Логін повинен містити не менше 3 символів!");
        ui->loginEdit->setFocus();
        return false;
    }

    if (isUsernameExists(username)) {
        QMessageBox::warning(this, "Помилка", "Користувач з таким логіном вже існує!");
        ui->loginEdit->setFocus();
        return false;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "Помилка", "Пароль повинен містити не менше 6 символів!");
        ui->passwordEdit->setFocus();
        return false;
    }

    if (password != confirm) {
        QMessageBox::warning(this, "Помилка", "Паролі не співпадають!");
        ui->confirmEdit->setFocus();
        return false;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Введіть ваше ім'я!");
        ui->nameEdit->setFocus();
        return false;
    }

    if (!phone.isEmpty()) {
        QRegularExpressionValidator validator(QRegularExpression("^\\+?3?8?0?\\d{9}$"));
        int pos = 0;
        if (validator.validate(phone, pos) != QValidator::Acceptable) {
            QMessageBox::warning(this, "Помилка", "Невірний формат телефону!\nПриклад: +380501234567");
            ui->phoneEdit->setFocus();
            return false;
        }
    }

    return true;
}

bool RegisterForm::isUsernameExists(const QString& username)
{
    DatabaseManager& db = DatabaseManager::instance();
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Users WHERE Username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}