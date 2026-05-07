#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
# SPDX-FileCopyrightText: 2024–2025 Knode.ai
# SPDX-License-Identifier: Apache-2.0
#
# Maintainer: Andy Curtis <contactandyc@gmail.com>

set -Eeuo pipefail

# --- Discover and source scoped environment ---
_cur="$PWD"
while [ "$_cur" != "/" ]; do
  if [ -f "$_cur/.scaffoldrc.yaml" ]; then
    [ -f "$_cur/.scaffoldrc_c_cmake" ] && source "$_cur/.scaffoldrc_c_cmake"
    break
  fi
  _cur="$(dirname "$_cur")"
done
[ -z "${WORKSPACE_DIR:-}" ] && [ -f "$HOME/.scaffoldrc_c_cmake" ] && source "$HOME/.scaffoldrc_c_cmake"

# --- Script Commands ---
COMMAND="${1:-build}" # Default to 'build' if no command is given

# --- Knobs (Absorbs exports from .scaffoldrc naturally) ---
PREFIX="${PREFIX:-/usr/local}"
BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
BUILD_VARIANT="${BUILD_VARIANT:-debug}"
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
  bootstrap)
    echo "--- Bootstrapping Hermetic Workspace ---"

    # --- FAILSAFE: Shield against leaky parent environments ---
    export PREFIX="$PWD/repos/install"
    export WORKSPACE_DIR="$PWD/repos"

    # 1. Fetch the scaffolding engine to disk so it is visible and editable
    mkdir -p repos
    if [ ! -d "repos/scaffold-repo" ]; then
        echo "📦 Fetching scaffold engine into repos/scaffold-repo..."
        git clone --branch "main" \
            "https://github.com/contactandyc/scaffold-repo.git" \
            "repos/scaffold-repo"
    else
        echo "📦 Scaffold engine already exists at repos/scaffold-repo. Leaving untouched."
    fi

    # 2. Isolate the Python scaffolding environment (The Sandbox)
    if [ ! -d ".scaffold/venv" ]; then
        echo "Creating isolated Python sandbox..."
        python3 -m venv .scaffold/venv
        source .scaffold/venv/bin/activate
        pip install --upgrade pip -q
    else
        source .scaffold/venv/bin/activate
    fi

    # ALWAYS sync the engine's entry points into the sandbox
    echo "Installing scaffold engine into sandbox..."
    pip install -q -e repos/scaffold-repo

    # 3. Generate local hermetic configs & clone dependencies recursively
    scaffold-bootstrap

    # 4. Compile the newly cloned dependencies
    scaffold-repo --build-deps

    echo ""
    echo "✅ Bootstrap complete! All dependencies are isolated in your workspace."

    # Automatically cascade into a first-party build!
    "$0" build
    ;;

  build|install)
    pick_generator
    echo "--- Building Project (Generator: $GENERATOR, Variant: $BUILD_VARIANT) ---"

    cmake -S . -B "$BUILD_DIR" -G "$GENERATOR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_PREFIX_PATH="$PREFIX" \
      -DCMAKE_INSTALL_PREFIX="$PREFIX" \
      -DA_BUILD_VARIANT="$BUILD_VARIANT"

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

  semver-check)
    OLD_REF="${2:-}"

    if [ -z "$OLD_REF" ]; then
      # Fallback if this is the very first release and no previous tags exist
      echo "PATCH"
      exit 0
    fi

    if ! command -v abidiff &> /dev/null; then
      echo "Error: abidiff not found. Are you running this inside the dev container?" >&2
      exit 1
    fi

    # 1. Build the old reference in a temporary worktree
    OLD_DIR=$(mktemp -d)
    git worktree add "$OLD_DIR" "$OLD_REF" >/dev/null 2>&1

    # Force 'shared' variant so abidiff can easily analyze the exported symbols
    cmake -S "$OLD_DIR" -B "$OLD_DIR/build" -DA_BUILD_VARIANT=shared -DCMAKE_BUILD_TYPE=Debug >/dev/null 2>&1
    cmake --build "$OLD_DIR/build" -j >/dev/null 2>&1

    # 2. Build the current tree
    cmake -S . -B build -DA_BUILD_VARIANT=shared -DCMAKE_BUILD_TYPE=Debug >/dev/null 2>&1
    cmake --build build -j >/dev/null 2>&1

    # 3. Compare the shared libraries
    OLD_SO="$OLD_DIR/build/libthe_macro_library_shared.so"
    NEW_SO="build/libthe_macro_library_shared.so"

    # Temporarily disable 'set -e' because abidiff returns non-zero if it finds differences
    set +e
    abidiff "$OLD_SO" "$NEW_SO" > /dev/null
    ABI_STATUS=$?
    set -e

    # Clean up the temporary git worktree
    git worktree remove "$OLD_DIR" --force >/dev/null 2>&1

    # 4. Parse libabigail exit codes (it uses bitmasks)
    # Bit 2 (4): ABI breaking change (e.g., changed struct size, removed function) -> MAJOR
    # Bit 3 (8): ABI compatible change (e.g., added a new function) -> MINOR

    if [ $((ABI_STATUS & 4)) -ne 0 ]; then
      echo "MAJOR"
    elif [ $((ABI_STATUS & 8)) -ne 0 ]; then
      echo "MINOR"
    else
      echo "PATCH"
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
    echo "Usage: $0 [bootstrap|build|install|coverage|clean]" >&2
    exit 1
    ;;
esac
