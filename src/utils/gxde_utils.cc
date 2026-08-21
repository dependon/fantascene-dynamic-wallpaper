/*
 * Copyright (C) 2026 CharOfString <root@charofstring.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wayland-client.h>

#include <cstdlib>
#include <cstring>

#include "./gxde_utils.h"

namespace Utils {
namespace GXDE {
namespace {
constexpr char kGXDEIdentifierInterface[] = "gxde-identifier-v1";

struct WaylandState {
  bool has_gxde_identifier = false;
};

bool IsWaylandSession() {
  const char* session_type = std::getenv("XDG_SESSION_TYPE");
  if (session_type && session_type[0] != '\0') {
    return std::strcmp(session_type, "wayland") == 0;
  }

  const char* wayland_display = std::getenv("WAYLAND_DISPLAY");
  return wayland_display && wayland_display[0] != '\0';
}

void RegistryHandleGlobal(void* data, struct wl_registry*, uint32_t,
    const char* interface, uint32_t) {
  WaylandState* state = static_cast<WaylandState*>(data);
  if (interface && std::strcmp(interface, kGXDEIdentifierInterface) == 0) {
    state->has_gxde_identifier = true;
  }
}

void RegistryHandleGlobalRemove(void*, struct wl_registry*, uint32_t) {}

const struct wl_registry_listener kRegistryListener = {
    RegistryHandleGlobal,
    RegistryHandleGlobalRemove,
};
}  // namespace

bool IsGXWM() {
  if (!IsWaylandSession()) {
    return false;
  }

  struct wl_display* display = wl_display_connect(nullptr);
  if (!display) {
    return false;
  }

  struct wl_registry* registry = wl_display_get_registry(display);
  if (!registry) {
    wl_display_disconnect(display);
    return false;
  }

  WaylandState state;
  if (wl_registry_add_listener(registry, &kRegistryListener, &state) != 0 ||
      wl_display_roundtrip(display) < 0) {
    wl_registry_destroy(registry);
    wl_display_disconnect(display);
    return false;
  }

  wl_registry_destroy(registry);
  wl_display_disconnect(display);

  return state.has_gxde_identifier;
}

}  // namespace GXDE
}  // namespace Utils
