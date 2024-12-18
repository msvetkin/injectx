// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include <Windows.h>

namespace injectx::stdext::expects::tests {

struct SuppressAbortDialog {
  SuppressAbortDialog() {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
  }
} suppressAbortDialog;

}  // namespace injectx::stdext::expects::tests
