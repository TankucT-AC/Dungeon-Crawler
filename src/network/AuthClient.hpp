// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_CLIENT_HPP
#define AUTH_CLIENT_HPP

#include "src/network/AuthConfig.hpp"
#include "src/network/AuthResult.hpp"
#include "src/network/AuthTransport.hpp"

#include <QJsonObject>
#include <QString>
#include <memory>

namespace auth {

/**
 * @brief Синхронный клиент протокола авторизации.
 *
 * Тонкий слой поверх IAuthTransport: собирает JSON, разбирает ответ
 * сервера (см. server/controllers/AuthController.cc) в AuthResult.
 * Вся конфигурация (пути, таймауты) — из AuthConfig, ключи JSON —
 * в одном месте (AuthClient.cpp, namespace protocol).
 * Транспорт подменяется фейком в тестах.
 */
class AuthClient {
public:
  explicit AuthClient(const AuthConfig &config = AuthConfig{},
                      std::unique_ptr<IAuthTransport> transport = nullptr);

  AuthResult login(const QString &username, const QString &password);
  AuthResult registerUser(const QString &username);
  AuthResult setup2FA(const QString &username);
  AuthResult verify2FA(const QString &username, const QString &code);

  const AuthConfig &config() const { return config_; }

private:
  AuthResult request(const QString &path, const QJsonObject &body);

  AuthConfig config_;
  std::unique_ptr<IAuthTransport> transport_;
};

} // namespace auth

#endif // AUTH_CLIENT_HPP
