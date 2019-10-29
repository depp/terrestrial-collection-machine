"""Pack GLSL shaders into C files for inclusion into a program.

Usage: pack_shaders --out-c=<file.c> --out-h=<file.h> shader...

Each shader file will be emitted as a const char array. The name of the array is
generated from the name of the shader file, without the directory, and with
non-alphanumeric sequences converted to underscores.

So if there's a single input file named "shaders/triangle.vert", the resulting
header file will declare an array:

    extern const char TRIANGLE_VERT[N];

Here, N is the actual size of the shader text, so you can use sizeof().
"""
import argparse
import os
import re
import sys

NON_ALPHANUM = re.compile('(?:[^A-Za-z0-9]+)+')
NEEDS_ESCAPE = re.compile(rb'[^ -~]|[\\"]')

ESCAPES = {
    b'\n': b'\\n',
    b'\t': b'\\t',
    b'\\': b'\\\\',
    b'"': b'\\"',
}
def escape(m):
    c = m.group(0)
    return ESCAPES.get(c) or b'\\x{:02x}'.format(c[0])

HEADER = '// This file is automatically generated.\n'

class Shader:
    def __init__(self, path):
        self.name = NON_ALPHANUM.sub('_', os.path.basename(path)).upper()
        with open(path, 'rb') as fp:
            self.text = fp.read()
    def write_c(self, fp):
        fp.write('const char {}[{}] =\n"'.format(self.name, len(self.text)))
        fp.write(NEEDS_ESCAPE.sub(escape, self.text).decode('ASCII'))
        fp.write('";\n')
    def write_h(self, fp):
        fp.write('extern const char {}[{}];\n'
                 .format(self.name, len(self.text)))

def main():
    p = argparse.ArgumentParser('pack_shaders')
    p.add_argument('--out-c', help='Output C file', required=True)
    p.add_argument('--out-h', help='Output H file', required=True)
    p.add_argument('shader', help='Input shader files', nargs='+')
    args = p.parse_args()

    shaders = [Shader(path) for path in sorted(args.shader)]
    with open(args.out_c, 'w') as fp:
        fp.write(HEADER)
        fp.write('#include "tcm/packed_shaders.h"\n')
        for shader in shaders:
            shader.write_c(fp)
    with open(args.out_h, 'w') as fp:
        fp.write(HEADER)
        for shader in shaders:
            shader.write_h(fp)

if __name__ == '__main__':
    main()
