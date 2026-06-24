#include "InfoForm.h"
#include "ui_infoform.h"
#include <QHeaderView>

InfoForm::InfoForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoForm)
{
    ui->setupUi(this);

    QString infoformButtonStyle =
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
        btn->setStyleSheet(infoformButtonStyle);
    }

    setupAboutTab();
    setupContactsTab();
    setupHoursTab();

    ui->stackedWidget->setCurrentWidget(ui->pageAbout);
    setDarkTheme();
}

InfoForm::~InfoForm()
{
    delete ui;
}

void InfoForm::on_pushButtonAbout_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageAbout);
}

void InfoForm::on_pushButtonContacts_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageContacts);
}

void InfoForm::on_pushButtonHours_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageHours);
}

void InfoForm::setupAboutTab()
{
    QString html = R"(
        <h2 style="color:#ffe6ff;">Grand Rivali</h2>
        <p>Ресторан <b>Grand Rivoli</b> — місце, де класична європейська кухня
        зустрічається з теплою атмосферою домашнього вечора. Ми відкрилися
        з однією метою: дарувати гостям не просто їжу, а справжній досвід.</p>

        <p>Наша команда шеф-кухарів щодня готує страви з свіжих локальних
        продуктів, поєднуючи традиційні рецепти з сучасними тенденціями
        гастрономії.</p>

        <h3 style="color:#ffe6ff;">Наші переваги</h3>
        <ul>
            <li>Авторська кухня від шеф-повара з 15-річним досвідом</li>
            <li>Затишний інтер'єр і приватні зони для особливих подій</li>
            <li>Винна карта з понад 10 позицій</li>
            <li>Широкий асортимент алкоголю</li>
            <li>Можливість бронювання столиків онлайн</li>
        </ul>

        <p><i>Ми завжди раді бачити вас у Grand Rivali!</i></p>
    )";

    ui->aboutTextEdit->setHtml(html);
}

void InfoForm::setupContactsTab()
{

    QString html = R"(
        <h2 style="color:#ffe6ff;">Контакти</h2>

        <p><b>Адреса:</b><br>
        Вул. Незалежності 2, село Чагор, Чернівці, Чернівецька область</p>

        <p><b>Телефон:</b><br>
        +380 (50) 123-45-67</p>

        <p><b>Email:</b><br>
        info@grandrivoli.ua</p>

        <p><b>Соціальні мережі:</b><br>
        Instagram: @grandrivoli<br>
        Facebook: /grandrivoli</p>

        <h3 style="color:#ffe6ff;">Як до нас доїхати</h3>
        <p>Ресторан розташований на околицях міста, поруч є безкоштовна
        парковка для гостей та розкішний готель щоб відпочити.</p>
    )";

    ui->contactsTextEdit->setHtml(html);
}

void InfoForm::setupHoursTab()
{
    ui->hoursTableWidget->setColumnCount(2);
    ui->hoursTableWidget->setHorizontalHeaderLabels(QStringList() << "День" << "Години роботи");
    ui->hoursTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->hoursTableWidget->setColumnWidth(0, 150);

    struct DayHours { QString day; QString hours; };
    QList<DayHours> schedule = {
        {"Понеділок", "10:00 - 23:00"},
        {"Вівторок",  "10:00 - 23:00"},
        {"Середа",    "10:00 - 23:00"},
        {"Четвер",    "10:00 - 23:00"},
        {"П'ятниця",  "10:00 - 23:00"},
        {"Субота",    "10:00 - 22:00"},
        {"Неділя",    "11:00 - 22:00"}
    };

    ui->hoursTableWidget->setRowCount(schedule.size());
    for (int i = 0; i < schedule.size(); ++i) {
        ui->hoursTableWidget->setItem(i, 0, new QTableWidgetItem(schedule[i].day));
        ui->hoursTableWidget->setItem(i, 1, new QTableWidgetItem(schedule[i].hours));
    }

    ui->hoursNoteLabel->setText(
        "Кухня закривається за 30 хвилин до завершення робочого дня. "
        "У святкові дні графік роботи може змінюватись - актуальну "
        "інформацію уточнюйте за телефоном."
        );
}

void InfoForm::setDarkTheme()
{
    this->setStyleSheet("background-color: #1a1a1e; color: #ffffff;");
    ui->navFrame->setStyleSheet("background-color: #111114; border-bottom: 1px solid #333;");

    QString textEditStyle = "QTextEdit { background-color: #242428; color: #ffffff; border: 1px solid #3e3e42; border-radius: 6px; }";
    ui->aboutTextEdit->setStyleSheet(textEditStyle);
    ui->contactsTextEdit->setStyleSheet(textEditStyle);

    QString tableStyle =
        "QTableWidget { background-color: #242428; color: #ffffff; gridline-color: #3e3e42; border: 1px solid #3e3e42; }"
        "QHeaderView::section { background-color: #1e1e22; color: #D1B26D; padding: 5px; border: 1px solid #3e3e42; font-weight: bold; }";
    ui->hoursTableWidget->setStyleSheet(tableStyle);

    ui->hoursNoteLabel->setStyleSheet("color: #aaaaaa;");

    QString buttonStyle =
        "QPushButton { background-color: #242428; color: #D1B26D; border: 1px solid #D1B26D; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #D1B26D; color: #1a1a1e; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}

void InfoForm::setLightTheme()
{
    this->setStyleSheet("background-color: #f5f5f7; color: #1a1a1e;");
    ui->navFrame->setStyleSheet("background-color: #ffffff; border-bottom: 1px solid #dddddd;");

    QString textEditStyle = "QTextEdit { background-color: #ffffff; color: #1a1a1e; border: 1px solid #cccccc; border-radius: 6px; }";
    ui->aboutTextEdit->setStyleSheet(textEditStyle);
    ui->contactsTextEdit->setStyleSheet(textEditStyle);

    QString tableStyle =
        "QTableWidget { background-color: #ffffff; color: #1a1a1e; gridline-color: #e5e5e7; border: 1px solid #d1d1d6; }"
        "QHeaderView::section { background-color: #f5f5f7; color: #1a1a1e; padding: 5px; border: 1px solid #e5e5e7; font-weight: bold; }";
    ui->hoursTableWidget->setStyleSheet(tableStyle);

    ui->hoursNoteLabel->setStyleSheet("color: #555555;");

    QString buttonStyle =
        "QPushButton { background-color: #ffffff; color: #e0a96d; border: 1px solid #e0a96d; border-radius: 6px; padding: 5px 15px; font-weight: 500; }"
        "QPushButton:hover { background-color: #e0a96d; color: #ffffff; }";
    QList<QPushButton*> allButtons = findChildren<QPushButton*>();
    for (QPushButton* btn : allButtons) {
        btn->setStyleSheet(buttonStyle);
    }
}