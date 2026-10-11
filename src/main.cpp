// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "src/core/config.hpp"
#include "src/network/AuthClient.hpp"
#include "src/network/AuthDialog.hpp"
#include <QApplication>
#include <QDialog>
#include <iostream>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  try {
#if !DEBUG_NOT_AUTH
    QApplication app(argc, argv);
    auth::AuthClient auth;
    auth::AuthDialog authDialog(auth);

    if (authDialog.exec() != QDialog::Accepted) {
      std::cout << "[Main] Авторизация не удалась или окно закрыто. Выход.\n";
      return EXIT_SUCCESS;
    }
#endif // DEBUG_NOT_AUTH

    // Если дошли сюда, значит авторизация успешна (или дебаг)
    Engine engine;
    engine.run();

  } catch (const std::exception &e) {
    std::cerr << "FATAL: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
