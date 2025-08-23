#!/usr/bin/env bash
set -euo pipefail

# --- parse knobs ---
VRAW="debug"
COV="off"

for arg in "$@"; do
  case "$arg" in
    variant=coverage) VRAW="debug"; COV="on";;
    variant=debug|variant=memory|variant=static|variant=shared) VRAW="${arg#variant=}";;
    coverage=on) COV="on";;
    coverage=off) COV="off";;
  esac
done

case "$VRAW" in
  debug|memory|static|shared) ;;
  help|-h|--help)
    cat <<EOF
Usage: ./build.sh [variant=debug|memory|static|shared|coverage] [coverage=on|off] [extra CMake args]
Notes:
  - 'variant=coverage' => shorthand for debug + -DA_ENABLE_COVERAGE=ON
  - Prefer a unified build so the library is instrumented too.
EOF
    exit 0;;
  *) echo "Unknown variant: '$VRAW'"; exit 1;;
esac

VAR_CMAKE="${VRAW//-/_}"

# --- banner ---
cat <<EOF
▶ Building tests with:
  A_BUILD_VARIANT=${VAR_CMAKE}
  A_ENABLE_COVERAGE=${COV}
EOF

# --- choose -j ---
if command -v nproc >/dev/null 2>&1; then
  JOBS="$(nproc)"
elif command -v getconf >/dev/null 2>&1; then
  JOBS="$(getconf _NPROCESSORS_ONLN || echo 4)"
elif command -v sysctl >/dev/null 2>&1; then
  JOBS="$(sysctl -n hw.ncpu || echo 4)"
else
  JOBS=4
fi

# --- configure, build ---
rm -rf build
mkdir -p build
cd build

cmake .. \
  -DA_BUILD_VARIANT="${VAR_CMAKE}" \
  -DA_ENABLE_COVERAGE=$([ "$COV" = "on" ] && echo ON || echo OFF) \
  "$@"

cmake --build . -j "${JOBS}"
