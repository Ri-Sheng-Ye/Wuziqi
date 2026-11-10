#include "mainwindow.h"

#include "gamewindow.h"
#include "uitheme.h"

#include <QCloseEvent>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("五子棋 · 主菜单"));
    setFixedSize(480, 620);
    setStyleSheet(wuziqiStyleSheet());

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(40, 22, 40, 22);
    layout->setSpacing(6);

    // ---------- 顶部徽标 + 标题 ----------
    auto *badge = new QLabel(QStringLiteral("五"), central);
    badge->setObjectName(QStringLiteral("stoneBadge"));
    badge->setFixedSize(66, 66);
    badge->setAlignment(Qt::AlignCenter);
    layout->addWidget(badge, 0, Qt::AlignHCenter);

    auto *title = new QLabel(QStringLiteral("五 子 棋"), central);
    title->setObjectName(QStringLiteral("appTitle"));
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto *subtitle = new QLabel(QStringLiteral("人 机 对 战 · 五 子 连 珠"), central);
    subtitle->setObjectName(QStringLiteral("appSubtitle"));
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);
    layout->addSpacing(2);

    // ---------- 欢迎语 ----------
    m_welcomeLabel = new QLabel(central);
    m_welcomeLabel->setObjectName(QStringLiteral("welcomePill"));
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_welcomeLabel, 0, Qt::AlignHCenter);
    layout->addSpacing(8);

    // ---------- 难度选择卡片 ----------
    auto *difficultyBox = new QWidget(central);
    difficultyBox->setObjectName(QStringLiteral("card"));
    auto *difficultyLayout = new QVBoxLayout(difficultyBox);
    difficultyLayout->setContentsMargins(20, 16, 20, 18);
    difficultyLayout->setSpacing(2);

    auto *difficultyTitle = new QLabel(QStringLiteral("选 择 难 度"), difficultyBox);
    difficultyTitle->setStyleSheet(QStringLiteral(
        "font-size: 14px; font-weight: bold; color: #5b3a1e;"));
    difficultyLayout->addWidget(difficultyTitle);
    difficultyLayout->addSpacing(6);

    // 简单难度
    m_easyRadio = new QRadioButton(QStringLiteral("简 单"), difficultyBox);
    m_easyRadio->setStyleSheet(QStringLiteral("font-size: 15px; font-weight: bold; color: #41301f;"));
    difficultyLayout->addWidget(m_easyRadio);
    auto *easyDesc = new QLabel(QStringLiteral("偏随机的落子，适合热身练手"), difficultyBox);
    easyDesc->setObjectName(QStringLiteral("mutedLabel"));
    easyDesc->setContentsMargins(26, 0, 0, 6);
    difficultyLayout->addWidget(easyDesc);

    // 正常难度
    m_normalRadio = new QRadioButton(QStringLiteral("正 常"), difficultyBox);
    m_normalRadio->setStyleSheet(QStringLiteral("font-size: 15px; font-weight: bold; color: #41301f;"));
    m_normalRadio->setChecked(true);
    difficultyLayout->addWidget(m_normalRadio);
    auto *normalDesc = new QLabel(QStringLiteral("会拦你的四连，也会自己冲四连"), difficultyBox);
    normalDesc->setObjectName(QStringLiteral("mutedLabel"));
    normalDesc->setContentsMargins(26, 0, 0, 0);
    difficultyLayout->addWidget(normalDesc);

    auto *group = new QButtonGroup(this);
    group->addButton(m_easyRadio);
    group->addButton(m_normalRadio);

    layout->addWidget(difficultyBox);
    layout->addStretch(1);

    // ---------- 操作按钮 ----------
    auto *startButton = new QPushButton(QStringLiteral("开始人机对战"), central);
    startButton->setObjectName(QStringLiteral("primaryButton"));
    startButton->setMinimumHeight(52);
    layout->addWidget(startButton);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);

    auto *logoutButton = new QPushButton(QStringLiteral("退 出 登 录"), central);
    logoutButton->setObjectName(QStringLiteral("ghostButton"));
    layout->addWidget(logoutButton, 0, Qt::AlignHCenter);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::setUserName(const QString &username)
{
    m_username = username;
    m_welcomeLabel->setText(QStringLiteral("你好，%1 · 你执黑棋先行").arg(username));
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
