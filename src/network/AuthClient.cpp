// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/network/AuthClient.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace auth {

/**
 * Ключи и значения JSON-протокола.
*/
namespace protocol {
inline QString status() { return QStringLiteral("status"); }
inline QString successValue() { return QStringLiteral("success"); }
inline QString login() { return QStringLiteral("login"); }
inline QString password() { return QStringLiteral("password"); }
inline QString totpKey() { return QStringLiteral("totp_key"); }
inline QString totpVerify() { return QStringLiteral("is_totp_verify"); }
inline QString qrBase64() { return QStringLiteral("qr_code_base64"); }
inline QString totpSecret() { return QStringLiteral("totp_secret"); }
} // namespace protocol

AuthClient::AuthClient(const AuthConfig &config,
                       std::unique_ptr<IAuthTransport> transport)
    : config_(config), transport_(std::move(transport)) {
  if (!transport_)
    transport_ = std::make_unique<QNetworkTransport>(config_);
}

AuthResult AuthClient::login(const QString &username, const QString &password) {
  QJsonObject body;
  body.insert(protocol::login(), username);
  body.insert(protocol::password(), password);
  return request(config_.loginPath, body);
}

AuthResult AuthClient::registerUser(const QString &username) {
  QJsonObject body;
  body.insert(protocol::login(), username);
  return request(config_.registerPath, body);
}

AuthResult AuthClient::setup2FA(const QString &username) {
  QJsonObject body;
  body.insert(protocol::login(), username);
  return request(config_.setup2FaPath, body);
}

AuthResult AuthClient::verify2FA(const QString &username, const QString &code) {
  QJsonObject body;
  body.insert(protocol::login(), username);
  body.insert(protocol::totpKey(), code);
  return request(config_.verify2FaPath, body);
}

AuthResult AuthClient::request(const QString &path, const QJsonObject &body) {
  AuthResult out;

  const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
  const HttpResult http = transport_->post(path, payload);

  if (!http.transportOk) {
    out.message = QStringLiteral("NET_ERR: ") + http.errorString;
    return out;
  }

  if (http.status != 200) {
    out.message =
        QStringLiteral("SERVER_ERROR_") + QString::number(http.status);
    return out;
  }

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(http.body, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    out.message = QStringLiteral("JSON_ERROR");
    return out;
  }

  const QJsonObject data = doc.object();
  const QString status = data.value(protocol::status()).toString();
  out.success = (status == protocol::successValue());
  out.message = status;
  out.requires2FA = data.value(protocol::totpVerify()).toBool(false);
  out.password = data.value(protocol::password()).toString();

  if (path == config_.setup2FaPath) {
    const QString encoded = data.value(protocol::qrBase64()).toString();
    out.qrPng = QByteArray::fromBase64(encoded.toUtf8());
    out.totpSecret = data.value(protocol::totpSecret()).toString();
  }

  return out;
}

} // namespace auth
