#include "mainwindow.h"

#include "gamewindow.h"

#include <QCloseEvent>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("五子棋 · 主菜单"));
    setFixedSize(460, 560);
    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: #f7f3ec; }"
        "QWidget#central { background-color: #f7f3ec; }"
        "QLabel#appTitle { font-size: 44px; font-weight: bold; color: #5b3a1e; }"
        "QLabel#appSubtitle { font-size: 14px; color: #8a6d4b; }"
        "QLabel#welcomeLabel { font-size: 16px; color: #3f2c17; }"
        "QGroupBox {"
        "  font-size: 15px; font-weight: bold; color: #5b3a1e;"
        "  border: 1px solid #d9cba9; border-radius: 10px;"
        "  margin-top: 14px; padding: 14px 12px 12px 12px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }"
        "QRadioButton { font-size: 14px; color: #3f2c17; padding: 4px 2px; }"
        "QRadioButton::indicator { width: 18px; height: 18px; }"
        "QPushButton#startButton {"
        "  font-size: 19px; font-weight: bold; color: #ffffff; padding: 14px 0;"
        "  border: none; border-radius: 10px; background-color: #b34700; }"
        "QPushButton#startButton:hover { background-color: #8f3800; }"
        "QPushButton#logoutButton {"
        "  font-size: 13px; color: #6b5233; padding: 8px 0;"
        "  border: 1px solid #c9b58a; border-radius: 8px; background: transparent; }"
        "QPushButton#logoutButton:hover { background-color: #eee5d0; }"));

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("central"));
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(36, 24, 36, 20);
    layout->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("五子棋"), central);
    title->setObjectName(QStringLiteral("appTitle"));
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto *subtitle = new QLabel(QStringLiteral("—— 人机对战 · 五子连珠 ——"), central);
    subtitle->setObjectName(QStringLiteral("appSubtitle"));
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    m_welcomeLabel = new QLabel(central);
    m_welcomeLabel->setObjectName(QStringLiteral("welcomeLabel"));
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_welcomeLabel);

    // 难度选择卡片
    auto *difficultyBox = new QGroupBox(QStringLiteral("选择难度"), central);
    auto *difficultyLayout = new QVBoxLayout(difficultyBox);
    difficultyLayout->setSpacing(8);

    m_easyRadio = new QRadioButton(QStringLiteral("简单难度（偏随机，适合练手）"), difficultyBox);
    m_normalRadio = new QRadioButton(QStringLiteral("正常难度（会拦玩家的四连，也会自己冲四连）"),
                                     difficultyBox);
    m_normalRadio->setChecked(true);

    auto *group = new QButtonGroup(this);
    group->addButton(m_easyRadio);
    group->addButton(m_normalRadio);

    difficultyLayout->addWidget(m_easyRadio);
    difficultyLayout->addWidget(m_normalRadio);
    layout->addWidget(difficultyBox);

    layout->addStretch(1);

    auto *startButton = new QPushButton(QStringLiteral("开始人机对战"), central);
    startButton->setObjectName(QStringLiteral("startButton"));
    layout->addWidget(startButton);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);

    auto *logoutButton = new QPushButton(QStringLiteral("退出登录"), central);
    logoutButton->setObjectName(QStringLiteral("logoutButton"));
    layout->addWidget(logoutButton);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::setUserName(const QString &username)
{
    m_username = username;
    m_welcomeLabel->setText(QStringLiteral("欢迎你，%1！你执黑棋，先行。").arg(username));
}

Difficulty MainWindow::selectedDifficulty() const
{
    return m_easyRadio->isChecked() ? Difficulty::Easy : Difficulty::Normal;
}

void MainWindow::onStartGameClicked()
{
    // 每次新建对局窗口（销毁后自动释放）；本窗口隐藏，对局结束后回到这里
    auto *game = new GameWindow(m_username, selectedDifficulty());
    connect(game, &GameWindow::backToMenu, this, [this, game]() {
        game->close(); // WA_DeleteOnClose 会自动释放
        this->show();
    });
    hide();
    game->show();
}

void MainWindow::onLogoutClicked()
{
    emit logoutRequested();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭主菜单视为退出登录，回到登录界面
    event->ignore();
    emit logoutRequested();
}
