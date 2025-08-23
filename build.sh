#!/usr/bin/env bash
set -Eeuo pipefail

# --- Script Commands ---
COMMAND="${1:-build}" # Default to 'build' if no command is given

# --- Knobs (can be set as environment variables) ---
PREFIX="${PREFIX:-/usr/local}"
BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
GENERATOR="${GENERATOR:-}" # Auto-detects if empty

# --- Helper: auto-detect CMake generator ---
pick_generator() {
  if [ -n "$GENERATOR" ]; then return; fi
  if command -v ninja >/dev/null; then GENERATOR="Ninja"
  elif command -v make >/dev/null; then GENERATOR="Unix Makefiles"
  else echo "Error: No build tool found. Install Ninja or Make." >&2; exit 1; fi
}

# --- Main Logic ---
case "$COMMAND" in
  build|install)
    pick_generator
    echo "--- Building Project (Generator: $GENERATOR) ---"

    cmake -S . -B "$BUILD_DIR" -G "$GENERATOR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_INSTALL_PREFIX="$PREFIX"

    cmake --build "$BUILD_DIR" -j

    if [ "$COMMAND" = "install" ]; then
      echo "--- Installing to '$PREFIX' ---"
      INSTALL_SUDO=""
      if [[ "$PREFIX" == "/usr"* ]] && [[ "${EUID:-$(id -u)}" -ne 0 ]]; then
        INSTALL_SUDO="sudo"
      fi
      $INSTALL_SUDO cmake --install "$BUILD_DIR"
      echo "✅ Install complete."
    else
      echo "✅ Build complete. Artifacts are in '$BUILD_DIR/'."
    fi
    ;;

  coverage)
    echo "--- Cleaning Build Directories ---"
    rm -rf "$BUILD_DIR" "build-coverage"
    echo "✅ Clean complete."

    pick_generator
    COV_BUILD_DIR="build-coverage"
    echo "--- Running Coverage (Generator: $GENERATOR) ---"

    cmake -S . -B "$COV_BUILD_DIR" -G "$GENERATOR" \
      -DA_ENABLE_COVERAGE=ON \
      -DCMAKE_BUILD_TYPE=Debug \
      -DA_BUILD_VARIANT=debug

    cmake --build "$COV_BUILD_DIR" -j
    (cd "$COV_BUILD_DIR" && ctest --output-on-failure)
    cmake --build "$COV_BUILD_DIR" --target coverage_report

    COVERAGE_FILE="$COV_BUILD_DIR/tests/coverage_html/index.html"

    echo ""
    echo "--- Coverage summary (console) ---"
    # Header-only: summarize from all test executables
    for exe in "$COV_BUILD_DIR"/tests/test_*; do
      if [ -x "$exe" ]; then
        llvm-cov report "$exe" -instr-profile="$COV_BUILD_DIR/tests/default.profdata" || true
      fi
    done

    echo ""
    echo "✅ Coverage report generated successfully."
    echo "   View HTML report: open '$COVERAGE_FILE'"
    ;;

  clean)
    echo "--- Cleaning Build Directories ---"
    rm -rf "$BUILD_DIR" "build-coverage"
    echo "✅ Clean complete."
    ;;

  *)
    echo "Usage: $0 [build|install|coverage|clean]" >&2
    exit 1
    ;;
esac
