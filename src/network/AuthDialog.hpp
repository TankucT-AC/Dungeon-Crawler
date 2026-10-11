// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_DIALOG_HPP
#define AUTH_DIALOG_HPP

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;

namespace auth {

class AuthClient;

/**
 * @brief Qt Widgets-окно авторизации.
 *
 * Layout-based UI без абсолютных координат: общий заголовок/подзаголовок,
 * QStackedWidget с переменным содержимым страниц, общие кнопки и статус.
 * Сетевые вызовы синхронные (AuthClient), на время запроса кнопки
 * блокируются. Закрытие окна = отказ (QDialog::Rejected), успех =
 * QDialog::Accepted.
 */
class AuthDialog : public QDialog {
  Q_OBJECT

public:
  explicit AuthDialog(AuthClient &client, QWidget *parent = nullptr);

  bool isAuthorized() const { return authorized_; }
  QString currentUser() const { return currentUser_; }

private slots:
  void onPrimary();
  void onSecondary();

private:
  enum class Page { Login, Register, Ask2FA, SetupQR, Verify2FA };

  void showPage(Page page);
  void setStatus(const QString &message, bool error);
  void setBusy(bool busy);
  static bool isValidCode(const QString &code);

  QWidget *makeLoginPage();
  QWidget *makeRegisterPage();
  QWidget *makeAsk2FAPage();
  QWidget *makeSetupQRPage();
  QWidget *makeVerify2FAPage();

  AuthClient &client_;
  bool authorized_ = false;
  QString currentUser_;
  QString generatedPassword_;
  Page currentPage_ = Page::Login;

  QLabel *titleLabel_ = nullptr;
  QLabel *subtitleLabel_ = nullptr;
  QStackedWidget *pages_ = nullptr;
  QPushButton *primaryButton_ = nullptr;
  QPushButton *secondaryButton_ = nullptr;
  QLabel *statusLabel_ = nullptr;

  QLineEdit *loginUserEdit_ = nullptr;
  QLineEdit *loginPassEdit_ = nullptr;
  QLineEdit *registerUserEdit_ = nullptr;
  QLabel *generatedPassLabel_ = nullptr;
  QLabel *qrLabel_ = nullptr;
  QLineEdit *setupCodeEdit_ = nullptr;
  QLineEdit *verifyCodeEdit_ = nullptr;
};

} // namespace auth

#endif // AUTH_DIALOG_HPP
