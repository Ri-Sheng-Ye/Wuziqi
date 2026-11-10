#include "usermanager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>

namespace {

constexpr int kHashIterations = 10000; // PBKDF 风格：盐 + 密码做 10000 次 SHA-256 迭代

// 16 字节随机盐，转十六进制字符串存储
QString generateSaltHex()
{
    QByteArray salt;
    salt.resize(16);
    for (int i = 0; i < salt.size(); ++i)
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    return QString::fromLatin1(salt.toHex());
}

// 计算哈希：SHA256 迭代 kHashIterations 次，输出十六进制字符串
QString hashPassword(const QString &saltHex, const QString &password)
{
    const QByteArray salt = QByteArray::fromHex(saltHex.toLatin1());
    QByteArray h = QCryptographicHash::hash(salt + password.toUtf8(),
                                            QCryptographicHash::Sha256);
    for (int i = 1; i < kHashIterations; ++i)
        h = QCryptographicHash::hash(h, QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

// 读取账号文件，返回 users 数组；文件不存在时返回空数组并视为首次运行
bool loadUsers(QJsonArray *users, QString *errorMessage)
{
    const QString path = UserManager::storageFilePath();
    QFile file(path);
    if (!file.exists()) {
        *users = QJsonArray();
        return true;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) *errorMessage = QStringLiteral("无法读取账号文件：") + path;
        return false;
    }
    const QByteArray data = file.readAll();
    file.close();
    if (data.trimmed().isEmpty()) {
        *users = QJsonArray();
        return true;
    }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("账号文件 users.json 已损坏，请备份后删除或修复。");
        return false;
    }
    *users = doc.object().value(QStringLiteral("users")).toArray();
    return true;
}

// 写回账号文件（缩进格式，方便查看但不含明文密码）
bool saveUsers(const QJsonArray &users, QString *errorMessage)
{
    QJsonObject root;
    root.insert(QStringLiteral("users"), users);
    const QByteArray data = QJsonDocument(root).toJson(QJsonDocument::Indented);

    QFile file(UserManager::storageFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorMessage) *errorMessage = QStringLiteral("无法写入账号文件，请检查目录权限。");
        return false;
    }
    file.write(data);
    file.close();
    return true;
}

} // namespace

QString UserManager::storageFilePath()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/users.json");
}

bool UserManager::registerUser(const QString &username, const QString &password,
                               QString *errorMessage)
{
    const QString name = username.trimmed();
    if (name.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("用户名不能为空。");
        return false;
    }
    if (name.size() > 32) {
        if (errorMessage) *errorMessage = QStringLiteral("用户名过长（不超过 32 个字符）。");
        return false;
    }
    if (password.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("密码不能为空。");
        return false;
    }
    if (password.size() < 6) {
        if (errorMessage) *errorMessage = QStringLiteral("密码长度不能少于 6 位。");
        return false;
    }

    QJsonArray users;
    if (!loadUsers(&users, errorMessage))
        return false;

    for (const QJsonValue &value : users) {
        const QJsonObject obj = value.toObject();
        if (obj.value(QStringLiteral("username")).toString() == name) {
            if (errorMessage) *errorMessage = QStringLiteral("该用户名已被注册，请更换。");
            return false;
        }
    }

    const QString salt = generateSaltHex();
    QJsonObject record;
    record.insert(QStringLiteral("username"), name);
    record.insert(QStringLiteral("salt"), salt);
    record.insert(QStringLiteral("hash"), hashPassword(salt, password));
    record.insert(QStringLiteral("createdAt"),
                  QDateTime::currentDateTime().toString(Qt::ISODate));
    users.append(record);

    return saveUsers(users, errorMessage);
}

bool UserManager::verifyLogin(const QString &username, const QString &password,
                              QString *errorMessage)
{
    const QString name = username.trimmed();
    if (name.isEmpty() || password.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("请输入用户名和密码。");
        return false;
    }

    QJsonArray users;
    if (!loadUsers(&users, errorMessage))
        return false;

    for (const QJsonValue &value : users) {
        const QJsonObject obj = value.toObject();
        if (obj.value(QStringLiteral("username")).toString() != name)
            continue;
        const QString salt = obj.value(QStringLiteral("salt")).toString();
        const QString storedHash = obj.value(QStringLiteral("hash")).toString();
        if (hashPassword(salt, password) == storedHash)
            return true;
        if (errorMessage) *errorMessage = QStringLiteral("密码错误，请重新输入。");
        return false;
    }

    if (errorMessage) *errorMessage = QStringLiteral("用户不存在，请先注册。");
    return false;
}
