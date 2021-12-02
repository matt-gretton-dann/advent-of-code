#!/bin/sh
set -eu

# Find the source directory
cwd="$(pwd)"
cd "$(dirname "$0")"
srcdir="$(pwd)"
cd "${cwd}"

# Parse command line options
if [ $# -ne 4 ]; then
    echo "Usage: $0 <year> <day> <puzzle> <input-file>" >&2
    exit 1
fi

# Parse and validate command line options"
year="$1"
day="$2"
num="$3"
input="$4"

if [ ! -d "${srcdir}/${year}" ]; then
  echo "Year of puzzles ${year} does not exist" >&2
  exit 2
fi

if [ ${#day} -eq 1 ]; then
    day="0${day}"
fi

if [ ${#num} -eq 1 ]; then
    num="0${num}"
fi

puzzle_src="${srcdir}/${year}/puzzle-${day}-${num}.cc"
puzzle="puzzle-${year}-${day}-${num}"
exe="build/puzzle-${year}-${day}-${num}"

if [ ! -e "${puzzle_src}" ]; then
    echo "Unable to find puzzle sources for ${year}-12-${day} number ${num}" >&2
    exit 2
fi

# Rebuild the executable we want to run.
if [ ! -d "build" ]; then
    opts=
    if [ "$(uname -s)" = "Darwin" ]; then
      if [ "$(uname -m)" = "arm64" ]; then
        opts="-DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl"
      else
        opts="-DOPENSSL_ROOT_DIR=/usr/local/opt/openssl"
      fi
    fi

    cmake -Bbuild -S "${srcdir}" ${opts}
fi
cmake --build build --target "${puzzle}"

# And run the executable - using the driver if necessary.
driver="${srcdir}/${year}/driver-${day}-${num}.sh"
if [ -e "${driver}" ]; then
    sh "${driver}" "${exe}" "${input}"
else
    "${exe}" <"${input}"
fi
