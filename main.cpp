#include "mainwindow.h"
#include "loginwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("Wuziqi"));
    a.setApplicationDisplayName(QStringLiteral("五子棋"));

    LoginWindow loginWindow;
    MainWindow mainWindow;

    // 登录成功：隐藏登录窗口，显示主菜单
    QObject::connect(&loginWindow, &LoginWindow::loggedIn,
                     [&](const QString &username) {
        loginWindow.hide();
        mainWindow.setUserName(username);
        mainWindow.show();
    });

    // 退出登录：隐藏主菜单，回到登录界面
    QObject::connect(&mainWindow, &MainWindow::logoutRequested,
                     [&]() {
        mainWindow.hide();
        loginWindow.clearFields();
        loginWindow.show();
    });

    loginWindow.show();
    return QApplication::exec();
}
