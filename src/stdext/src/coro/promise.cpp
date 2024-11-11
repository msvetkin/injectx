// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/coro/promise.hpp"

namespace injectx::stdext::coro {

bool resume_continuation::await_ready() const noexcept {
  return false;
}

void resume_continuation::await_resume() const noexcept {
}

stdext::suspend_always promise_continuation::initial_suspend() const noexcept {
  return {};
}

resume_continuation promise_continuation::final_suspend() const noexcept {
  return {};
}

#ifdef INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER
void promise_continuation::resume_awaiting() noexcept {
  if (state_.exchange(true, std::memory_order_acq_rel)) {
    std::exchange(continuation_, stdext::noop_coroutine()).resume();
  }
}
#else   // symmetric transfer
stdext::coroutine_handle<> promise_continuation::resume_awaiting() noexcept {
  return std::exchange(continuation_, stdext::noop_coroutine());
}
#endif  // INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER

}  // namespace injectx::stdext::coro
