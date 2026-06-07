#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import time
import zlib

MAGIC_HEADER_MAGIC = 0x4D414749  # "MAGI"
DATA_TYPE_FIRMWARE = 0  
BL_VERSION_MAJOR = 1    
BL_VERSION_MINOR = 0   
BL_VERSION_PATCH = 0   
BL_VERSION_EXTRA = "alpha"

PARTITION_MAP = {
    0: {"this_addr": 0x08010000, "data_addr": 0x08010100, "suffix": "_app0"},
    1: {"this_addr": 0x08050000, "data_addr": 0x08050100, "suffix": "_app1"},
}

def main():
    project_root = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))

    if len(sys.argv) < 3:
        print("Usage: gen_magic_header.py <input_bin> <partition:0|1>")
        print("  partition 0: APP0 (0x08010000)")
        print("  partition 1: APP1 (0x08050000)")
        sys.exit(1)

    binfile = os.path.realpath(sys.argv[1])
    if not os.path.isfile(binfile):
        print(f"Error: File '{binfile}' does not exist.")
        sys.exit(1)

    partition = int(sys.argv[2])
    if partition not in PARTITION_MAP:
        print("Error: partition must be 0 or 1")
        sys.exit(1)

    this_addr = PARTITION_MAP[partition]["this_addr"]
    data_addr = PARTITION_MAP[partition]["data_addr"]
    suffix = PARTITION_MAP[partition]["suffix"]

    with open(binfile, "rb") as f:
        bin_data = f.read()
    bin_crc = zlib.crc32(bin_data) & 0xFFFFFFFF

    header = []
    header.append((MAGIC_HEADER_MAGIC).to_bytes(4, byteorder='little'))
    header.append((0).to_bytes(4, byteorder='little'))
    header.append((0).to_bytes(4 * 6, byteorder='little'))

    header.append((DATA_TYPE_FIRMWARE).to_bytes(4, byteorder='little'))
    header.append((256).to_bytes(4, byteorder='little'))
    header.append((data_addr).to_bytes(4, byteorder='little'))
    header.append((len(bin_data)).to_bytes(4, byteorder='little'))
    header.append((bin_crc).to_bytes(4, byteorder='little'))
    header.append((0).to_bytes(4 * 11, byteorder='little'))

    version_date = time.strftime("%y%m%d", time.localtime())
    version_time = time.strftime("%H%M", time.localtime())
    version_str = f"v{BL_VERSION_MAJOR}.{BL_VERSION_MINOR}.{BL_VERSION_PATCH}-{version_date}-{version_time}-{BL_VERSION_EXTRA}"
    version_bytes = version_str.encode('ascii')
    version_bytes = version_bytes.ljust(128, b'\x00')
    header.append(version_bytes)

    header.append((0).to_bytes(4 * 6, byteorder='little'))
    header.append((this_addr).to_bytes(4, byteorder='little'))

    this_crc32 = zlib.crc32(b''.join(header)) & 0xFFFFFFFF
    header.append((this_crc32).to_bytes(4, byteorder='little'))

    magic_header = b''.join(header)
    magic_header = magic_header.ljust(256, b'\x00')

    print(version_str)

    base_name = os.path.splitext(os.path.basename(binfile))[0]
    upgrade_filename = base_name + "_upgrade.xbin"
    output_file = os.path.join(project_root, "generated", upgrade_filename)
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, "wb") as f:
        f.write(b''.join([magic_header, bin_data]))

    print(f"Partition: APP{partition}")
    print(f"this_address: 0x{this_addr:08X}")
    print(f"data_address: 0x{data_addr:08X}")
    print(f"Generated file: {output_file}")

if __name__ == "__main__":
    main()