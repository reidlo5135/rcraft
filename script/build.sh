#!/usr/bin/env bash
set -euo pipefail
set -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
STAGE="${ROOT}/rcraft_libs"
STAGE_BIN="${STAGE}/bin"
BUILD_TYPE="${BUILD_TYPE:-Debug}"
GENERATOR="${GENERATOR:-MinGW Makefiles}"
PARALLEL="${CMAKE_BUILD_PARALLEL_LEVEL:-}"
QT_WDEP="/c/Qt/Qt5.12.4/5.12.4/mingw73_64/bin/windeployqt.exe"

mkdir -p "${STAGE_BIN}"

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

nuke_one() {
  local pkg="$1"
  case "$pkg" in
    rcraft_map_server)
      rm -f "${STAGE_BIN}/librcraft_map_server"*.dll || true
      rm -rf "${STAGE}/lib/cmake/rcraft_map_server" || true
      ;;
    rcraft_path_planner)
      rm -f "${STAGE_BIN}/librcraft_path_planner"*.dll || true
      rm -rf "${STAGE}/lib/cmake/rcraft_path_planner" || true
      ;;
    rcraft_viz) : ;;
    *)
      rm -f "${STAGE_BIN}/lib${pkg}"*.dll || true
      rm -rf "${STAGE}/lib/cmake/${pkg}" || true
      ;;
  esac
}

build_one() {
  local pkg="$1"
  local src="${ROOT}/${pkg}"
  local bld="${src}/build"
  rm -rf "$bld"

  if [[ "$pkg" == "rcraft_map_server" ]]; then
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$STAGE"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi

  elif [[ "$pkg" == "rcraft_path_planner" ]]; then
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$STAGE" -DCMAKE_PREFIX_PATH="$STAGE"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi

  elif [[ "$pkg" == "rcraft_viz" ]]; then
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_PREFIX_PATH="$STAGE"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --parallel "$PARALLEL"; else cmake --build "$bld" --parallel; fi
    local exe="${bld}/rcraft_viz.exe"
    rm -f "${bld}/librcraft_"*.dll || true
    cp -f "${STAGE_BIN}/"librcraft_map_server*.dll "${bld}/" 2>/dev/null || true
    cp -f "${STAGE_BIN}/"librcraft_path_planner*.dll "${bld}/" 2>/dev/null || true
    "$QT_WDEP" --no-translations --compiler-runtime --verbose 1 "$exe"

  else
    cmake -S "$src" -B "$bld" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$STAGE"
    if [[ -n "$PARALLEL" ]]; then cmake --build "$bld" --target install --parallel "$PARALLEL"; else cmake --build "$bld" --target install --parallel; fi
  fi
}

for pkg in "${SELECTED[@]}"; do nuke_one "$pkg"; done
for pkg in "${SELECTED[@]}"; do build_one "$pkg"; done

ls -l "${STAGE_BIN}" || true
