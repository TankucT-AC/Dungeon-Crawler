// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/network/AuthDialog.hpp"

#include "src/network/AuthClient.hpp"
#include "src/network/AuthTheme.hpp"

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace auth {

AuthDialog::AuthDialog(AuthClient &client, QWidget *parent)
    : QDialog(parent), client_(client) {
  setWindowTitle(tr("Escape From Epstein Island"));
  setModal(true);
  theme::applyTo(this);

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(theme::kLayoutMargin, theme::kLayoutMargin,
                           theme::kLayoutMargin, theme::kLayoutMargin);
  root->setSpacing(theme::kLayoutSpacing);

  titleLabel_ = new QLabel(this);
  titleLabel_->setProperty("role", "title");
  titleLabel_->setAlignment(Qt::AlignCenter);
  root->addWidget(titleLabel_);

  subtitleLabel_ = new QLabel(this);
  subtitleLabel_->setProperty("role", "subtitle");
  subtitleLabel_->setAlignment(Qt::AlignCenter);
  subtitleLabel_->setWordWrap(true);
  root->addWidget(subtitleLabel_);

  pages_ = new QStackedWidget(this);
  pages_->addWidget(makeLoginPage());     // Page::Login
  pages_->addWidget(makeRegisterPage());  // Page::Register
  pages_->addWidget(makeAsk2FAPage());    // Page::Ask2FA
  pages_->addWidget(makeSetupQRPage());   // Page::SetupQR
  pages_->addWidget(makeVerify2FAPage()); // Page::Verify2FA
  root->addWidget(pages_);

  primaryButton_ = new QPushButton(this);
  connect(primaryButton_, &QPushButton::clicked, this, &AuthDialog::onPrimary);
  root->addWidget(primaryButton_);

  secondaryButton_ = new QPushButton(this);
  secondaryButton_->setProperty("role", "secondary");
  connect(secondaryButton_, &QPushButton::clicked, this,
          &AuthDialog::onSecondary);
  root->addWidget(secondaryButton_);

  statusLabel_ = new QLabel(this);
  statusLabel_->setProperty("role", "status");
  statusLabel_->setAlignment(Qt::AlignCenter);
  statusLabel_->setWordWrap(true);
  root->addWidget(statusLabel_);

  showPage(Page::Login);
}

QWidget *AuthDialog::makeLoginPage() {
  auto *page = new QWidget(this);
  auto *layout = new QVBoxLayout(page);
  layout->setSpacing(theme::kLayoutSpacing);

  loginUserEdit_ = new QLineEdit(page);
  loginUserEdit_->setPlaceholderText(tr("USERNAME"));
  loginUserEdit_->setClearButtonEnabled(true);
  layout->addWidget(loginUserEdit_);

  loginPassEdit_ = new QLineEdit(page);
  loginPassEdit_->setPlaceholderText(tr("PASSWORD"));
  loginPassEdit_->setEchoMode(QLineEdit::Password);
  layout->addWidget(loginPassEdit_);

  return page;
}

QWidget *AuthDialog::makeRegisterPage() {
  auto *page = new QWidget(this);
  auto *layout = new QVBoxLayout(page);

  registerUserEdit_ = new QLineEdit(page);
  registerUserEdit_->setPlaceholderText(tr("USERNAME"));
  registerUserEdit_->setClearButtonEnabled(true);
  layout->addWidget(registerUserEdit_);

  return page;
}

QWidget *AuthDialog::makeAsk2FAPage() {
  auto *page = new QWidget(this);
  auto *layout = new QVBoxLayout(page);

  generatedPassLabel_ = new QLabel(page);
  generatedPassLabel_->setProperty("role", "generated");
  generatedPassLabel_->setAlignment(Qt::AlignCenter);
  generatedPassLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  layout->addWidget(generatedPassLabel_);

  return page;
}

QWidget *AuthDialog::makeSetupQRPage() {
  auto *page = new QWidget(this);
  auto *layout = new QVBoxLayout(page);
  layout->setSpacing(theme::kLayoutSpacing);

  qrLabel_ = new QLabel(page);
  qrLabel_->setAlignment(Qt::AlignCenter);
  qrLabel_->setMinimumSize(theme::kQrSize, theme::kQrSize);
  layout->addWidget(qrLabel_);

  setupCodeEdit_ = new QLineEdit(page);
  setupCodeEdit_->setPlaceholderText(tr("6-DIGIT CODE"));
  setupCodeEdit_->setMaxLength(theme::kCodeLength);
  setupCodeEdit_->setValidator(new QRegularExpressionValidator(
      QRegularExpression(QStringLiteral("[0-9]{0,6}")), setupCodeEdit_));
  setupCodeEdit_->setAlignment(Qt::AlignCenter);
  layout->addWidget(setupCodeEdit_);

  return page;
}

QWidget *AuthDialog::makeVerify2FAPage() {
  auto *page = new QWidget(this);
  auto *layout = new QVBoxLayout(page);

  verifyCodeEdit_ = new QLineEdit(page);
  verifyCodeEdit_->setPlaceholderText(tr("6-DIGIT CODE"));
  verifyCodeEdit_->setMaxLength(theme::kCodeLength);
  verifyCodeEdit_->setValidator(new QRegularExpressionValidator(
      QRegularExpression(QStringLiteral("[0-9]{0,6}")), verifyCodeEdit_));
  verifyCodeEdit_->setAlignment(Qt::AlignCenter);
  layout->addWidget(verifyCodeEdit_);

  return page;
}

void AuthDialog::showPage(Page page) {
  currentPage_ = page;
  pages_->setCurrentIndex(static_cast<int>(page));
  secondaryButton_->setVisible(true);

  switch (page) {
  case Page::Login:
    titleLabel_->setText(tr("Welcome back!"));
    subtitleLabel_->setText(tr("We're so excited to see you again!"));
    primaryButton_->setText(tr("LOGIN"));
    secondaryButton_->setText(tr("NEED AN ACCOUNT?"));
    break;
  case Page::Register:
    titleLabel_->setText(tr("Create an account"));
    subtitleLabel_->setText(
        tr("Enter a username. We will generate a secure password for you "
           "automatically."));
    primaryButton_->setText(tr("REGISTER"));
    secondaryButton_->setText(tr("BACK TO LOGIN"));
    break;
  case Page::Ask2FA:
    titleLabel_->setText(tr("Account Created!"));
    subtitleLabel_->setText(
        tr("SAVE THIS PASSWORD! You will not see it again.\n\nWould you like "
           "to enable 2FA protection?"));
    generatedPassLabel_->setText(generatedPassword_);
    primaryButton_->setText(tr("YES, ENABLE 2FA"));
    secondaryButton_->setText(tr("SKIP & PLAY"));
    break;
  case Page::SetupQR:
    titleLabel_->setText(tr("Scan QR Code"));
    subtitleLabel_->setText(
        tr("Open your Authenticator app and scan the code below to finish "
           "setup."));
    primaryButton_->setText(tr("FINISH SETUP"));
    secondaryButton_->setVisible(false);
    setupCodeEdit_->clear();
    break;
  case Page::Verify2FA:
    titleLabel_->setText(tr("Enter Login Code"));
    subtitleLabel_->setText(tr("Enter the 6-digit code from your app."));
    primaryButton_->setText(tr("VERIFY"));
    secondaryButton_->setText(tr("BACK TO LOGIN"));
    verifyCodeEdit_->clear();
    break;
  }
}

void AuthDialog::setStatus(const QString &message, bool error) {
  statusLabel_->setText(message);
  statusLabel_->setStyleSheet(theme::statusStyle(error));
}

void AuthDialog::setBusy(bool busy) {
  primaryButton_->setEnabled(!busy);
  secondaryButton_->setEnabled(!busy);
  if (busy)
    QApplication::setOverrideCursor(Qt::WaitCursor);
  else
    QApplication::restoreOverrideCursor();
}

bool AuthDialog::isValidCode(const QString &code) {
  if (code.size() != theme::kCodeLength)
    return false;
  for (const QChar c : code) {
    if (!c.isDigit())
      return false;
  }
  return true;
}

void AuthDialog::onPrimary() {
  switch (currentPage_) {
  case Page::Login: {
    const QString user = loginUserEdit_->text().trimmed();
    if (user.isEmpty() || loginPassEdit_->text().isEmpty()) {
      setStatus(tr("Enter username and password."), true);
      return;
    }
    setBusy(true);
    const AuthResult res = client_.login(user, loginPassEdit_->text());
    setBusy(false);
    if (res.success) {
      currentUser_ = user;
      if (res.requires2FA) {
        showPage(Page::Verify2FA);
        setStatus(tr("2FA Token required."), false);
      } else {
        authorized_ = true;
        accept();
      }
    } else {
      setStatus(tr("Invalid credentials!"), true);
    }
    break;
  }
  case Page::Register: {
    const QString user = registerUserEdit_->text().trimmed();
    if (user.isEmpty()) {
      setStatus(tr("Enter a username."), true);
      return;
    }
    setBusy(true);
    const AuthResult res = client_.registerUser(user);
    setBusy(false);
    if (res.success) {
      currentUser_ = user;
      generatedPassword_ = res.password;
      showPage(Page::Ask2FA);
      setStatus(tr("Success!"), false);
    } else {
      setStatus(res.message.isEmpty() ? tr("Registration failed.")
                                      : res.message,
                true);
    }
    break;
  }
  case Page::Ask2FA: {
    setBusy(true);
    const AuthResult res = client_.setup2FA(currentUser_);
    setBusy(false);
    if (res.success && !res.qrPng.isEmpty()) {
      QPixmap qr;
      if (!qr.loadFromData(res.qrPng)) {
        setStatus(tr("Failed to display QR code."), true);
        return;
      }
      qrLabel_->setPixmap(qr.scaled(theme::kQrSize, theme::kQrSize,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation));
      showPage(Page::SetupQR);
    } else {
      setStatus(tr("Failed to generate QR."), true);
    }
    break;
  }
  case Page::SetupQR:
  case Page::Verify2FA: {
    const QString code = (currentPage_ == Page::SetupQR)
                             ? setupCodeEdit_->text().trimmed()
                             : verifyCodeEdit_->text().trimmed();
    if (!isValidCode(code)) {
      setStatus(tr("Enter a valid 6-digit code."), true);
      return;
    }
    setBusy(true);
    const AuthResult res = client_.verify2FA(currentUser_, code);
    setBusy(false);
    if (res.success) {
      authorized_ = true;
      accept();
    } else {
      setStatus(tr("Invalid 2FA code!"), true);
    }
    break;
  }
  }
}

void AuthDialog::onSecondary() {
  switch (currentPage_) {
  case Page::Login:
    showPage(Page::Register);
    break;
  case Page::Register:
  case Page::Verify2FA:
    showPage(Page::Login);
    break;
  case Page::Ask2FA:
    authorized_ = true;
    accept();
    break;
  case Page::SetupQR:
    break;
  }
}

} // namespace auth
