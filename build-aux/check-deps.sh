#!/usr/bin/env bash

set -u

failures=0
selected_gcc_bin=""
selected_gcc_version=""
selected_gcc_major=""
selected_plugin_include=""

MAKE_CONFIG=${MAKE_CONFIG:-build/make-config.mk}
WITH_PREDATOR=${WITH_PREDATOR:-ON}
TARGET_GCC_OVERRIDE=${TARGET_GCC:-}
GCC_PLUGIN_INCLUDE_OVERRIDE=${GCC_PLUGIN_INCLUDE_DIR:-}

have_command() {
	command -v "$1" >/dev/null 2>&1
}

canonicalize_path() {
	if have_command realpath; then
		realpath "$1"
	else
		readlink -f "$1"
	fi
}

ok() {
	printf '[ok] %s\n' "$1"
}

info() {
	printf '[info] %s\n' "$1"
}

missing() {
	printf '[missing] %s\n' "$1"
	failures=$((failures + 1))
}

section() {
	printf '\n%s\n' "$1"
}

read_make_config_var() {
	local key=$1

	if [[ ! -f "$MAKE_CONFIG" ]]; then
		return 1
	fi

	sed -n "s/^${key} := //p" "$MAKE_CONFIG" | head -n 1
}

resolve_binary() {
	local candidate=$1

	if [[ -z "$candidate" ]]; then
		return 1
	fi

	if [[ "$candidate" = /* ]]; then
		[[ -x "$candidate" ]] || return 1
		canonicalize_path "$candidate"
	else
		command -v "$candidate"
	fi
}

select_gcc_candidate() {
	local cached_target_gcc=""
	local version

	if [[ -n "$TARGET_GCC_OVERRIDE" ]]; then
		printf '%s\n' "$TARGET_GCC_OVERRIDE"
		return 0
	fi

	cached_target_gcc=$(read_make_config_var TARGET_GCC 2>/dev/null || true)
	if [[ -n "$cached_target_gcc" ]]; then
		printf '%s\n' "$cached_target_gcc"
		return 0
	fi

	for version in $(seq 12 20); do
		if have_command "gcc-${version}"; then
			printf 'gcc-%s\n' "$version"
			return 0
		fi
	done

	if have_command gcc; then
		printf 'gcc\n'
		return 0
	fi

	return 1
}

check_required_command() {
	local cmd=$1
	local purpose=$2

	if have_command "$cmd"; then
		ok "${cmd}: $(command -v "$cmd") (${purpose})"
	else
		missing "${cmd}: command not found (${purpose})"
	fi
}

check_optional_command() {
	local cmd=$1
	local purpose=$2

	if have_command "$cmd"; then
		ok "${cmd}: $(command -v "$cmd") (${purpose})"
	else
		info "${cmd}: not found (${purpose})"
	fi
}

check_c_compiler() {
	local cc_bin

	if ! cc_bin=$(resolve_binary "${CC_FOR_BUILD:-cc}"); then
		missing "cc: C compiler not found for CMake configuration"
		return
	fi

	if printf 'int main() { return 0; }\n' | "$cc_bin" -x c -c -o /dev/null - >/dev/null 2>&1; then
		ok "C compiler: ${cc_bin}"
	else
		missing "C compiler: ${cc_bin} cannot compile a trivial C input"
	fi
}

check_cxx_compiler() {
	local cxx_candidate="${CXX_FOR_BUILD:-${CXX:-c++}}"
	local cxx_bin

	if ! cxx_bin=$(resolve_binary "$cxx_candidate"); then
		if ! cxx_bin=$(resolve_binary g++ 2>/dev/null); then
			missing "c++: C++ compiler not found for CMake configuration"
			return
		fi
	fi

	if printf 'int main() { return 0; }\n' | "$cxx_bin" -std=gnu++23 -x c++ -c -o /dev/null - >/dev/null 2>&1; then
		ok "C++ compiler: ${cxx_bin} (supports -std=gnu++23)"
	else
		missing "C++ compiler: ${cxx_bin} does not accept -std=gnu++23"
	fi
}

check_selected_gcc() {
	local candidate
	local gcc_bin
	local plugin_dir

	if ! candidate=$(select_gcc_candidate); then
		missing "target GCC: no GCC candidate >= 12 was found"
		return
	fi

	if ! gcc_bin=$(resolve_binary "$candidate"); then
		missing "target GCC: could not resolve '${candidate}'"
		return
	fi

	selected_gcc_bin=$gcc_bin
	selected_gcc_version=$(
		"$gcc_bin" -dumpfullversion 2>/dev/null || "$gcc_bin" -dumpversion 2>/dev/null
	)
	selected_gcc_major=$(printf '%s' "$selected_gcc_version" | sed -E 's/^([0-9]+).*/\1/')

	if [[ -z "$selected_gcc_major" ]]; then
		missing "target GCC: could not determine version for ${selected_gcc_bin}"
		return
	fi

	if ((selected_gcc_major < 12)); then
		missing "target GCC: ${selected_gcc_bin} is version ${selected_gcc_version}, but GCC >= 12 is required"
	else
		ok "target GCC: ${selected_gcc_bin} (version ${selected_gcc_version})"
	fi

	if printf 'int main(void) { return 0; }\n' | "$selected_gcc_bin" -x c -S -o /dev/null - >/dev/null 2>&1; then
		ok "target GCC compile probe: ${selected_gcc_bin} can compile a trivial C input"
	else
		missing "target GCC compile probe: ${selected_gcc_bin} failed on a trivial C input"
	fi

	if [[ -n "$GCC_PLUGIN_INCLUDE_OVERRIDE" ]]; then
		selected_plugin_include=$GCC_PLUGIN_INCLUDE_OVERRIDE
		info "using GCC_PLUGIN_INCLUDE_DIR override: ${selected_plugin_include}"
	else
		plugin_dir=$(
			"$selected_gcc_bin" -print-file-name=plugin 2>/dev/null
		)
		selected_plugin_include="${plugin_dir}/include"
	fi

	if [[ -f "${selected_plugin_include}/gcc-plugin.h" ]]; then
		ok "gcc-plugin.h: ${selected_plugin_include}/gcc-plugin.h"
	else
		missing "gcc-plugin.h: not found under ${selected_plugin_include}"
	fi
}

print_debian_package_hints() {
	local packages=(bash make cmake jq)
	local hint_gcc_major="${selected_gcc_major:-12}"
	local pkg

	if [[ -n "$selected_gcc_major" ]]; then
		packages+=("gcc-${selected_gcc_major}" "g++-${selected_gcc_major}" "gcc-${selected_gcc_major}-plugin-dev")
	else
		packages+=("gcc-${hint_gcc_major}" "g++-${hint_gcc_major}" "gcc-${hint_gcc_major}-plugin-dev")
	fi

	if [[ "$WITH_PREDATOR" = "ON" ]]; then
		packages+=(patch)
	fi

	if ! have_command dpkg-query; then
		info "Debian/Ubuntu package hint: sudo apt install gcc-${hint_gcc_major} g++-${hint_gcc_major} gcc-${hint_gcc_major}-plugin-dev cmake jq$([[ "$WITH_PREDATOR" = "ON" ]] && printf ' patch')"
		return
	fi

	section "Debian/Ubuntu package status"
	for pkg in "${packages[@]}"; do
		if dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q 'install ok installed'; then
			info "${pkg}: installed"
		else
			info "${pkg}: not installed via dpkg (or not managed by dpkg)"
		fi
	done
}

main() {
	section "Required tools"
	check_required_command bash "shell-based test runners"
	check_required_command make "top-level build wrapper"
	check_required_command cmake "configuration and build generation"
	check_required_command ctest "test execution"
	check_required_command jq "JSON assertions in test suites"
	check_c_compiler
	check_cxx_compiler

	section "GCC plugin toolchain"
	check_selected_gcc

	section "Predator extras"
	if [[ "$WITH_PREDATOR" = "ON" ]]; then
		check_required_command patch "Predator shim patching"
	else
		info "WITH_PREDATOR=OFF, skipping Predator-only dependencies"
	fi

	section "Optional helpers"
	check_optional_command git "fresh clone, submodule maintenance, and clean-predator"
	check_optional_command dot "rendering SVG output for make dot-multi"
	check_optional_command doxygen "API documentation generation"

	printf '\n'
	if ((failures > 0)); then
		print_debian_package_hints
		printf '\n'
		printf 'Dependency check failed: %d required item(s) are missing or unusable.\n' "$failures"
		exit 1
	fi

	printf 'Dependency check passed.\n'
}

main "$@"
