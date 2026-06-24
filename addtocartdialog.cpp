#include "addtocartdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>

AddToCartDialog::AddToCartDialog(const QString& dishName, QWidget *parent)
    : QDialog(parent), m_quantity(1), m_isDineIn(true)
{
    setWindowTitle("Додати в кошик");
    resize(320, 220);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(QString("Страва: %1").arg(dishName), this);
    titleLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    QSpinBox *quantitySpin = new QSpinBox(this);
    quantitySpin->setRange(1, 100);
    quantitySpin->setValue(1);
    quantitySpin->setObjectName("quantitySpin");
    formLayout->addRow("Кількість порцій:", quantitySpin);
    mainLayout->addLayout(formLayout);

    QGroupBox *serveGroup = new QGroupBox("Спосіб подачі", this);
    QVBoxLayout *serveLayout = new QVBoxLayout(serveGroup);
    QRadioButton *dineInRadio = new QRadioButton("На місці", serveGroup);
    QRadioButton *takeawayRadio = new QRadioButton("На винос", serveGroup);
    dineInRadio->setChecked(true);
    dineInRadio->setObjectName("dineInRadio");
    takeawayRadio->setObjectName("takeawayRadio");
    serveLayout->addWidget(dineInRadio);
    serveLayout->addWidget(takeawayRadio);
    mainLayout->addWidget(serveGroup);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this, quantitySpin, dineInRadio]() {
        m_quantity = quantitySpin->value();
        m_isDineIn = dineInRadio->isChecked();
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

AddToCartDialog::~AddToCartDialog() {}

int AddToCartDialog::getQuantity() const
{
    return m_quantity;
}

bool AddToCartDialog::isDineIn() const
{
    return m_isDineIn;
}