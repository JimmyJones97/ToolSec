# coding: utf-8

elf_file = "D:\\Discovery\\pubgmhd\\v1.14.10\\libUE4.so"

version = "v1.14.10"

def replace(orig_so, piece, piece_start):
    orig_bytes = open(orig_so,"rb").read()
    out_bytes = bytearray(orig_bytes)
    
    piece_bytes = open(piece, "rb").read()
    for i,b in enumerate(piece_bytes):
        out_bytes[piece_start+i] = b
    
    with open("out.so", "wb") as outso:
        outso.write(out_bytes)

"""
{
    "bin_file": "libUE4_so", # binary file path
    "off": 0xdeadbeaf, # "offset where start to replace"
}
"""
def replace2(orig_so, replaced_bin):
    """
    @param orig_so: str, original elf/so file path
    @param replaced_bin: list, replaced bin file info
    """
    orig_bytes = open(orig_so, "rb").read()
    out_bytes = bytearray(orig_bytes)

    for bin in replaced_bin:
        bin_bytes = open(bin["bin_file"], "rb").read()
        for i,b in enumerate(bin_bytes):
            out_bytes[bin["off"]+i] = b

    with open("out.so", "wb") as fp:
        fp.write(out_bytes)

if __name__ == "__main__":
    replace_list = [
        {
            "bin_file": "D:\\Discovery\\pubgmhd\\v1.14.10\\libUE4_text",
            "off": 0x00d1c360,
        },
        {
            "bin_file": "D:\\Discovery\\pubgmhd\\v1.14.10\\libUE4_rodata",
            "off": 0x05ac24f0,
        },
        ]
    
    replace2(elf_file, replace_list)