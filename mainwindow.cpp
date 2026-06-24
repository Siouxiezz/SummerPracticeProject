#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "menuform.h"
#include "OrdersForm.h"
#include "bookingform.h"
#include "AdminForm.h"
#include "reportsform.h"
#include "InfoForm.h"
#include <QPixmap>
#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>
#include <QCloseEvent>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(const User& user, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentUser(user)
{
    ui->setupUi(this);

    // QPixmap pix(":/image/img/RivoliLogo.jpeg");
    // int w = ui->labelLogo->width();
    // int h = ui->labelLogo->height();

    // ui->labelLogo->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    ui->labelLogo->setText("GR");
    ui->labelLogo->setStyleSheet(
        "QLabel {"
        "   color: #D4AF37;"
        "   font-size: 20pt;"
        "   font-weight: bold;"
        "   font-family: 'Book Antiqua', sans-serif;"
        "}"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(2, 2);
    ui->labelLogo->setGraphicsEffect(shadow);

    QString mainwindowButtonStyle =
        "QPushButton {"
        "  background-color: #1a1a1e;"
        "  color: #D1B26D;"
        "  border: 1px solid #D1B26D;"
        "  border-radius: 6px;"
        "  padding: 5px 15px;"
        "  font-family: 'Segoe UI';"
        "  font-size: 11pt;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background-color: #D1B26D;"
        "  color: #1a1a1e;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #bfa363;"
        "}";

    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(mainwindowButtonStyle);
    }

    setWindowTitle(QString("Grand Rivali Flow - %1 (%2)")
                       .arg(m_currentUser.getName())
                       .arg(m_currentUser.getRole()));

    m_settings = new QSettings("Rivali", "GrandRivaliFlow", this);

    navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);

    navGroup->addButton(ui->btnMenu, 0);
    navGroup->addButton(ui->btnOrders, 1);
    navGroup->addButton(ui->btnBookings, 2);
    navGroup->addButton(ui->btnAdmin, 3);
    navGroup->addButton(ui->btnReports, 4);
    navGroup->addButton(ui->btnInfo, 5);

    connect(navGroup, &QButtonGroup::idClicked, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    ui->btnMenu->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    MenuForm *menuForm = new MenuForm(m_currentUser, this);
    OrdersForm *ordersForm = new OrdersForm(m_currentUser, this);
    BookingForm *bookingsForm = new BookingForm(m_currentUser, this);
    AdminForm *adminForm = new AdminForm(m_currentUser, this);
    ReportsForm *reportsForm = new ReportsForm(m_currentUser, this);
    InfoForm *infoForm = new InfoForm(this);

    ui->stackedWidget->addWidget(menuForm);
    ui->stackedWidget->addWidget(ordersForm);
    ui->stackedWidget->addWidget(bookingsForm);
    ui->stackedWidget->addWidget(adminForm);
    ui->stackedWidget->addWidget(reportsForm);
    ui->stackedWidget->addWidget(infoForm);

    connect(menuForm, &MenuForm::statusMessage, this, &MainWindow::updateStatusBar);
    connect(ordersForm, &OrdersForm::statusMessage, this, &MainWindow::updateStatusBar);
    connect(bookingsForm, &BookingForm::statusMessage, this, &MainWindow::updateStatusBar);
    connect(adminForm, &AdminForm::statusMessage, this, &MainWindow::updateStatusBar);
    connect(reportsForm, &ReportsForm::statusMessage, this, &MainWindow::updateStatusBar);

    if (m_currentUser.getRole() == "Client") {
        ui->btnBookings->setVisible(false);
        ui->btnAdmin->setVisible(false);
        ui->btnReports->setVisible(false);
    } else if (m_currentUser.getRole() == "Waiter") {
        ui->btnAdmin->setVisible(false);
    }

    ui->statusBar->showMessage("Ласкаво просимо до Grand Rivali Flow!");
    m_userStatusLabel = new QLabel(QString("Користувач: %1 (%2)  ")
                                       .arg(m_currentUser.getName())
                                       .arg(m_currentUser.getRole()), this);
    ui->statusBar->addPermanentWidget(m_userStatusLabel);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    if (m_settings->contains("geometry")) {
        restoreGeometry(m_settings->value("geometry").toByteArray());
    }
    if (m_settings->contains("windowState")) {
        restoreState(m_settings->value("windowState").toByteArray());
    }

    QString themeMode = m_settings->value("themeMode", "dark").toString();
    QString customColor = m_settings->value("customThemeColor", "").toString();

    if (themeMode == "light") {
        ui->themeCheckBox->setChecked(true);
        applyTheme("light");
    } else if (themeMode == "custom" && !customColor.isEmpty()) {
        ui->themeCheckBox->setChecked(false);
        applyTheme("custom", customColor);
    } else {
        ui->themeCheckBox->setChecked(false);
        applyTheme("dark");
    }
}

void MainWindow::saveSettings()
{
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::updateStatusBar(const QString& message)
{
    ui->statusBar->showMessage(message, 5000);
}

void MainWindow::on_themeCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked) {
        m_settings->setValue("themeMode", "light");
        applyTheme("light");
    } else {
        m_settings->setValue("themeMode", "dark");
        applyTheme("dark");
    }
}

void MainWindow::on_actionSwitchTheme_triggered()
{
    QColor color = QColorDialog::getColor(Qt::darkGray, this, "Оберіть кастомний колір інтерфейсу");
    if (color.isValid()) {
        m_settings->setValue("themeMode", "custom");
        m_settings->setValue("customThemeColor", color.name());
        applyTheme("custom", color.name());
    }
}

