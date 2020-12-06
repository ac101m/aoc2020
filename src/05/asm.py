#!/usr/bin/env python3
'''
The assembler for my jank asf CPU.
It takes in an assembly file and outputs a file that can be
loaded into a rom in Logisim. If for some reason beyond
comprehension you actually want to try using this, then consult
the accompanying readme.

Usage:
    asm.py [options] <input-file>

Options:
    -h --help           Display this help message.
    -o --output <file>  Path to output assembled code to.
'''


import docopt


# Instruction formats
instruction_formats = {
    "op only": {
        "fields": [
            ("OPCODE", 0, 7)
        ]
    },
    "op 16bimm reg": {
        "fields": [
            ("OPCODE", 0, 7),
            ("IMMEDIATE", 8, 23),
            ("REG_ADDR", 28, 31)
        ]
    },
    "op reg nothing reg": {
        "fields": [
            ("OPCODE", 0, 7),
            ("REG_ADDR", 12, 15),
            ("REG_ADDR", 28, 31)
        ]
    },
    "op reg reg reg": {
        "fields": [
            ("OPCODE", 0, 7),
            ("REG_ADDR", 12, 15),
            ("REG_ADDR", 20, 23),
            ("REG_ADDR", 28, 31)
        ]
    },
    "op reg 8bimm reg": {
        "fields": [
            ("OPCODE", 0, 7),
            ("REG_ADDR", 12, 15),
            ("IMMEDIATE", 16, 23),
            ("REG_ADDR", 28, 31)
        ]
    },
    "op reg reg nothing": {
        "fields": {
            ("OPCODE", 0, 7),
            ("REG_ADDR", 12, 15),
            ("REG_ADDR", 20, 23)
        }
    },
    "op 24bimm": {
        "fields": [
            ("OPCODE", 0, 7),
            ("IMMEDIATE", 8, 31)
        ]
    },
    "op nothing reg nothing": {
        "fields": {
            ("OPCODE", 0, 7),
            ("REG_ADDR", 20, 23)
        }
    }
}


