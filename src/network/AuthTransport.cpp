// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/network/AuthTransport.hpp"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QTimer>

namespace auth {

namespace {
constexpr const char *kJsonContentType = "application/json";
}

QNetworkTransport::QNetworkTransport(const AuthConfig &config,
                                     QNetworkAccessManager *nam)
    : config_(config), nam_(nam), ownsNam_(nam == nullptr) {
  if (nam_ == nullptr)
    nam_ = new QNetworkAccessManager();
}

QNetworkTransport::~QNetworkTransport() {
  if (ownsNam_)
    delete nam_;
}

HttpResult QNetworkTransport::post(const QString &path,
                                   const QByteArray &body) {
  HttpResult out;

  QNetworkRequest request(config_.urlFor(path));
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    QString::fromLatin1(kJsonContentType));

  if (!config_.verifySsl) {
    QSslConfiguration ssl = request.sslConfiguration();
    ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(ssl);
  }

  QNetworkReply *reply = nam_->post(request, body);

  QEventLoop loop;
  QTimer timer;
  timer.setSingleShot(true);
  QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  timer.start(config_.requestTimeoutMs);
  loop.exec();

  if (!timer.isActive()) {
    reply->abort();
    out.errorString = QStringLiteral("Request timed out");
    reply->deleteLater();
    return out;
  }
  timer.stop();

  if (reply->error() != QNetworkReply::NoError) {
    out.errorString = reply->errorString();
    reply->deleteLater();
    return out;
  }

  out.transportOk = true;
  out.status =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  out.body = reply->readAll();
  reply->deleteLater();
  return out;
}

} // namespace auth
