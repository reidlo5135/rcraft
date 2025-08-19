#!/usr/bin/env bash
set -euo pipefail
set -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_TYPE="${BUILD_TYPE:-Debug}"
GENERATOR="${GENERATOR:-MinGW Makefiles}"
PARALLEL="${CMAKE_BUILD_PARALLEL_LEVEL:-}"
QT_WDEP="/c/Qt/Qt5.12.4/5.12.4/mingw73_64/bin/windeployqt.exe"

if [[ $# -eq 0 || "${1:-}" == "all" ]]; then
  SELECTED=()
  for d in "${ROOT}"/rcraft_*; do [[ -d "$d" ]] && SELECTED+=("$(basename "$d")"); done
else
  SELECTED=("$@")
fi

for p in "${SELECTED[@]}"; do [[ -d "${ROOT}/${p}" ]] || { echo "[ERR] missing ${p}"; exit 1; }; done

ORDERED=()
for p in rcraft_map_server rcraft_path_planner rcraft_viz; do
  for q in "${SELECTED[@]}"; do [[ "$p" == "$q" ]] && ORDERED+=("$q"); done
done
for q in "${SELECTED[@]}"; do f=0; for r in "${ORDERED[@]}"; do [[ "$q" == "$r" ]] && f=1; done; [[ $f -eq 0 ]] && ORDERED+=("$q"); done
SELECTED=("${ORDERED[@]}")

nuke_pkg() {
  local pkg="$1"
  local inst="${ROOT}/${pkg}/install"
  rm -f  "${inst}/bin/lib${pkg}"*.dll 2>/dev/null || true
  rm -rf "${inst}/lib/cmake/${pkg}"  2>/dev/null || true
}

build_pkg() {
  local pkg="$1"
  local src="${ROOT}/${pkg}"
  local bld="${src}/build"
  local inst="${src}/install"
  rm -rf "$bld"

  if [[ "$pkg" == "rcraft_path_planner" ]]; then
    local dep="${ROOT}/rcraft_map_server/install"
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$inst" -DCMAKE_PREFIX_PATH="$dep"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi
  elif [[ "$pkg" == "rcraft_map_server" ]]; then
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$inst"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi
  elif [[ "$pkg" == "rcraft_viz" ]]; then
    local dep="${ROOT}/rcraft_map_server/install;${ROOT}/rcraft_path_planner/install"
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_PREFIX_PATH="$dep"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --parallel "$PARALLEL"; else cmake --build "$bld" --parallel; fi
    local exe="${bld}/rcraft_viz.exe"
    rm -f "${bld}/librcraft_"*.dll 2>/dev/null || true
    cp -f "${ROOT}/rcraft_map_server/install/bin/"librcraft_map_server*.dll "${bld}/" 2>/dev/null || true
    cp -f "${ROOT}/rcraft_path_planner/install/bin/"librcraft_path_planner*.dll "${bld}/" 2>/dev/null || true
    "$QT_WDEP" --no-translations --compiler-runtime --verbose 1 "$exe"
  else
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$inst"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi
  fi
}

for pkg in "${SELECTED[@]}"; do nuke_pkg "$pkg"; done
for pkg in "${SELECTED[@]}"; do build_pkg "$pkg"; done

for pkg in "${SELECTED[@]}"; do
  inst="${ROOT}/${pkg}/install"
  ls -l "${inst}/bin" 2>/dev/null || true
done