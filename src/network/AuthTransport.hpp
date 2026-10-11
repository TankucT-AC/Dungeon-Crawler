// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_TRANSPORT_HPP
#define AUTH_TRANSPORT_HPP

#include "src/network/AuthConfig.hpp"
#include <QByteArray>
#include <QString>

class QNetworkAccessManager;

namespace auth {

/** @brief Сырой итог HTTP-вызова без привязки к протоколу авторизации. */
struct HttpResult {
  bool transportOk = false;
  int status = -1;
  QByteArray body;
  QString errorString;
};

/**
 * @brief Абстракция синхронного POST-транспорта.
 *
 * Позволяет подменять сеть фейком в тестах, не таская httplib-моки.
 */
class IAuthTransport {
public:
  virtual ~IAuthTransport() = default;
  virtual HttpResult post(const QString &path, const QByteArray &body) = 0;
};

/**
 * @brief Реальный транспорт на QNetworkAccessManager.
 *
 * Выполняет блокирующий POST через локальный QEventLoop с таймаутом
 * из AuthConfig. Не требует Q_OBJECT: только синхронная логика.
 * Может использовать внешний QNetworkAccessManager (не владеет им)
 * либо создать собственный.
 */
class QNetworkTransport final : public IAuthTransport {
public:
  explicit QNetworkTransport(const AuthConfig &config,
                             QNetworkAccessManager *nam = nullptr);
  ~QNetworkTransport() override;

  QNetworkTransport(const QNetworkTransport &) = delete;
  QNetworkTransport &operator=(const QNetworkTransport &) = delete;

  HttpResult post(const QString &path, const QByteArray &body) override;

private:
  AuthConfig config_;
  QNetworkAccessManager *nam_;
  bool ownsNam_;
};

} // namespace auth

#endif // AUTH_TRANSPORT_HPP
