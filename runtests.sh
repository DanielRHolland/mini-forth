#!/bin/sh

if [ -z "$FORTH_CMD" ] ; then
    FORTH_CMD="./forth"
fi

dir='./tests'
if [ "$#" -gt 0 ] ; then
    dir="$1"
fi

n=0
failed=0

runtest () {
    file="$1"

    output="$(mktemp)"
    expectedoutput="$(mktemp)"
    sed '0,/^INPUT$/d;/^OUTPUT$/,$d' "$file" | "$FORTH_CMD" > "$output" &2> /dev/null
    sed '0,/^OUTPUT$/d' "$file" > "$expectedoutput"

    diff="$(diff -w -y "$output" "$expectedoutput")"

    if [ "1" -eq "$?" ] ; then
        echo "Test $n failed: $file" >&2
        echo "Diff: (actual vs expected)" >&2
        echo "$diff"
        exitcode=1
        failed="$(expr "$failed" + 1)"

    fi
}

for f in $dir/*.forth; do
    n="$(expr "$n" + 1)"
    runtest "$f"
done

for f in $dir/*/*.forth; do
    n="$(expr "$n" + 1)"
    runtest "$f"
done

if [ "0" -eq "$failed" ]; then
    echo "All $n tests passed" >&2
    exit 0
else
    echo "$failed/$n tests failed" >&2
    exit 1
fi
