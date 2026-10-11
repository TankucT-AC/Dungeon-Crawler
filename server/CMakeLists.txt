cmake_minimum_required(VERSION 3.20)

# Подключается из корня через add_subdirectory(server),
# но умеет собираться и standalone.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(server CXX)
  include(CTest)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ни наш сервер, ни Drogon не используют C++20-модули, а P1689-сканирование
# в связке CMake 4 + GCC 16 падает (INCLUDE-TRANSLATE). Выключаем.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(SERVER_USE_SYSTEM_DROGON
    "Использовать системный Drogon, если он установлен" ON)
option(SERVER_FETCH_DROGON
    "Скачать Drogon через FetchContent, если системный не найден" ON)
set(SERVER_DROGON_TAG "v1.9.13"
    CACHE STRING "Тег Drogon для FetchContent")

# ─── Drogon: система → FetchContent → пропуск с предупреждением ───
if(SERVER_USE_SYSTEM_DROGON)
  find_package(Drogon CONFIG QUIET)
endif()

if(NOT Drogon_FOUND AND SERVER_FETCH_DROGON)
  include(FetchContent)

  # Только HTTP-ядро: ORM/СУБД/утилиты серверу не нужны
  # (SQLite используется напрямую через sqlite3.h).
  set(BUILD_CTL OFF CACHE BOOL "" FORCE)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_ORM OFF CACHE BOOL "" FORCE)
  set(BUILD_POSTGRESQL OFF CACHE BOOL "" FORCE)
  set(BUILD_MYSQL OFF CACHE BOOL "" FORCE)
  set(BUILD_SQLITE OFF CACHE BOOL "" FORCE)
  set(BUILD_REDIS OFF CACHE BOOL "" FORCE)
  set(BUILD_MONGO OFF CACHE BOOL "" FORCE)
  set(BUILD_BROTLI OFF CACHE BOOL "" FORCE)
  set(BUILD_YAML_CONFIG OFF CACHE BOOL "" FORCE)
  set(BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(COZ_PROFILING OFF CACHE BOOL "" FORCE)
  set(USE_SPDLOG OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
      drogon
      GIT_REPOSITORY https://github.com/drogonframework/drogon.git
      GIT_TAG ${SERVER_DROGON_TAG}
      GIT_SUBMODULES trantor
      GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(drogon)
endif()

if(NOT Drogon_FOUND AND NOT TARGET Drogon::Drogon)
  message(WARNING
      "Drogon не найден и SERVER_FETCH_DROGON=OFF — цель server пропускается. "
      "Соберите с -DBUILD_SERVER=OFF, чтобы скрыть это предупреждение.")
  return()
endif()

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(JSONCPP REQUIRED IMPORTED_TARGET jsoncpp)
endif()

set(SOURCE_FILES
    main.cc
    controllers/AuthController.cc
    controllers/DatabaseManager.cc
    controllers/TOTPGenerator.cc
    plugins/qrcodegen.cpp
)

add_executable(server ${SOURCE_FILES})

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
)

target_link_libraries(server PRIVATE
    Drogon::Drogon
    SQLite3::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    PkgConfig::JSONCPP
)

# ─── views/ опционален: пустого каталога в репозитории нет ───
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/views")
  if(NOT COMMAND drogon_create_views AND DEFINED drogon_SOURCE_DIR)
    include("${drogon_SOURCE_DIR}/cmake/DrogonUtilities.cmake" OPTIONAL)
  endif()
  if(COMMAND drogon_create_views)
    drogon_create_views(server
        ${CMAKE_CURRENT_SOURCE_DIR}/views ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()

# ─── config.json рядом с бинарником ───
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ─── Самоподписанный сертификат (в исходниках его нет, *.pem в игноре).
# Генерируется один раз: custom_command не перезапускается,
# пока файлы на месте.
find_program(OPENSSL_BIN openssl REQUIRED)
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/cert.pem
        ${CMAKE_CURRENT_BINARY_DIR}/key.pem
    COMMAND ${OPENSSL_BIN} req -x509 -newkey rsa:2048 -nodes
        -keyout ${CMAKE_CURRENT_BINARY_DIR}/key.pem
        -out ${CMAKE_CURRENT_BINARY_DIR}/cert.pem
        -days 825
        -subj "/CN=127.0.0.1"
        -addext "subjectAltName=IP:127.0.0.1,DNS:localhost"
    COMMENT "Генерация самоподписанного сертификата auth-сервера"
    VERBATIM
)
add_custom_target(server_certs
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/cert.pem
        ${CMAKE_CURRENT_BINARY_DIR}/key.pem
)
add_dependencies(server server_certs)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
