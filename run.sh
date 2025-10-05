#!/bin/bash

assert() {
    input=$1
    expected=$2

    actual=$(./asari-lox "$input")

    if [ "$actual" = "$expected" ]; then
        echo "$input => $expected"
    else
        echo "$input => $actual"
        exit 1
    fi
}

assert "" "-1"
assert "(){}" "0 1 2 3 -1"
assert "!=" "13 -1"
assert "!" "12 -1"
assert "=" "10 -1"
assert "==" "11 -1"
assert "<" "14 -1"
assert "<=" "15 -1"
assert ">" "16 -1"
assert ">=" "17 -1"

echo "Ok"