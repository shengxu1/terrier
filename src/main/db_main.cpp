#include "main/db_main.h"

#define __SETTING_GFLAGS_DEFINE__      // NOLINT
#include "settings/settings_common.h"  // NOLINT
#include "settings/settings_defs.h"    // NOLINT
#undef __SETTING_GFLAGS_DEFINE__       // NOLINT

namespace terrier {

void DBMain::Run() {
  TERRIER_ASSERT(network_layer_ != DISABLED, "Trying to run without a NetworkLayer.");
  const auto server = network_layer_->GetServer();
  server->RunServer();
  {
    std::unique_lock<std::mutex> lock(server->RunningMutex());
    server->RunningCV().wait(lock, [=] { return !(server->Running()); });
  }
}

void DBMain::ForceShutdown() {
  if (network_layer_ != DISABLED && network_layer_->GetServer()->Running()) {
    network_layer_->GetServer()->StopServer();
  }
}

DBMain::~DBMain() { ForceShutdown(); }

}  // namespace terrier