instruction_definitions = {
    "HLT": {
        "id": 0x00,
        "format": "op only",
        "relative_immediates": False,
        "description": "Halts the processor on the current instruction."
    },
    "MOVI": {
        "id": 0x01,
        "format": "op 16bimm reg",
        "relative_immediates": False,
        "description": "Move a 16 bit immediate into the low bytes of a register and sign extend."
    },
    "MOVIL": {
        "id": 0x02,
        "format": "op 16bimm reg",
        "relative_immediates": False,
        "description": "Move a 16 bit immediate into the low bytes of a register."
    },
    "MOVIH": {
        "id": 0x03,
        "format": "op 16bimm reg",
        "relative_immediates": False,
        "description": "Move a 16 bit immediate into the high bytes of a register."
    },
    "MOV": {
        "4": 0x04,
        "format": "op reg nothing reg",
        "relative_immediates": False,
        "description": "Copy the contents of one register to another."
    },
    "ADD": {
        "id": 0x05,
        "format": "op reg reg reg",
        "relative_immediates": False,
        "description": "Add first and second register, store result in third register."
    },
    "ADDI": {
        "id": 0x06,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Add an immediate value to a register and store the result in another register."
    },
    "ADDC": None,
    "SUB": {
        "id": 0x08,
        "format": "op reg reg reg",
        "relative_immediates": False,
        "description": "Subtract second register from first register and store in third register."
    },
    "SUBI": {
        "id": 0x09,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Subtract an immediate value from a register and store the result in another register."
    },
    "SUBC": None,
    "AND": None,
    "OR": None,
    "XOR": None,
    "NOT": None,
    "LSL": None,
    "LSR": None,
    "ROL": None,
    "ROR": None,
    "ASR": None,
    "CMP": {
        "id": 0x14,
        "format": "op reg reg nothing",
        "relative_immediates": False,
        "description": "Compare two registers and set the status register without storing the result."
    },
    "JMP": {
        "id": 0x15,
        "format": "op 24bimm",
        "relative_immediates": True,
        "description": "Unconditional PC relative jump based on sign extended 24 bit immediate."
    },
    "JMPZ": {
        "id": 0x16,
        "format": "op 24bimm",
        "relative_immediates": True,
        "description": "PC relative jump based on sign extended 24 bit immediate if zero flag is set."
    },
    "JMPNZ": {
        "id": 0x17,
        "format": "op 24bimm",
        "relative_immediates": True,
        "description": "PC relative jump based on sign extended 24 bit immediate if zero flag is not set."
    },
    "JMPLZ": None,
    "JMPGZ": None,
    "JMPC": None,
    "JMPR": {
        "id": None,
        "format": "op nothing reg nothing",
        "relative_immediates": False,
        "description": "Unconditional non-relative jump to the memory address stored in the register."
    },
    "LD32": {
        "id": 0x20,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Fetch the 32b word located at the address in the first register offset by an 8 bit sign extended immediate and store it in the second register."
    },
    "LD16": {
        "id": None,
    },
    "LD16S": {
        "id": None,
    },
    "LD8": {
        "id": 0x23,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Fetch the 8b word located at the address in the first register offset by an 8 bit sign extended immediate and store it in the second register."
    },
    "LD8S": {
        "id": 0x24,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Fetch the sign extended 8b word located at the address in the first register offset by an 8 bit sign extended immediate and store it in the second register."
    },
    "ST32": {
        "id": 0x25,
        "format": "op reg 8bimm reg",
        "relative_immediates": False,
        "description": "Store the value of the last register at the address in the first register offset by an 8 bit sign extended immediate."
    },
    "ST16": {
        "id": None,
    },
    "ST8": {
        "id": None,
    },
    "CALL": {
        "id": 0x30,
        "format": "op 24bimm",
        "relative_immediates": True,
        "description": "Pushes r5-14 onto the stack, sets LR to the current PC value + 1, then performs a relative jump by 24b sign extended immediate."
    },
    "CALLR": {
        "id": None,
        "format": "op nothing reg nothing",
        "relative_immediates": False,
        "description": "Pushes r5-14 onto the stack, sets LR to the current PC value, then performs a direct jump to the location stored in a register."
    },
    "RET": {
        "id": 0x32,
        "format": "op only",
        "relative_immediates": False,
        "description": "Jumps to the location stored in the link register, then pops r5-14 from the stack."
    }
}


registers = {
    "R0": 0,
    "R1": 1,
    "R2": 2,
    "R3": 3,
    "R4": 4,
    "R5": 5,
    "R6": 6,
    "R7": 7,
    "R8": 8,
    "R9": 9,
    "R10": 10,
    "R11": 11,
    "R12": 12,
    "R13": 13,
    "R14": 14,
    "R15": 15,
    "LR": 14,
    "SP": 15
}


def InstructionValid(instruction):
    if instruction is None:
        return "Error, no definition"

    id = instruction.get("id")

    if id is None:
        return "Error, missing id field"
    if not isinstance(id, int):
        return "Error, id not integer"
    if not id < 256 and id >= 0:
        return "Error, id out of range"

    format = instruction.get("format")

    if format is None:
        return "Error, missing format field"
    if not isinstance(format, str):
        return "Error, format is not a string"
    if instruction_formats.get(format) is None:
        return "Error, invalid isntruction format '%s'" % (format)

    description = instruction.get("description")

    if description is None:
        return "Error, missing decription"
    if not isinstance(description, str):
        return "Error, description is not a string"

    relative_immediates = instruction.get("relative_immediates")

    if relative_immediates is None:
        return "Error, missing relative immediates specification"
    if not isinstance(relative_immediates, bool):
        return "Error, relative_immediates is not a boolean"

    return "ok"


def EnumerateInstructions():
    instructions = {}

    for mnemonic, instruction in instruction_definitions.items():
        result = InstructionValid(instruction)

        print("%s: \t%s" % (mnemonic, result))

        if result == "ok":
            instructions[mnemonic] = instruction

    return instructions


def InsertInstrField(instr, value, start, end):
    size = end - start + 1
    offset = 32 - size - start

    min_value = -2 ** (size - 1)
    max_value = (2 ** size) - 1

    if value < min_value or value > max_value:
        raise ValueError("Field value value out of range.")

    value_bitmask = (1 << size) - 1
    value &= value_bitmask

    instr |= value << offset;

    return instr


