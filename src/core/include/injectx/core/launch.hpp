// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/core/bundle.hpp"
#include "injectx/core/export_macro.hpp"

namespace injectx::core {

INJECTX_CORE_EXPORT SetupTask<void> launch(Bundle bundle) noexcept;

}  // namespace injectx::core
