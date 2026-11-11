# Wuziqi 五子棋

基于 Qt 6 / C++17 的五子棋桌面程序，支持账号注册登录、双人对战与人机对战。

![界面截图](docs/screenshot.png)

> 上图放在 `docs/screenshot.png`，程序跑起来后截一张图放进去即可。没有图就先删掉这一行。

## 功能

- **账号系统**：注册与登录，账号数据存本地 `users.json`，密码加盐哈希存储，不落明文
- **对局**：15×15 棋盘，黑白轮流落子，五子连珠判胜
- **人机对战**：两档难度
  - `Easy`：偏随机落子
  - `Normal`：会拦截玩家的四连，同时主动冲自己的四连
- **AI 策略**：只在已有棋子附近（距离 ≤ 2）搜索候选点，空盘下天元，逐点做攻防打分

## 技术栈

| 项目 | 说明 |
| 语言 | C++17 |
| 框架 | Qt 6.5+（Core / Gui / Widgets） |
| 构建 | CMake ≥ 3.19 |
| IDE | Qt Creator（直接打开 `CMakeLists.txt`） |

## 目录结构

```
Wuziqi/
├─ CMakeLists.txt        构建脚本
├─ main.cpp              程序入口
├─ mainwindow.*          主窗口
├─ loginwindow.*         登录 / 注册界面
├─ gamewindow.*          对局窗口（计时、落子流程）
├─ boardwidget.*         棋盘绘制与鼠标落子
├─ gomokuai.*            人机对战 AI
├─ usermanager.*         账号读写与密码哈希
└─ uitheme.h             界面配色与样式常量
```

## 编译运行

前置：已安装 Qt 6.5+ 与 CMake 3.19+，且 Qt 的 `bin` 目录在 PATH 中。

```bash
cmake -S . -B build
cmake --build build --config Release
```

或在 Qt Creator 中「打开项目」→ 选择 `CMakeLists.txt` → 选好 Kit → 运行。

首次启动需先注册一个账号，登录后进入对局界面。

## 说明

- `users.json` 在程序运行目录生成，已被 `.gitignore` 排除，不随仓库分发
- 换机器 clone 后没有历史账号，属预期行为，重新注册即可

## 待办

- [ ] 增加悔棋与复盘
- [ ] AI 加入极小化极大搜索与棋型评分表
- [ ] 支持先后手选择与对局存档





*协作练习：来自fork的修改*