void MainWindow::applyTheme(const QString& themeMode, const QString& customColor)
{
    bool isLight = (themeMode == "light");
    bool isCustom = (themeMode == "custom");

    QString mainWindowStyle;
    if (isLight) {
        mainWindowStyle =
            "QMainWindow { background-color: #FAFAFA; }"
            "QFrame#topPanel { background-color: #F5F5F5; border-bottom: 1px solid #E0E0E0; }"
            "QStatusBar { background-color: #F5F5F5; color: #212121; }"
            "QLabel { color: #212121; }";
        ui->themeCheckBox->setText("☀️ Світла тема");
    } else if (isCustom) {
        QColor base(customColor);
        mainWindowStyle = QString(
                              "QMainWindow { background-color: %1; }"
                              "QFrame#topPanel { background-color: %2; border-bottom: 1px solid %3; }"
                              "QStatusBar { background-color: %2; color: white; }"
                              "QLabel { color: white; }")
                              .arg(base.name())
                              .arg(base.darker(120).name())
                              .arg(base.lighter(120).name());
        ui->themeCheckBox->setText("🎨 Кастомна");
    } else {
        mainWindowStyle =
            "QMainWindow { background-color: #1a1a1e; }"
            "QFrame#topPanel { background-color: #111114; border-bottom: 2px solid #333333; }"
            "QStatusBar { background-color: #111114; color: #FFFFFF; }"
            "QLabel { color: #FFFFFF; }";
        ui->themeCheckBox->setText("🌙 Темна тема");
    }
    this->setStyleSheet(mainWindowStyle);

    QString navButtonStyle;
    if (isLight) {
        navButtonStyle =
            "QPushButton { background-color: #ffffff; color: #e0a96d; border: 1px solid #e0a96d; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
            "QPushButton:hover { background-color: #e0a96d; color: #ffffff; }"
            "QPushButton:checked { background-color: #e0a96d; color: #ffffff; }";
    } else if (isCustom) {
        QColor base(customColor);
        QString lighter = base.lighter(140).name();
        QString darker = base.darker(140).name();
        navButtonStyle = QString(
                             "QPushButton { background-color: %1; color: %2; border: 1px solid %2; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
                             "QPushButton:hover { background-color: %2; color: %1; }"
                             "QPushButton:checked { background-color: %2; color: %1; }")
                             .arg(base.name())
                             .arg(base.lighter(180).name());
    } else {
        navButtonStyle =
            "QPushButton { background-color: #242428; color: #D1B26D; border: 1px solid #D1B26D; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
            "QPushButton:hover { background-color: #D1B26D; color: #1a1a1e; }"
            "QPushButton:checked { background-color: #D1B26D; color: #1a1a1e; }";
    }

    ui->btnMenu->setStyleSheet(navButtonStyle);
    ui->btnOrders->setStyleSheet(navButtonStyle);
    ui->btnBookings->setStyleSheet(navButtonStyle);
    ui->btnAdmin->setStyleSheet(navButtonStyle);
    ui->btnReports->setStyleSheet(navButtonStyle);
    ui->btnInfo->setStyleSheet(navButtonStyle);

    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        QWidget* w = ui->stackedWidget->widget(i);
        if (!w) continue;

        if (isLight) {
            if (MenuForm* f = qobject_cast<MenuForm*>(w)) f->setLightTheme();
            else if (OrdersForm* f = qobject_cast<OrdersForm*>(w)) f->setLightTheme();
            else if (BookingForm* f = qobject_cast<BookingForm*>(w)) f->setLightTheme();
            else if (AdminForm* f = qobject_cast<AdminForm*>(w)) f->setLightTheme();
            else if (ReportsForm* f = qobject_cast<ReportsForm*>(w)) f->setLightTheme();
            else if (InfoForm* f = qobject_cast<InfoForm*>(w)) f->setLightTheme();
        } else {
            if (MenuForm* f = qobject_cast<MenuForm*>(w)) f->setDarkTheme();
            else if (OrdersForm* f = qobject_cast<OrdersForm*>(w)) f->setDarkTheme();
            else if (BookingForm* f = qobject_cast<BookingForm*>(w)) f->setDarkTheme();
            else if (AdminForm* f = qobject_cast<AdminForm*>(w)) f->setDarkTheme();
            else if (ReportsForm* f = qobject_cast<ReportsForm*>(w)) f->setDarkTheme();
            else if (InfoForm* f = qobject_cast<InfoForm*>(w)) f->setDarkTheme();
        }
    }

    if (isLight) updateStatusBar("Встановлено світлу тему");
    else if (isCustom) updateStatusBar("Застосовано кастомну палітру кольорів");
    else updateStatusBar("Встановлено темну тему");
}

void MainWindow::on_btnMenu_clicked()     { ui->stackedWidget->setCurrentIndex(0); }
void MainWindow::on_btnOrders_clicked()   { ui->stackedWidget->setCurrentIndex(1); }
void MainWindow::on_btnBookings_clicked() { ui->stackedWidget->setCurrentIndex(2); }
void MainWindow::on_btnAdmin_clicked()    { ui->stackedWidget->setCurrentIndex(3); }
void MainWindow::on_btnReports_clicked()  { ui->stackedWidget->setCurrentIndex(4); }
void MainWindow::on_btnInfo_clicked()     { ui->stackedWidget->setCurrentIndex(5); }

void MainWindow::on_actionExit_triggered()
{
    if (QMessageBox::question(this, "Вихід", "Ви дійсно хочете закрити застосунок?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        close();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Про застосунок", "<h2>Grand Rivali Flow</h2><p>Розробник: Трачук Д.О.</p>");
}