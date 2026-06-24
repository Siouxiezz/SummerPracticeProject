#ifndef INFOFORM_H
#define INFOFORM_H

#include <QWidget>

namespace Ui {
class InfoForm;
}

class InfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit InfoForm(QWidget *parent = nullptr);
    ~InfoForm();

    void setDarkTheme();
    void setLightTheme();

private slots:
    void on_pushButtonAbout_clicked();
    void on_pushButtonContacts_clicked();
    void on_pushButtonHours_clicked();

private:
    void setupAboutTab();
    void setupContactsTab();
    void setupHoursTab();

    Ui::InfoForm *ui;
};

#endif // INFOFORM_H