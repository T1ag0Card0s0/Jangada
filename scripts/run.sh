#!/bin/bash

qemu-system-aarch64 -machine virt -cpu cortex-a53 -m 128M -nographic -kernel build/jangada
