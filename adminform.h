#ifndef ADMINFORM_H
#define ADMINFORM_H

#include <QWidget>
#include <QMenu>
#include "User.h"
#include "Table.h"

namespace Ui {
class AdminForm;
}

class AdminForm : public QWidget
{
    Q_OBJECT

public:
    explicit AdminForm(const User& currentUser, QWidget *parent = nullptr);
    ~AdminForm();

    void setDarkTheme();
    void setLightTheme();

signals:
    void statusMessage(const QString& message);

private slots:
    void on_pushButtonUsersTab_clicked();
    void on_pushButtonTablesTab_clicked();

    // Користувачі
    void on_usersSearchEdit_textChanged(const QString &text);
    void on_usersRoleFilter_currentTextChanged(const QString &text);
    void on_usersEditPushButton_clicked();
    void on_usersDeletePushButton_clicked();
    void on_usersRefreshPushButton_clicked();
    void on_usersTableWidget_customContextMenuRequested(const QPoint &pos);
    void on_usersTableWidget_doubleClicked(const QModelIndex &index);

    // Столики
    void on_tablesAddPushButton_clicked();
    void on_tablesEditPushButton_clicked();
    void on_tablesDeletePushButton_clicked();
    void on_tablesRefreshPushButton_clicked();
    void on_tablesTableWidget_customContextMenuRequested(const QPoint &pos);
    void on_tablesTableWidget_doubleClicked(const QModelIndex &index);

private:
    void setupUsersTab();
    void setupUsersContextMenu();
    void loadUsers();
    void updateUsersTable(const QList<User>& users);
    void filterUsers();

    void setupTablesTab();
    void setupTablesContextMenu();
    void loadTables();
    void updateTablesTable(const QList<Table>& tables);
    bool isTableDeletable(int tableId, QString& reasonIfNot);

    Ui::AdminForm *ui;
    User m_currentUser;
    QList<User> m_currentUsers;
    QList<Table> m_currentTables;
    QMenu *m_usersContextMenu;
    QMenu *m_tablesContextMenu;
};

#endif // ADMINFORM_H