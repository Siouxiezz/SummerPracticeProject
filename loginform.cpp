#include "LoginForm.h"
#include "ui_loginform.h"
#include "RegisterForm.h"
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

LoginForm::LoginForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);

    QPixmap pix(":/image/img/RivoliLogo.jpeg");
    int w = ui->labelLogo->width();
    int h = ui->labelLogo->height();

    ui->labelLogo->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    setWindowTitle("Вітаємо в застосунку Grand Rivoli!");

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->roleComboBox->clear();
    ui->roleComboBox->addItem("Клієнт", "Client");
    ui->roleComboBox->addItem("Офіціант", "Waiter");
    ui->roleComboBox->addItem("Адміністратор", "Admin");
    ui->lineEditLogin->setPlaceholderText("Введіть логін...");
    ui->lineEditPassword->setPlaceholderText("Введіть пароль...");

    loadSettings();
    setStatus("Введіть дані для входу", "info");
}

LoginForm::~LoginForm()
{
    saveSettings();
    delete ui;
}

void LoginForm::setStatus(const QString& message, const QString& type)
{
    ui->statusLabel->setText(message);

    if (type == "error") {
        ui->statusLabel->setStyleSheet(
            "background-color: #f8d7da; "
            "border: 1px solid #dc3545; "
            "border-radius: 3px; "
            "padding: 3px 10px; "
            "font-size: 10pt; "
            "color: #721c24;");
    } else if (type == "success") {
        ui->statusLabel->setStyleSheet(
            "background-color: #d4edda; "
            "border: 1px solid #28a745; "
            "border-radius: 3px; "
            "padding: 3px 10px; "
            "font-size: 10pt; "
            "color: #155724;");
    } else if (type == "warning") {
        ui->statusLabel->setStyleSheet(
            "background-color: #fff3cd; "
            "border: 1px solid #ffc107; "
            "border-radius: 3px; "
            "padding: 3px 10px; "
            "font-size: 10pt; "
            "color: #856404;");
    } else {
        ui->statusLabel->setStyleSheet(
            "background-color: #2c2c30; "
            "border: 1px solid #4a4a4e; "
            "border-radius: 3px; "
            "padding: 3px 10px; "
            "font-size: 10pt; "
            "color: #d0d0d0;");
    }
}

void LoginForm::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditLogin->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();
    QString role = ui->roleComboBox->currentData().toString();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Помилка входу", "Будь ласка, заповніть всі поля!");
        return;
    }

    setStatus("Перевірка даних...", "info");

    m_currentUser = User::authenticate(username, password);

    if (m_currentUser.getId() == -1) {
        QMessageBox::warning(this, "Помилка входу", "Невірний логін або пароль!");
        setStatus("Введіть дані для входу", "info");
        return;
    }

    if (m_currentUser.getRole() != role) {
        QMessageBox::warning(this, "Помилка входу",
                             QString("Вибрана роль '%1' не відповідає вашій ролі '%2'!")
                                 .arg(ui->roleComboBox->currentText())
                                 .arg(m_currentUser.getRole()));
        setStatus("Введіть дані для входу", "info");
        return;
    }

    if (ui->rememberMeCheckBox->isChecked()) {
        saveSettings();
    }

    setStatus(QString("Вітаємо, %1! Вхід виконано!").arg(m_currentUser.getName()), "success");

    // Затримка перед закриттям
    QTimer::singleShot(500, this, &LoginForm::accept);
}

void LoginForm::on_pushButtonRegister_clicked()
{
    RegisterForm registerForm(this);
    if (registerForm.exec() == QDialog::Accepted) {
        QString username = registerForm.getUsername();
        if (!username.isEmpty()) {
            ui->lineEditLogin->setText(username);
            ui->lineEditPassword->clear();
            ui->lineEditPassword->setFocus();
            setStatus("Користувача зареєстровано! Введіть пароль.", "success");
        }
    }
}

void LoginForm::on_pushButtonExit_clicked()
{
    reject();
}

void LoginForm::on_rememberMeCheckBox_stateChanged(int state)
{
    if (state == Qt::Unchecked) {
        QSettings settings("Rivali", "GrandRivaliFlow");
        settings.remove("username");
        settings.remove("password");
        settings.remove("role");
        ui->lineEditLogin->clear();
        ui->lineEditPassword->clear();
        setStatus("Дані для входу видалено", "info");
    } else {
        setStatus("Дані будуть збережені при вході", "info");
    }
}

void LoginForm::loadSettings()
{
    QSettings settings("Rivali", "GrandRivaliFlow");
    QString username = settings.value("username").toString();
    QString password = settings.value("password").toString();
    QString role = settings.value("role", "Client").toString();

    if (!username.isEmpty() && !password.isEmpty()) {
        ui->lineEditLogin->setText(username);
        ui->lineEditPassword->setText(password);
        ui->rememberMeCheckBox->setChecked(true);

        int index = ui->roleComboBox->findData(role);
        if (index >= 0) {
            ui->roleComboBox->setCurrentIndex(index);
        }
        setStatus("Дані для входу завантажено", "info");
    }
}

void LoginForm::saveSettings()
{
    QSettings settings("Rivali", "GrandRivaliFlow");
    settings.setValue("username", ui->lineEditLogin->text().trimmed());
    settings.setValue("password", ui->lineEditPassword->text().trimmed());
    settings.setValue("role", ui->roleComboBox->currentData().toString());
}