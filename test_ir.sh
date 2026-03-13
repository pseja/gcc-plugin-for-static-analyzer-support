#!/bin/bash

# command line options
RUN_INTEGRATION_TESTS=false
VISUALIZE_RESULTS=false
CLEAR_OLD_OUTPUTS=false

# directory paths
TEST_DIR="tests"
INTEGRATION_TEST_DIR="$TEST_DIR/integration_tests"

show_usage() {
	cat <<EOF
Usage: $(basename "${0}") [OPTIONS]

OPTIONS:
    -i, --integration
        Run integration tests
    -v, --visualize
        Visualize results
    -c, --clear
        Clear old test output files

EOF
	exit 0
}

clear() {
	echo -n "Clearing old test output files..."

	for file in $INTEGRATION_TEST_DIR/*.{dot,json,png}; do
		rm "$file" || true
	done

	echo " Done."
}

integration_tests() {
	echo "Running integration tests..."

	for file in $INTEGRATION_TEST_DIR/*.c; do
		echo "  > Compiling $(basename "$file")"

		json_file="$INTEGRATION_TEST_DIR/$(basename "$file" .c).json"
		touch "$json_file"
		gcc-12 -fplugin=build/libcl.so -fplugin-arg-libcl-gen-json="$json_file" "$file" 2>/dev/null
	done
}

visualize_results() {
	echo "Visualizing results..."

	for file in $INTEGRATION_TEST_DIR/*.c; do
		echo -n "  > Visualizing $(basename "$file")"

		base_name="$(basename "$file" .c)"
		dot_file="$INTEGRATION_TEST_DIR/"$base_name".dot"
		touch "$dot_file"
		png_file="$INTEGRATION_TEST_DIR/"$base_name".png"

		gcc-12 -fplugin=build/libcl.so -fplugin-arg-libcl-gen-dot="$dot_file" "$file" 2>/dev/null

		dot -Tpng $dot_file -o $png_file

		echo " Done."
	done
}

main() {
	if ! TEMP=$(getopt -o 'ivc' --long 'integration,visualize,clear' -n "$(basename "${0}")" -- "${@}" 2>&1); then
		echo "[Error] Invalid option provided" >&2
		show_usage
		return 1
	fi

	eval set -- "${TEMP}"

	# process command line options
	while true; do
		case "${1}" in
		-i | --integration)
			RUN_INTEGRATION_TESTS=true
			shift
			;;
		-v | --visualize)
			VISUALIZE_RESULTS=true
			shift
			;;
		-c | --clear)
			CLEAR_OLD_OUTPUTS=true
			shift
			;;
		--)
			shift
			break
			;;
		*)
			echo "[Error] Invalid option: ${1}" >&2
			show_usage
			;;
		esac
	done

	if [ "$CLEAR_OLD_OUTPUTS" = false ] && [ "$RUN_INTEGRATION_TESTS" = false ] && [ "$VISUALIZE_RESULTS" = false ]; then
		show_usage
	fi

	if [ "$CLEAR_OLD_OUTPUTS" = true ]; then
		clear
	fi

	if [ "$RUN_INTEGRATION_TESTS" = true ]; then
		integration_tests
	fi

	if [ "$VISUALIZE_RESULTS" = true ]; then
		visualize_results
	fi
}

main "$@"
