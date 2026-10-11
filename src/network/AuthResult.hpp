// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_RESULT_HPP
#define AUTH_RESULT_HPP

#include <QByteArray>
#include <QString>

namespace auth {

/**
 * @brief Результат одного вызова AuthClient.
 *
 * Единый тип вместо ad-hoc полей старого Response:
 * success/сообщение всегда заполнены, остальные поля — только для
 * соответствующих операций (password — register, qrPng/totpSecret — setup2FA).
 */
struct AuthResult {
  bool success = false;
  bool requires2FA = false;
  QString message;
  QString password;
  QString totpSecret;
  QByteArray qrPng;
};

} // namespace auth

#endif // AUTH_RESULT_HPP
