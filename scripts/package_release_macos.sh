#!/usr/bin/env bash
# package_release_macos.sh - build a signed macOS .app release for
# Bloody Roar II Recompiled.
#
# Thin wrapper over the framework packager
# (psxrecomp/tools/package_release_macos.sh), mirroring how
# package_release_linux.sh assembles the Linux folders: the runtime is already
# built in <build-dir>; this only stages, signs and gates the bundle.
#
# Usage:
#   scripts/package_release_macos.sh <build-dir> <version> [--dmg] [--no-zip]
#
# Writes: release-macos/BloodyRoar2Recomp-macos-<arch>/Bloody Roar II
#         Recompiled.app plus BloodyRoar2Recomp-<version>-macos-<arch>.zip
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:?usage: $0 <build-dir> <version> [--dmg] [--no-zip]}"
VERSION="${2:?usage: $0 <build-dir> <version> [--dmg] [--no-zip]}"
shift 2 || true

if [[ ! -d "${BUILD_DIR}" ]]; then
  echo "error: build dir not found: ${BUILD_DIR}" >&2
  exit 1
fi
BUILD_DIR="$(cd "${BUILD_DIR}" && pwd)"

EXE_PATH="${BUILD_DIR}/BloodyRoar2_Recompiled"
VERSION_STAMP="${BUILD_DIR}/psx_game_version.txt"
if [[ ! -f "${EXE_PATH}" ]]; then
  echo "error: executable not found: ${EXE_PATH}" >&2
  exit 1
fi
if [[ ! -f "${VERSION_STAMP}" ]]; then
  echo "error: missing game-version stamp beside executable: ${VERSION_STAMP}" >&2
  exit 1
fi
BUILT_VERSION="$(tr -d '[:space:]' <"${VERSION_STAMP}")"
BUILT_VERSION="${BUILT_VERSION#v}"
if [[ "${BUILT_VERSION}" != "${VERSION}" ]]; then
  echo "error: requested version ${VERSION} does not match executable stamp ${BUILT_VERSION}" >&2
  echo "  Reconfigure with -DPSX_GAME_VERSION=${VERSION} and rebuild." >&2
  exit 1
fi

PACKAGER="${ROOT}/psxrecomp/tools/package_release_macos.sh"
if [[ ! -f "${PACKAGER}" ]]; then
  echo "error: framework packager missing: ${PACKAGER}" >&2
  exit 1
fi

# The packager resolves --game-config and --data relative to the repo root and
# stages them beside the executable by basename, so build/-relative paths work.
EXTRA_ARGS=()
for f in game_options.toml keybinds.ini input.ini game_us.toml; do
  if [[ -f "${BUILD_DIR}/${f}" ]]; then
    EXTRA_ARGS+=(--data "build/${f}")
  fi
done

bash "${PACKAGER}" \
  --repo "${ROOT}" \
  --build "${BUILD_DIR}" \
  --bin "BloodyRoar2_Recompiled" \
  --app-name "Bloody Roar II Recompiled" \
  --package "BloodyRoar2Recomp" \
  --bundle-id "com.novapowers0.bloodyroar2recomp" \
  --version "v${VERSION}" \
  --game-config "build/game.toml" \
  "${EXTRA_ARGS[@]}" \
  "$@"
