#!/bin/bash

# Builds the Bluetooth firmware for all supported boards.
#
# Needs west and the Zephyr SDK, for example from the Zephyr CI Docker image
# (start from the top level of the repository):
#
#   docker run --rm -v $(pwd):/workdir/project -w /workdir/project/firmware-bluetooth \
#       ghcr.io/zephyrproject-rtos/ci:v0.29.2 ./build.sh
#
# Each build names the nRF Connect SDK version it uses, so that boards whose
# chips get dropped from newer SDK releases can stay on an older one. Each SDK
# version is checked out under sdk/ the first time it's needed.
#
# Pass build names as arguments to only run those builds.

set -x -e -u -o pipefail

cd "$(dirname "$0")"

SDK_PATH_PREFIX=${SDK_PATH_PREFIX:-./sdk}
BUILD_PATH_PREFIX=${BUILD_PATH_PREFIX:-./build}
ARTIFACTS_PATH=${ARTIFACTS_PATH:-./artifacts}

mkdir -p "${SDK_PATH_PREFIX}" "${BUILD_PATH_PREFIX}" "${ARTIFACTS_PATH}"

setup_ncs() {
    local version=$1
    local sdk="${SDK_PATH_PREFIX}/ncs-${version}"

    if [ -d "${sdk}/.west" ]; then
        return
    fi

    mkdir -p "${sdk}/manifest"
    cat > "${sdk}/manifest/west.yml" <<EOF
manifest:
  remotes:
    - name: ncs
      url-base: https://github.com/nrfconnect
  projects:
    - name: nrf
      remote: ncs
      repo-path: sdk-nrf
      revision: ${version}
      import: true
EOF
    west init -l "${sdk}/manifest"
    (cd "${sdk}" && west update -o=--depth=1 -n)
}

# build <name> <ncs version> <board> [extra CMake arguments...]
build() {
    local name=$1
    local version=$2
    local board=$3
    shift 3

    if [ -n "${SELECTED_BUILDS}" ] && [[ " ${SELECTED_BUILDS} " != *" ${name} "* ]]; then
        return
    fi

    setup_ncs "${version}"

    local build_dir="${BUILD_PATH_PREFIX}/${name}"
    ZEPHYR_BASE=$(realpath "${SDK_PATH_PREFIX}/ncs-${version}/zephyr") \
        west build -p -d "${build_dir}" -b "${board}" . -- "$@"

    # With sysbuild, the application's own build lives in a subdirectory
    # named after the source directory.
    local app_build_dir="${build_dir}/$(basename "${PWD}")"
    if [ -f "${app_build_dir}/zephyr/remapper.uf2" ]; then
        cp "${app_build_dir}/zephyr/remapper.uf2" "${ARTIFACTS_PATH}/remapper_${name}.uf2"
    else
        cp "${app_build_dir}/zephyr/remapper.hex" "${ARTIFACTS_PATH}/remapper_${name}.hex"
    fi
}

SELECTED_BUILDS="$*"

build adafruit_feather_nrf52840 v3.4.1 adafruit_feather_nrf52840/nrf52840/uf2
build seeed_xiao_nrf52840 v3.4.1 xiao_ble
build nrf54lm20dk v3.4.1 nrf54lm20dk/nrf54lm20a/cpuapp
