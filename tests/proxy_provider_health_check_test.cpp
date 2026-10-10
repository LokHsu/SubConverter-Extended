#include <climits>
#include <iostream>
#include <string>

#include "config/proxy_provider_health_check.h"

static bool expectIntervalAccepted(const std::string &input, int expected) {
  int value = -1;
  if (!parseProxyProviderHealthCheckInterval(input, value)) {
    std::cerr << "expected accepted health-check interval: " << input << '\n';
    return false;
  }
  if (value != expected) {
    std::cerr << "unexpected health-check interval for: " << input << '\n';
    return false;
  }
  return true;
}

static bool expectIntervalRejected(const std::string &input) {
  int value = 123;
  if (parseProxyProviderHealthCheckInterval(input, value)) {
    std::cerr << "expected rejected health-check interval: " << input << '\n';
    return false;
  }
  if (value != 123) {
    std::cerr << "rejected health-check interval mutated output: " << input
              << '\n';
    return false;
  }
  return true;
}

static bool expectUrlAccepted(const std::string &input,
                              const std::string &expected) {
  std::string value;
  if (!parseProxyProviderHealthCheckUrl(input, value)) {
    std::cerr << "expected accepted health-check url: " << input << '\n';
    return false;
  }
  if (value != expected) {
    std::cerr << "unexpected health-check url for: " << input << '\n';
    return false;
  }
  return true;
}

static bool expectUrlRejected(const std::string &input) {
  std::string value = "sentinel";
  if (parseProxyProviderHealthCheckUrl(input, value)) {
    std::cerr << "expected rejected health-check url: " << input << '\n';
    return false;
  }
  if (value != "sentinel") {
    std::cerr << "rejected health-check url mutated output: " << input << '\n';
    return false;
  }
  return true;
}

int main() {
  bool ok = kDefaultProxyProviderHealthCheckInterval == 300;
  ok = std::string(kDefaultProxyProviderHealthCheckUrl) ==
           "https://www.gstatic.com/generate_204" &&
       ok;

  ok = expectIntervalAccepted("60", 60) && ok;
  ok = expectIntervalAccepted("300", 300) && ok;
  ok = expectIntervalAccepted(" 7200 ", 7200) && ok;
  ok = expectIntervalAccepted(std::to_string(INT_MAX), INT_MAX) && ok;

  ok = expectIntervalRejected("0") && ok;
  ok = expectIntervalRejected("") && ok;
  ok = expectIntervalRejected(" ") && ok;
  ok = expectIntervalRejected("-1") && ok;
  ok = expectIntervalRejected("+1") && ok;
  ok = expectIntervalRejected("none") && ok;
  ok = expectIntervalRejected("300s") && ok;
  ok = expectIntervalRejected("300.5") && ok;
  ok = expectIntervalRejected("2147483648") && ok;

  ok = expectUrlAccepted("https://www.gstatic.com/generate_204",
                         "https://www.gstatic.com/generate_204") &&
       ok;
  ok = expectUrlAccepted("http://cp.cloudflare.com/generate_204",
                         "http://cp.cloudflare.com/generate_204") &&
       ok;
  ok = expectUrlAccepted(
      "  https://example.com/generate_204?target=health  ",
      "https://example.com/generate_204?target=health") &&
       ok;

  ok = expectUrlRejected("") && ok;
  ok = expectUrlRejected("   ") && ok;
  ok = expectUrlRejected("generate_204") && ok;
  ok = expectUrlRejected("ftp://example.com/generate_204") && ok;
  ok = expectUrlRejected("https://example.com/a b") && ok;
  ok = expectUrlRejected("https://example.com/a\tb") && ok;
  ok = expectUrlRejected(std::string("https://example.com/\x01")) && ok;

  return ok ? 0 : 1;
}
