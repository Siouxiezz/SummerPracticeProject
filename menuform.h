#ifndef MENUFORM_H
#define MENUFORM_H

#include <QWidget>
#include <QMenu>
#include "User.h"
#include "Dish.h"

namespace Ui {
class MenuForm;
}

class MenuForm : public QWidget
{
    Q_OBJECT

public:
    explicit MenuForm(const User& user, QWidget *parent = nullptr);
    ~MenuForm();

    void setDarkTheme();
    void setLightTheme();

public slots:
    void applyTheme(bool isDark);

signals:
    void statusMessage(const QString& message);

private slots:
    void on_pushButtonCart_clicked();
    void on_searchEdit_textChanged(const QString &text);
    void on_pushButtonAdd_clicked();
    void on_pushButtonEdit_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonOrder_clicked();
    void on_pushButtonExport_clicked();
    void on_pushButtonImport_clicked();
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void on_sortCategoryComboBox_currentIndexChanged(int index);

private:
    struct CartItem {
        int dishId;
        QString dishName;
        int quantity;
        double price;
    };
    QList<CartItem> m_cartItems;

    void loadDishes();
    void filterDishes(const QString& text);
    void setupTable();
    void applyFilters();
    void updateTable(const QList<Dish>& dishes);
    void populateCategoryComboBox();
    void setupContextMenu();
    int pickClientId(bool& ok);
    int assignLeastBusyWaiter();

    Ui::MenuForm *ui;
    User m_currentUser;
    QMenu *m_contextMenu;
    QList<Dish> m_currentDishes;
};

#endif // MENUFORM_H