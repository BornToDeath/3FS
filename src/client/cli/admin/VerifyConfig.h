#pragma once

#include "common/utils/Coroutine.h"

namespace hf3fs::client::cli {
class Dispatcher;
CoTryTask<void> registerVerifyConfigHandler(Dispatcher &dispatcher);
}  // namespace hf3fs::client::cli
