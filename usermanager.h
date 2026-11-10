#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>

// 账号管理：本地 JSON 文件存储，密码加盐哈希（禁止明文）
class UserManager
{
public:
    // 账号文件路径（users.json，位于程序运行目录/可执行文件同目录）
    static QString storageFilePath();

    // 注册新用户。成功返回 true；失败返回 false，并通过 errorMessage 输出中文原因。
    static bool registerUser(const QString &username, const QString &password,
                             QString *errorMessage = nullptr);

    // 校验登录。成功返回 true；失败返回 false，并通过 errorMessage 输出中文原因。
    static bool verifyLogin(const QString &username, const QString &password,
                            QString *errorMessage = nullptr);

private:
    UserManager() = delete;
};

#endif // USERMANAGER_H
