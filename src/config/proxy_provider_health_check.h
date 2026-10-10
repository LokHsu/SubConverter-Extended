#ifndef PROXY_PROVIDER_HEALTH_CHECK_H_INCLUDED
#define PROXY_PROVIDER_HEALTH_CHECK_H_INCLUDED

#include <cctype>
#include <string>
#include <string_view>

#include "config/proxy_provider_interval.h"

inline constexpr int kDefaultProxyProviderHealthCheckInterval = 300;
inline constexpr char kDefaultProxyProviderHealthCheckUrl[] =
    "https://www.gstatic.com/generate_204";

// A zero-second interval would make the client probe without pause, so only
// positive integers are accepted; the range check also rejects values that do
// not fit the generated integer field.
inline bool parseProxyProviderHealthCheckInterval(std::string_view input,
                                                  int &result) {
  int value = 0;
  if (!parseProxyProviderInterval(input, value) || value < 1)
    return false;
  result = value;
  return true;
}

// Only absolute http/https URLs without surrounding whitespace, embedded
// whitespace or control characters can be emitted safely into the generated
// YAML scalar.
inline bool parseProxyProviderHealthCheckUrl(std::string_view input,
                                             std::string &result) {
  while (!input.empty() &&
         std::isspace(static_cast<unsigned char>(input.front())))
    input.remove_prefix(1);
  while (!input.empty() &&
         std::isspace(static_cast<unsigned char>(input.back())))
    input.remove_suffix(1);
  if (input.empty())
    return false;

  for (unsigned char ch : input) {
    if (std::isspace(ch) || std::iscntrl(ch))
      return false;
  }

  const bool http = input.rfind("http://", 0) == 0;
  const bool https = input.rfind("https://", 0) == 0;
  if (!http && !https)
    return false;

  result.assign(input.begin(), input.end());
  return true;
}

#endif // PROXY_PROVIDER_HEALTH_CHECK_H_INCLUDED
