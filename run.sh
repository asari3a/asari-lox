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

assert "" "12"
assert "() {}" "0 1 2 3 12"
assert "() {} !=" "0 1 2 3 11 12"
assert "() {} !" "0 1 2 3 10 12"