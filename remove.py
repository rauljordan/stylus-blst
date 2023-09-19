"""
Removes the DataCount section of a WASM file for deployment to Arbitrum Stylus
"""
import re

def parse_sections(wasm_bytes):
    """Parse the sections of a WASM binary and return them as a list."""
    sections = []
    idx = 8  # Skip the WASM header (magic number and version)
    
    while idx < len(wasm_bytes):
        section_id = wasm_bytes[idx]
        idx += 1
        
        # Decode the length of the section (LEB128 format)
        section_length, consumed = decode_leb128(wasm_bytes[idx:])
        idx += consumed
        
        section_data = wasm_bytes[idx:idx+section_length]
        sections.append((section_id, section_data))
        idx += section_length
        
    return sections

def decode_leb128(data):
    """Decode a number in LEB128 format and return the number and the number of bytes consumed."""
    result = 0
    shift = 0
    consumed = 0
    byte = 0x80  # Just to enter the loop
    
    while byte & 0x80:
        byte = data[consumed]
        result |= (byte & 0x7F) << shift
        shift += 7
        consumed += 1
        
    return result, consumed

def remove_section(wasm_bytes, section_name):
    """Remove a section from a WASM binary."""
    section_ids = {
        "Custom": 0,
        "Type": 1,
        "Import": 2,
        "Function": 3,
        "Table": 4,
        "Memory": 5,
        "Global": 6,
        "Export": 7,
        "Start": 8,
        "Element": 9,
        "Code": 10,
        "Data": 11,
        "DataCount": 12
    }
    
    sections = parse_sections(wasm_bytes)
    filtered_sections = [(id, data) for id, data in sections if id != section_ids[section_name]]
    
    # Reconstruct the WASM binary without the specified section
    new_wasm_bytes = wasm_bytes[:8]  # WASM header (magic number and version)
    for id, data in filtered_sections:
        new_wasm_bytes += bytes([id])  # Section id
        length = len(data)
        new_wasm_bytes += encode_leb128(length) + data
        
    return new_wasm_bytes

def encode_leb128(value):
    """Encode a number in LEB128 format."""
    bytes_ = []
    while True:
        byte = value & 0x7F
        value >>= 7
        if value == 0 and (byte & 0x40) == 0:
            bytes_.append(byte)
            break
        elif value == -1 and (byte & 0x40) != 0:
            bytes_.append(byte)
            break
        else:
            bytes_.append(byte | 0x80)
            value &= (1 << 32) - 1
    return bytes(bytearray(bytes_))

# Read the WASM binary
with open("bls.wasm", "rb") as f:
    wasm_data = f.read()

# Remove the DataCount section
modified_wasm_data = remove_section(wasm_data, "DataCount")

# Save the modified WASM binary
with open("bls.wasm", "wb") as f:
    f.write(modified_wasm_data)
