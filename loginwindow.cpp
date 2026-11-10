#include "loginwindow.h"

#include "usermanager.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(QStringLiteral("五子棋 · 登录"));
    setFixedSize(420, 480);
    setStyleSheet(QStringLiteral(
        "QWidget { background-color: #f7f3ec; font-size: 14px; }"
        "QLabel#appTitle { font-size: 34px; font-weight: bold; color: #5b3a1e; }"
        "QLabel#appSubtitle { font-size: 13px; color: #8a6d4b; }"
        "QLabel#hintLabel { font-size: 12px; }"
        "QLineEdit {"
        "  padding: 8px; border: 1px solid #c9b58a; border-radius: 6px;"
        "  background: #ffffff; }"
        "QLineEdit:focus { border: 1px solid #b34700; }"
        "QPushButton#primaryButton {"
        "  font-size: 16px; font-weight: bold; padding: 10px 0; border-radius: 8px;"
        "  border: none; background-color: #b34700; color: #ffffff; }"
        "QPushButton#primaryButton:hover { background-color: #8f3800; }"
        "QTabWidget::pane { border: 1px solid #d9cba9; border-radius: 8px; top: -1px; }"
        "QTabBar::tab { padding: 8px 26px; font-size: 14px; }"
        "QTabBar::tab:selected { color: #b34700; font-weight: bold; }"));

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(28, 20, 28, 20);
    rootLayout->setSpacing(10);

    auto *title = new QLabel(QStringLiteral("五子棋"), this);
    title->setObjectName(QStringLiteral("appTitle"));
    title->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(title);

    auto *subtitle = new QLabel(QStringLiteral("人机对战 · 五子连珠"), this);
    subtitle->setObjectName(QStringLiteral("appSubtitle"));
    subtitle->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(subtitle);

    m_tabs = new QTabWidget(this);
    rootLayout->addWidget(m_tabs, 1);

    // ---------- 登录页 ----------
    auto *loginPage = new QWidget(m_tabs);
    auto *loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setContentsMargins(20, 18, 20, 18);
    loginLayout->setSpacing(12);

    auto *loginForm = new QFormLayout();
    loginForm->setLabelAlignment(Qt::AlignRight);
    loginForm->setSpacing(10);

    m_loginUserEdit = new QLineEdit(loginPage);
    m_loginUserEdit->setPlaceholderText(QStringLiteral("请输入用户名"));
    m_loginPassEdit = new QLineEdit(loginPage);
    m_loginPassEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_loginPassEdit->setEchoMode(QLineEdit::Password);
    loginForm->addRow(QStringLiteral("用户名"), m_loginUserEdit);
    loginForm->addRow(QStringLiteral("密　码"), m_loginPassEdit);
    loginLayout->addLayout(loginForm);

    m_loginHint = new QLabel(loginPage);
    m_loginHint->setObjectName(QStringLiteral("hintLabel"));
    m_loginHint->setWordWrap(true);
    m_loginHint->setMinimumHeight(34);
    loginLayout->addWidget(m_loginHint);

    auto *loginButton = new QPushButton(QStringLiteral("登 录"), loginPage);
    loginButton->setObjectName(QStringLiteral("primaryButton"));
    loginLayout->addWidget(loginButton);

    auto *accountPathLabel = new QLabel(QStringLiteral("账号文件：%1").arg(UserManager::storageFilePath()),
                                        loginPage);
    accountPathLabel->setObjectName(QStringLiteral("hintLabel"));
    accountPathLabel->setStyleSheet(QStringLiteral("color: #a09078; font-size: 10px;"));
    accountPathLabel->setWordWrap(true);
    loginLayout->addWidget(accountPathLabel);

    loginLayout->addStretch(1);
    m_tabs->addTab(loginPage, QStringLiteral("登 录"));

    // ---------- 注册页 ----------
    auto *regPage = new QWidget(m_tabs);
    auto *regLayout = new QVBoxLayout(regPage);
    regLayout->setContentsMargins(20, 18, 20, 18);
    regLayout->setSpacing(12);

    auto *regForm = new QFormLayout();
    regForm->setLabelAlignment(Qt::AlignRight);
    regForm->setSpacing(10);

    m_regUserEdit = new QLineEdit(regPage);
    m_regUserEdit->setPlaceholderText(QStringLiteral("1-32 个字符"));
    m_regPassEdit = new QLineEdit(regPage);
    m_regPassEdit->setPlaceholderText(QStringLiteral("不少于 6 位"));
    m_regPassEdit->setEchoMode(QLineEdit::Password);
    m_regConfirmEdit = new QLineEdit(regPage);
    m_regConfirmEdit->setPlaceholderText(QStringLiteral("再次输入密码"));
    m_regConfirmEdit->setEchoMode(QLineEdit::Password);
    regForm->addRow(QStringLiteral("用户名"), m_regUserEdit);
    regForm->addRow(QStringLiteral("密　码"), m_regPassEdit);
    regForm->addRow(QStringLiteral("确认密码"), m_regConfirmEdit);
    regLayout->addLayout(regForm);

    m_regHint = new QLabel(regPage);
    m_regHint->setObjectName(QStringLiteral("hintLabel"));
    m_regHint->setWordWrap(true);
    m_regHint->setMinimumHeight(34);
    regLayout->addWidget(m_regHint);

    auto *registerButton = new QPushButton(QStringLiteral("注 册"), regPage);
    registerButton->setObjectName(QStringLiteral("primaryButton"));
    regLayout->addWidget(registerButton);

    auto *tipLabel = new QLabel(QStringLiteral("密码经加盐哈希后存储，不会明文保存。"), regPage);
    tipLabel->setObjectName(QStringLiteral("hintLabel"));
    tipLabel->setStyleSheet(QStringLiteral("color: #a09078; font-size: 10px;"));
    regLayout->addWidget(tipLabel);

    regLayout->addStretch(1);
    m_tabs->addTab(regPage, QStringLiteral("注 册"));

    // 信号连接
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_loginPassEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
    connect(m_regConfirmEdit, &QLineEdit::returnPressed, this, &LoginWindow::onRegisterClicked);
}

