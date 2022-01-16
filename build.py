#!usr/bin/env python3

import os
from typing import Sequence


def collection_src_files(path: str) -> Sequence[str]:
    return list(filter(lambda s: s.endswith(
        ".c") or s.endswith(".cpp"), [os.path.join(path, f) for f in os.listdir(path)]))


def gen_build_cmd(compiler: str, output_file: str, src_files: Sequence[str], inc_paths: Sequence[str], defs: Sequence[str], link_libs: Sequence[str], cpp_std: str) -> str:
    return f'{compiler} -o {output_file} {" ".join(src_files)} {" ".join([f"-I {p}" for p in inc_paths])} {" ".join([f"-D{d}" for d in defs])} {" ".join([f"-l{l}" for l in link_libs])} -std={cpp_std}'


def main():
    lua_files = collection_src_files("libs/lua")
    proj_files = collection_src_files(".")

    cmd = gen_build_cmd("g++", "out",
                        lua_files + proj_files,
                        ["./libs/lua", "./libs/olcPixelGameEngine"],
                        ["FORCE_EXPERIMENTAL_FS"], ["X11", "GL", "pthread", "png", "stdc++fs"], "c++17")
    print(cmd)
    os.system(cmd)


if __name__ == "__main__":
    main()