def ResolveImmediate(token, labels, current_position=0):
    try:
        if token.startswith("0x"):
            return int(token, 16)
        else:
            return int(token)
    except ValueError:
        pass

    value = labels.get(token)

    if value is None:
        raise ValueError("Error, no such label '%s'" % token)

    return value - current_position


def AssembleInstruction(line, labels, instr_index):
    tokens = line.split();

    opname = tokens[0]
    definition = available_instructions.get(opname)

    if definition is None:
        raise ValueError("Error, no such instruction '%s'" % opname)

    id = definition["id"]
    format = instruction_formats.get(definition["format"])
    relative_immediates = definition["relative_immediates"]
    description = definition["description"]

    if format is None:
        raise ValueError("No such instruction format '%s'" % format)

    fields = format["fields"]
    field_values = []

    if len(fields) != len(tokens):
        raise ValueError("Incorrect number of tokens. %s expects %d tokens." % (opname, len(fields)))

    instr = 0

    for token, field in zip(tokens, fields):
        field_type = field[0]
        field_start = field[1]
        field_end = field[2]

        if field_type == "OPCODE":
            instr = InsertInstrField(instr, id, field_start, field_end)

        elif field_type == "IMMEDIATE":
            immediate_relative_to = instr_index if relative_immediates else 0
            immediate_value = ResolveImmediate(token, labels, immediate_relative_to)
            instr = InsertInstrField(instr, immediate_value, field_start, field_end)

        elif field_type == "REG_ADDR":
            register_index = registers.get(token.upper())

            if register_index is None:
                raise ValueError("No such register '%s'" % token)

            instr = InsertInstrField(instr, register_index, field_start, field_end)

        else:
            raise ValueError("Unrecognised field type '%s'" % field_type)

    return instr;


def main(args):

    print("[INSTRUCTION ENUMERATION PASS]:")
    global available_instructions
    available_instructions = EnumerateInstructions()

    labels = {}
    aliases = {}

    input_filename = args["<input-file>"]
    output_filename = input_filename.split('.')[0] if args["--output"] is None else args["--output"]

    # First pass finds all label locations
    print("\n[LABEL IDENTIFICATION]:")
    try:
        i = 0
        line_number = 0;

        with open(input_filename) as f:
            for line in f:
                line = line.strip()
                line_number += 1

                # Skip empty lines
                if line == "":
                    continue

                # Skip comments
                if line.startswith(";"):
                    continue

                # Colons indicate a label
                if line.endswith(":"):
                    label_key = line.strip(":").strip()

                    if labels.get(label_key) is None:
                        labels[label_key] = i
                    else:
                        raise ValueError("Duplicate label: ")

                    continue

                # Must be an instruction then!
                i += 1

    except ValueError as e:
        print("Error: %s" % e)
        print("[line %d]:\t%s" % (line_number, line))
        exit(1)

    print("Located %d labels" % len(labels))

    print("\n[LABEL INDICES]:")
    for label, index in labels.items():
        print("%s:\t%d" % (label, index))

    # Second pass assembles instructions
    print("\n[ASSEMBLY PASS]:")
    try:
        i = 0
        line_number = 0;
        program = []

        with open(input_filename) as f:
            for line in f:
                line = line.strip()
                line_number += 1

                # Skip empty lines
                if line == "":
                    continue

                # Skip comments
                if line.startswith(";"):
                    continue

                # Skip labels (already found)
                if line.endswith(":"):
                    continue

                # Must be an instruction then!
                program.append(AssembleInstruction(line, labels, i))
                i += 1

    except ValueError as e:
        print("Error: %s" % e)
        print("[line %d]:\t%s" % (line_number, line))
        exit(1)

    print("Generated %d instructions" % len(program))

    with open(output_filename, "w") as f:
        f.write("v2.0 raw\n")
        for instruction in program:
            f.write(format(instruction, 'x'))
            f.write("\n")



if __name__ == '__main__':
    args = docopt.docopt(__doc__)
    main(args)
