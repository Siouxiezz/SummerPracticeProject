#ifndef ADDTOCARTDIALOG_H
#define ADDTOCARTDIALOG_H
#include <QDialog>
#include <QString>

class AddToCartDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddToCartDialog(const QString& dishName, QWidget *parent = nullptr);
    ~AddToCartDialog();

    int getQuantity() const;
    bool isDineIn() const;

private:
    int m_quantity;
    bool m_isDineIn;
};
#endif // ADDTOCARTDIALOG_H