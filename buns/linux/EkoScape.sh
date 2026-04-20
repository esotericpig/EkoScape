#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

cd "$(dirname "$0")"

eko_exe_arch='x86_64' # Default if arch not recognized.

sys_arch="$(uname -m)"
sys_arch="$(printf '%s' "${sys_arch}" | tr '[:upper:]' '[:lower:]')" # Lower case.

# TODO: If add another arch in the future, uncomment this.
#case "${sys_arch}" in
#*'x86_64'*|*'x64'*|*'amd64'*)
#  eko_exe_arch='x86_64'
#  ;;
#*'86'*) # x86|(i386|i486|i586|i686)
#  eko_exe_arch='x86'
#  ;;
#*'arm64'*|*'aarch64'*)
#  eko_exe_arch='arm64'
#  ;;
#*'arm'*|*'aarch'*) # (arm|armhf|armv6l|armv7l)|aarch
#  eko_exe_arch='arm'
#  ;;
#*)
#  echo "[WARN] Unknown arch [${sys_arch}]; falling back to arch [${eko_exe_arch}]."
#  echo ''
#  ;;
#esac

eko_exe="./EkoScape-${eko_exe_arch}.AppImage"

echo "Dir:      '${PWD}'"
echo "Sys arch: '${sys_arch}'"
echo "Exe arch: '${eko_exe_arch}'"
echo "Exe:      '${eko_exe}'"
echo ''

set -x
exec "${eko_exe}" "$@"
