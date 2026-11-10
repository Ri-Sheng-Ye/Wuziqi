#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QTabWidget;

// 登录 + 注册界面（同一个窗口内用标签页切换）
class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

    // 清空输入框（退出登录后重新显示时调用）
    void clearFields();

signals:
    // 登录成功，参数为用户名
    void loggedIn(const QString &username);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QTabWidget *m_tabs = nullptr;

    // 登录页
    QLineEdit *m_loginUserEdit = nullptr;
    QLineEdit *m_loginPassEdit = nullptr;
    QLabel *m_loginHint = nullptr;

    // 注册页
    QLineEdit *m_regUserEdit = nullptr;
    QLineEdit *m_regPassEdit = nullptr;
    QLineEdit *m_regConfirmEdit = nullptr;
    QLabel *m_regHint = nullptr;
};

#endif // LOGINWINDOW_H
