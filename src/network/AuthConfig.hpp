// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_CONFIG_HPP
#define AUTH_CONFIG_HPP

#include <QString>
#include <QUrl>
#include <QtGlobal>

namespace auth {

/**
 * @brief Единая точка конфигурации сетевого слоя авторизации.
 *
 * Заменяет разбросанные по коду захардкоженные значения
 * (хост/порт/пути/таймауты в config::SCHEME_HOST_PORT и AuthClient).
 * Значения по умолчанию сохраняют прежнее поведение (сервер из
 * server/controllers/AuthController.cc), переопределение — через
 * переменные окружения, см. AuthConfig::fromEnvironment().
 */
struct AuthConfig {
  QString scheme = QStringLiteral("https");
  QString host = QStringLiteral("127.0.0.1");
  quint16 port = 8080;

  QString loginPath = QStringLiteral("/api/login");
  QString registerPath = QStringLiteral("/api/register");
  QString setup2FaPath = QStringLiteral("/api/setup_2fa");
  QString verify2FaPath = QStringLiteral("/api/verify_2fa");

  int requestTimeoutMs = 5000;
  bool verifySsl = false;

  QUrl baseUrl() const {
    QUrl url;
    url.setScheme(scheme);
    url.setHost(host);
    url.setPort(port);
    return url;
  }

  QUrl urlFor(const QString &path) const {
    QUrl url = baseUrl();
    url.setPath(path);
    return url;
  }

  /**
   * @brief Возвращает конфиг с переопределениями из окружения.
   *
   * Поддерживаемые переменные (пустые/невалидные игнорируются):
   *   2DGAME_AUTH_SCHEME, 2DGAME_AUTH_HOST, 2DGAME_AUTH_PORT,
   *   2DGAME_AUTH_LOGIN_PATH, 2DGAME_AUTH_REGISTER_PATH,
   *   2DGAME_AUTH_SETUP_2FA_PATH, 2DGAME_AUTH_VERIFY_2FA_PATH,
   *   2DGAME_AUTH_TIMEOUT_MS, 2DGAME_AUTH_VERIFY_SSL (1/true/yes/on).
   */
  static AuthConfig fromEnvironment(const AuthConfig &base);
};

inline AuthConfig AuthConfig::fromEnvironment(const AuthConfig &base) {
  AuthConfig cfg = base;

  const auto strVar = [](const char *name, QString &out) {
    const QByteArray v = qgetenv(name);
    if (!v.isEmpty())
      out = QString::fromUtf8(v);
  };
  strVar("2DGAME_AUTH_SCHEME", cfg.scheme);
  strVar("2DGAME_AUTH_HOST", cfg.host);
  strVar("2DGAME_AUTH_LOGIN_PATH", cfg.loginPath);
  strVar("2DGAME_AUTH_REGISTER_PATH", cfg.registerPath);
  strVar("2DGAME_AUTH_SETUP_2FA_PATH", cfg.setup2FaPath);
  strVar("2DGAME_AUTH_VERIFY_2FA_PATH", cfg.verify2FaPath);

  bool ok = false;
  const int port = qgetenv("2DGAME_AUTH_PORT").toInt(&ok);
  if (ok && port > 0 && port <= 65535)
    cfg.port = static_cast<quint16>(port);

  const int timeout = qgetenv("2DGAME_AUTH_TIMEOUT_MS").toInt(&ok);
  if (ok && timeout > 0)
    cfg.requestTimeoutMs = timeout;

  const QString ssl =
      QString::fromUtf8(qgetenv("2DGAME_AUTH_VERIFY_SSL")).toLower();
  if (ssl == QStringLiteral("1") || ssl == QStringLiteral("true") ||
      ssl == QStringLiteral("yes") || ssl == QStringLiteral("on"))
    cfg.verifySsl = true;
  else if (ssl == QStringLiteral("0") || ssl == QStringLiteral("false") ||
           ssl == QStringLiteral("no") || ssl == QStringLiteral("off"))
    cfg.verifySsl = false;

  return cfg;
}

} // namespace auth

#endif // AUTH_CONFIG_HPP
