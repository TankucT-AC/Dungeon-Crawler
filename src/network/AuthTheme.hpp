// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_THEME_HPP
#define AUTH_THEME_HPP

#include <QString>
#include <QWidget>

namespace auth::theme {

inline constexpr int kDialogMinWidth = 420;
inline constexpr int kQrSize = 200;
inline constexpr int kCodeLength = 6;
inline constexpr int kLayoutMargin = 24;
inline constexpr int kLayoutSpacing = 12;

inline const QString kColorError = QStringLiteral("#ed4245");
inline const QString kColorSuccess = QStringLiteral("#57f287");

/** @brief Тёмная тема диалога (палитра прежнего SFML-окна). */
inline QString styleSheet() {
  return QStringLiteral(
      "QDialog { background-color: #2b2d31; }"
      "QLabel { color: #ffffff; }"
      "QLabel[role=\"subtitle\"] { color: #b9bbbe; }"
      "QLabel[role=\"status\"] { color: #ffffff; }"
      "QLabel[role=\"generated\"] { color: #57f287; font-size: 22px; "
      "font-weight: bold; }"
      "QLabel[role=\"title\"] { font-size: 20px; font-weight: bold; }"
      "QLineEdit { background-color: #1e1f22; color: #ffffff; "
      "border: 2px solid #1e1f22; border-radius: 4px; padding: 8px; "
      "selection-background-color: #5865f2; }"
      "QLineEdit:focus { border: 2px solid #5865f2; }"
      "QPushButton { background-color: #5865f2; color: #ffffff; "
      "font-weight: bold; border: none; border-radius: 4px; padding: 10px; }"
      "QPushButton:hover { background-color: #7289da; }"
      "QPushButton:disabled { background-color: #3a3d44; color: #b9bbbe; }"
      "QPushButton[role=\"secondary\"] { background-color: #313338; "
      "color: #ffffff; }"
      "QPushButton[role=\"secondary\"]:hover { background-color: #3f4147; }");
}

/** @brief Применяет тему к диалогу. */
inline void applyTo(QWidget *widget) {
  widget->setStyleSheet(styleSheet());
  widget->setMinimumWidth(kDialogMinWidth);
}

/** @brief Цвет статусной строки: ошибка или успех/инфо. */
inline QString statusStyle(bool error) {
  return error ? QStringLiteral("QLabel { color: %1; }").arg(kColorError)
               : QStringLiteral("QLabel { color: %1; }").arg(kColorSuccess);
}

} // namespace auth::theme

#endif // AUTH_THEME_HPP
