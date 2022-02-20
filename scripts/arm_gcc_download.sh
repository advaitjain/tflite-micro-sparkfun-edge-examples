#!/bin/bash
# Copyright 2021 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
#
# Called with following arguments:
# 1 - Path to the downloads folder which is typically
#     tensorflow/lite/micro/tools/make/downloads
#
# This script is called from the Makefile and uses the following convention to
# enable determination of sucess/failure:
#
#   - If the script is successful, the only output on stdout should be SUCCESS.
#     The makefile checks for this particular string.
#
#   - Any string on stdout that is not SUCCESS will be shown in the makefile as
#     the cause for the script to have failed.
#
#   - Any other informational prints should be on stderr.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR=${SCRIPT_DIR}/../
cd "${ROOT_DIR}"

DOWNLOADS_DIR=${ROOT_DIR}/downloads
if [ ! -d ${DOWNLOADS_DIR} ]; then
  echo "The top-level downloads directory: ${DOWNLOADS_DIR} does not exist."
  exit 1
fi

DOWNLOADED_GCC_PATH=${DOWNLOADS_DIR}/gcc_embedded

if [ -d ${DOWNLOADED_GCC_PATH} ]; then
  echo >&2 "${DOWNLOADED_GCC_PATH} already exists, skipping the download."
else

	UNAME_S=$(uname -s)
	if [ ${UNAME_S} == "Linux" ]; then
		GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2"
	elif [ ${UNAME_S} == "Darwin" ]; then
		GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-mac.tar.bz2"
	else
		echo >&2 "invalid host architecture ${UNAME_S}"
	fi

  TEMPFILE=$(mktemp -d)/temp_file
  # TODO(b/200052821): remove no-check-certificate once it is no longer
  # required.
  wget ${GCC_URL} --no-check-certificate -O ${TEMPFILE} >&2

  mkdir ${DOWNLOADED_GCC_PATH}
  tar -C ${DOWNLOADED_GCC_PATH} --strip-components=1 -xjf ${TEMPFILE} >&2
  echo >&2 "Unpacked to directory: ${DOWNLOADED_GCC_PATH}"
fi

echo "SUCCESS"
