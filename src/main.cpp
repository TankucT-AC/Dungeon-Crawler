// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/AppOptions.hpp"
#include "src/core/Engine.hpp"
#include "src/network/AuthClient.hpp"
#include "src/network/AuthDialog.hpp"
#include <QApplication>
#include <QDialog>
#include <iostream>

int main(int argc, char *argv[]) {
  const AppOptions opts = AppOptions::parse(argc, argv);
  try {
    if (!opts.skipAuth) {
      QApplication app(argc, argv);
      auth::AuthClient auth;
      auth::AuthDialog authDialog(auth);

      if (authDialog.exec() != QDialog::Accepted) {
        std::cout << "[Main] Авторизация не удалась или окно закрыто. Выход.\n";
        return EXIT_SUCCESS;
      }
    }

    // Если дошли сюда, значит авторизация успешна (или пропущена флагом).
    Engine engine(opts.debugDraw());
    engine.run();

  } catch (const std::exception &e) {
    std::cerr << "FATAL: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
