// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <concepts>

namespace injectx::stdext {

template<typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

}  // namespace injectx::stdext
