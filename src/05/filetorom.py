#!/usr/bin/env python3
'''
Quick and dirty tool for converting arbitrary files into logisim hex files
In this case, files are generated for a Logisim memory with 32 bit roms.
This could be generalized, but I don't want to.

Usage:
    asm.py [options] <input-file>

Options:
    -h --help           Display this help message.
    -o --output <file>  Path to output encoded file to.
'''


import docopt


def main(args):
    input_path = args["<input-file>"]
    output_path = input_filename.split('.')[0] if args["--output"] is None else args["--output"]

    with open(input_path) as f_in:
        with open(output_path, "w") as f_out:
            f_out.write("v2.0 raw\n")

            word = 0
            word_offset = 0;

            while True:
                c = f_in.read(1)

                if word_offset == 32:
                    f_out.write(format(word, "x"));
                    f_out.write("\n")
                    word_offset = 0
                    word = 0

                if not c:
                    if word_offset != 32:
                        f_out.write(format(word, "x"));
                        f_out.write("\n")
                    break

                word |= (ord(c) & 0xff) << word_offset;
                word_offset += 8


if __name__ == '__main__':
    args = docopt.docopt(__doc__)
    main(args)
