// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/network/AuthClient.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace auth;

namespace {

class FakeTransport : public IAuthTransport {
public:
  explicit FakeTransport(HttpResult preset) : preset_(preset) {}

  HttpResult post(const QString &path, const QByteArray &body) override {
    lastPath = path;
    lastBody = body;
    return preset_;
  }

  QString lastPath;
  QByteArray lastBody;

private:
  HttpResult preset_;
};

static HttpResult okJson(const std::string &json) {
  HttpResult r;
  r.transportOk = true;
  r.status = 200;
  r.body = QByteArray::fromStdString(json);
  return r;
}

static HttpResult httpError(int status) {
  HttpResult r;
  r.transportOk = true;
  r.status = status;
  return r;
}

static HttpResult networkError(const char *what) {
  HttpResult r;
  r.transportOk = false;
  r.errorString = QString::fromLatin1(what);
  return r;
}

static AuthClient makeClient(HttpResult preset, FakeTransport **outFake) {
  auto fake = std::make_unique<FakeTransport>(preset);
  *outFake = fake.get();
  return AuthClient(AuthConfig{}, std::move(fake));
}

} // namespace

TEST(AuthClientTest, LoginSuccess) {
  FakeTransport *fake = nullptr;
  AuthClient auth =
      makeClient(okJson(R"({"status":"success","password":"secret"})"), &fake);

  const AuthResult res = auth.login("user", "pass");

  EXPECT_TRUE(res.success);
  EXPECT_FALSE(res.requires2FA);
  EXPECT_EQ(res.password.toStdString(), "secret");
  EXPECT_EQ(fake->lastPath.toStdString(), "/api/login");
}

TEST(AuthClientTest, LoginRequires2FA) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(
      okJson(R"({"status":"success","is_totp_verify":true})"), &fake);

  const AuthResult res = auth.login("user", "pass");

  EXPECT_TRUE(res.success);
  EXPECT_TRUE(res.requires2FA);
}

TEST(AuthClientTest, LoginFailure) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(okJson(R"({"status":"fail"})"), &fake);

  const AuthResult res = auth.login("user", "wrong");

  EXPECT_FALSE(res.success);
  EXPECT_FALSE(res.requires2FA);
}

TEST(AuthClientTest, LoginNetworkError) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(networkError("Connection refused"), &fake);

  const AuthResult res = auth.login("user", "pass");

  EXPECT_FALSE(res.success);
  EXPECT_TRUE(res.message.startsWith("NET_ERR:"));
}

TEST(AuthClientTest, LoginServerError) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(httpError(500), &fake);

  const AuthResult res = auth.login("user", "pass");

  EXPECT_FALSE(res.success);
  EXPECT_EQ(res.message.toStdString(), "SERVER_ERROR_500");
}

TEST(AuthClientTest, LoginJsonError) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(okJson("this is not json"), &fake);

  const AuthResult res = auth.login("user", "pass");

  EXPECT_FALSE(res.success);
  EXPECT_EQ(res.message.toStdString(), "JSON_ERROR");
}

TEST(AuthClientTest, RegisterSuccess) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(okJson(R"({"status":"success"})"), &fake);

  const AuthResult res = auth.registerUser("newuser");

  EXPECT_TRUE(res.success);
  EXPECT_EQ(fake->lastPath.toStdString(), "/api/register");
}

TEST(AuthClientTest, Setup2FADecodesQr) {
  FakeTransport *fake = nullptr;
  // "aGVsbG8=" — base64 от "hello"
  AuthClient auth =
      makeClient(okJson(R"({"status":"success","qr_code_base64":"aGVsbG8=",
                  "totp_secret":"SECRET123"})"),
                 &fake);

  const AuthResult res = auth.setup2FA("user");

  EXPECT_TRUE(res.success);
  EXPECT_EQ(fake->lastPath.toStdString(), "/api/setup_2fa");
  EXPECT_EQ(res.qrPng.toStdString(), "hello");
  EXPECT_EQ(res.totpSecret.toStdString(), "SECRET123");
}

TEST(AuthClientTest, Verify2FASuccess) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(okJson(R"({"status":"success"})"), &fake);

  const AuthResult res = auth.verify2FA("user", "123456");

  EXPECT_TRUE(res.success);
  EXPECT_EQ(fake->lastPath.toStdString(), "/api/verify_2fa");
  EXPECT_TRUE(fake->lastBody.contains("totp_key"));
}

TEST(AuthClientTest, Verify2FAFailure) {
  FakeTransport *fake = nullptr;
  AuthClient auth = makeClient(okJson(R"({"status":"fail"})"), &fake);

  const AuthResult res = auth.verify2FA("user", "000000");

  EXPECT_FALSE(res.success);
}

TEST(AuthClientTest, CustomConfigPathsAreUsed) {
  AuthConfig cfg;
  cfg.loginPath = "/custom/login";
  auto fake =
      std::make_unique<FakeTransport>(okJson(R"({"status":"success"})"));
  FakeTransport *raw = fake.get();
  AuthClient auth(cfg, std::move(fake));

  const AuthResult res = auth.login("user", "pass");

  EXPECT_TRUE(res.success);
  EXPECT_EQ(raw->lastPath.toStdString(), "/custom/login");
}
