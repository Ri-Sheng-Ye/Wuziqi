#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gomokuai.h"

#include <QMainWindow>

class QLabel;
class QRadioButton;

// 主界面：显示当前用户、选择难度、开始人机对战、退出登录
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // 设置当前登录用户名（登录成功后由 main 调用）
    void setUserName(const QString &username);

signals:
    // 用户选择退出登录（回到登录界面）
    void logoutRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onStartGameClicked();
    void onLogoutClicked();

private:
    Difficulty selectedDifficulty() const;

    QString m_username;
    QLabel *m_welcomeLabel = nullptr;
    QRadioButton *m_easyRadio = nullptr;
    QRadioButton *m_normalRadio = nullptr;
};

#endif // MAINWINDOW_H
