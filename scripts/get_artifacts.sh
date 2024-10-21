#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

###
# Downloads & Extracts all GH workflow artifacts to `build/artifacts/` using `gh`:
#   $ ./scripts/get_artifacts.sh
#
# @version 0.1.0
# @author Bradley Whited
###

ARTIC_DIR='build/artifacts'

get_artifact() {
  gh run download --dir "${ARTIC_DIR}/" --name "$1"
}

set -x
mkdir -pv "${ARTIC_DIR}"

# Download each one separately so that it won't create a sub dir.
get_artifact 'linux-appimage-x64'
get_artifact 'macos-uni'
get_artifact 'windows-x64'