void LoginWindow::clearFields()
{
    m_loginPassEdit->clear();
    m_loginHint->clear();
    m_regPassEdit->clear();
    m_regConfirmEdit->clear();
    m_regHint->clear();
    m_tabs->setCurrentIndex(0);
    m_loginUserEdit->setFocus();
}

void LoginWindow::onLoginClicked()
{
    const QString username = m_loginUserEdit->text().trimmed();
    const QString password = m_loginPassEdit->text();

    QString error;
    if (!UserManager::verifyLogin(username, password, &error)) {
        m_loginHint->setStyleSheet(QStringLiteral("color: #b3261e;"));
        m_loginHint->setText(error);
        return;
    }
    m_loginHint->setStyleSheet(QStringLiteral("color: #1a7f37;"));
    m_loginHint->setText(QStringLiteral("登录成功，正在进入游戏…"));
    emit loggedIn(username);
}

void LoginWindow::onRegisterClicked()
{
    const QString username = m_regUserEdit->text().trimmed();
    const QString password = m_regPassEdit->text();
    const QString confirm = m_regConfirmEdit->text();

    if (password != confirm) {
        m_regHint->setStyleSheet(QStringLiteral("color: #b3261e;"));
        m_regHint->setText(QStringLiteral("两次输入的密码不一致。"));
        return;
    }

    QString error;
    if (!UserManager::registerUser(username, password, &error)) {
        m_regHint->setStyleSheet(QStringLiteral("color: #b3261e;"));
        m_regHint->setText(error);
        return;
    }

    m_regHint->setStyleSheet(QStringLiteral("color: #1a7f37;"));
    m_regHint->setText(QStringLiteral("注册成功！已自动切换到登录页，请直接登录。"));
    m_loginUserEdit->setText(username);
    m_loginPassEdit->clear();
    m_tabs->setCurrentIndex(0);
    m_loginPassEdit->setFocus();
}
