// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/core/export_macro.hpp"

#include <string>

namespace injectx::core {

[[nodiscard]] INJECTX_CORE_EXPORT std::string version() noexcept;

}  // namespace injectx::core
