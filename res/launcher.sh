#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

cd "$(dirname "$0")"

EKO_ARCH="$(uname -m)"
EKO_BIN_ARCH='x86_64' # Default if arch not recognized.

# TODO: If add another arch in the future, uncomment this.
#case "$EKO_ARCH" in
#'x86_64'|'x64')
#  EKO_BIN_ARCH='x86_64'
#  ;;
#'i386'|'i686'|'x86')
#  EKO_BIN_ARCH='i386'
#  ;;
#'aarch64'|'arm64')
#  EKO_BIN_ARCH='aarch64'
#  ;;
#'armhf'|'armv7l'|'arm')
#  EKO_BIN_ARCH='armhf'
#  ;;
#*)
#  echo "[WARN] Unknown arch [${EKO_ARCH}]; falling back to arch [${EKO_BIN_ARCH}]."
#  ;;
#esac

EKO_BIN="./@BIN_NAME@-${EKO_BIN_ARCH}.AppImage"

echo "Arch:     '${EKO_ARCH}'"
echo "Bin arch: '${EKO_BIN_ARCH}'"
echo "Bin:      '${EKO_BIN}'"
echo ''

exec "$EKO_BIN" "$@"
