#! /usr/bin/env bash

for I in plot*.C; do
    root -b -q -l $I
done
