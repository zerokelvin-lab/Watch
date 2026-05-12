#!/usr/bin/env python3
"""IL bytecode decoder for .NET assembly decompilation"""

import subprocess
import json
import struct

# IL Opcode table (most common opcodes)
OPCODES = {
    0x00: ("nop", 0),
    0x01: ("break", 0),
    0x02: ("ldarg.0", 0),
    0x03: ("ldarg.1", 0),
    0x04: ("ldarg.2", 0),
    0x05: ("ldarg.3", 0),
    0x06: ("ldloc.0", 0),
    0x07: ("ldloc.1", 0),
    0x08: ("ldloc.2", 0),
    0x09: ("ldloc.3", 0),
    0x0A: ("stloc.0", 0),
    0x0B: ("stloc.1", 0),
    0x0C: ("stloc.2", 0),
    0x0D: ("stloc.3", 0),
    0x0E: ("ldarg.s", 1),
    0x0F: ("ldarga.s", 1),
    0x10: ("starg.s", 1),
    0x11: ("ldloc.s", 1),
    0x12: ("ldloca.s", 1),
    0x13: ("stloc.s", 1),
    0x14: ("ldnull", 0),
    0x15: ("ldc.i4.m1", 0),
    0x16: ("ldc.i4.0", 0),
    0x17: ("ldc.i4.1", 0),
    0x18: ("ldc.i4.2", 0),
    0x19: ("ldc.i4.3", 0),
    0x1A: ("ldc.i4.4", 0),
    0x1B: ("ldc.i4.5", 0),
    0x1C: ("ldc.i4.6", 0),
    0x1D: ("ldc.i4.7", 0),
    0x1E: ("ldc.i4.8", 0),
    0x1F: ("ldc.i4.s", 1),
    0x20: ("ldc.i4", 4),
    0x21: ("ldc.i8", 8),
    0x22: ("ldc.r4", 4),
    0x23: ("ldc.r8", 8),
    0x25: ("dup", 0),
    0x26: ("pop", 0),
    0x27: ("jmp", 4),
    0x28: ("call", 4),
    0x29: ("calli", 4),
    0x2A: ("ret", 0),
    0x2B: ("br.s", 1),
    0x2C: ("brfalse.s", 1),
    0x2D: ("brtrue.s", 1),
    0x2E: ("beq.s", 1),
    0x2F: ("bge.s", 1),
    0x30: ("bgt.s", 1),
    0x31: ("ble.s", 1),
    0x32: ("blt.s", 1),
    0x33: ("bne.un.s", 1),
    0x34: ("bge.un.s", 1),
    0x35: ("bgt.un.s", 1),
    0x36: ("ble.un.s", 1),
    0x37: ("blt.un.s", 1),
    0x38: ("br", 4),
    0x39: ("brfalse", 4),
    0x3A: ("brtrue", 4),
    0x3B: ("beq", 4),
    0x3C: ("bge", 4),
    0x3D: ("bgt", 4),
    0x3E: ("ble", 4),
    0x3F: ("blt", 4),
    0x40: ("bne.un", 4),
    0x41: ("bge.un", 4),
    0x42: ("bgt.un", 4),
    0x43: ("ble.un", 4),
    0x44: ("blt.un", 4),
    0x45: ("switch", 4),  # variable length
    0x46: ("ldind.i1", 0),
    0x47: ("ldind.u1", 0),
    0x48: ("ldind.i2", 0),
    0x49: ("ldind.u2", 0),
    0x4A: ("ldind.i4", 0),
    0x4B: ("ldind.u4", 0),
    0x4C: ("ldind.i8", 0),
    0x4D: ("ldind.r4", 0),
    0x4E: ("ldind.r8", 0),
    0x4F: ("ldind.ref", 0),
    0x50: ("stind.ref", 0),
    0x51: ("stind.i1", 0),
    0x52: ("stind.i2", 0),
    0x53: ("stind.i4", 0),
    0x54: ("stind.i8", 0),
    0x55: ("stind.r4", 0),
    0x56: ("stind.r8", 0),
    0x58: ("add", 0),
    0x59: ("sub", 0),
    0x5A: ("mul", 0),
    0x5B: ("div", 0),
    0x5C: ("div.un", 0),
    0x5D: ("rem", 0),
    0x5E: ("rem.un", 0),
    0x5F: ("and", 0),
    0x60: ("or", 0),
    0x61: ("xor", 0),
    0x62: ("shl", 0),
    0x63: ("shr", 0),
    0x64: ("shr.un", 0),
    0x65: ("neg", 0),
    0x66: ("not", 0),
    0x67: ("conv.i1", 0),
    0x68: ("conv.i2", 0),
    0x69: ("conv.i4", 0),
    0x6A: ("conv.i8", 0),
    0x6B: ("conv.r4", 0),
    0x6C: ("conv.r8", 0),
    0x6D: ("conv.u4", 0),
    0x6E: ("conv.u8", 0),
    0x6F: ("callvirt", 4),
    0x70: ("cpobj", 4),
    0x71: ("ldobj", 4),
    0x72: ("ldstr", 4),
    0x73: ("newobj", 4),
    0x74: ("castclass", 4),
    0x75: ("isinst", 4),
    0x76: ("conv.r.un", 0),
    0x7B: ("ldfld", 4),
    0x7C: ("ldfld", 4),  # sometimes 7C is also ldfld
    0x7D: ("stfld", 4),
    0x7E: ("ldsfld", 4),
    0x7F: ("stsfld", 4),
    0x80: ("stobj", 4),
    0x81: ("ldflda", 4),  # actually this might be different
    0x82: ("conv.ovf.i1.un", 0),
    0x83: ("conv.ovf.i2.un", 0),
    0x84: ("conv.ovf.i4.un", 0),
    0x85: ("conv.ovf.i8.un", 0),
    0x8A: ("stfld", 4),  # could be stfld variant
    0x8B: ("stfld", 4),  # could be stfld variant
    0x8D: ("newarr", 4),
    0x8E: ("ldlen", 0),
    0x8F: ("ldelema", 4),
    0x90: ("ldelem.i1", 0),
    0x91: ("ldelem.u1", 0),
    0x92: ("ldelem.i2", 0),
    0x93: ("ldelem.u2", 0),
    0x94: ("ldelem.i4", 0),
    0x95: ("ldelem.u4", 0),
    0x96: ("ldelem.i8", 0),
    0x97: ("ldelem.r4", 0),
    0x98: ("ldelem.r8", 0),
    0x99: ("ldelem.ref", 0),
    0x9A: ("stelem.i", 0),
    0x9B: ("stelem.i1", 0),
    0x9C: ("stelem.i2", 0),
    0x9D: ("stelem.i4", 0),
    0x9E: ("stelem.i8", 0),
    0x9F: ("stelem.r4", 0),
    0xA0: ("stelem.r8", 0),
    0xA1: ("stelem.ref", 0),
    0xA2: ("stelem", 4),
    0xA3: ("stelem", 4),
    0xA5: ("unbox", 4),
    0xB3: ("conv.ovf.i1", 0),
    0xB4: ("conv.ovf.u1", 0),
    0xB5: ("conv.ovf.i2", 0),
    0xB6: ("conv.ovf.u2", 0),
    0xB7: ("conv.ovf.i4", 0),
    0xB8: ("conv.ovf.u4", 0),
    0xB9: ("conv.ovf.i8", 0),
    0xBA: ("conv.ovf.u8", 0),
    0xC2: ("refanyval", 4),
    0xC3: ("ckfinite", 0),
    0xC6: ("mkrefany", 4),
    0xD0: ("ldtoken", 4),
    0xD1: ("conv.u2", 0),
    0xD2: ("conv.u1", 0),
    0xD3: ("conv.i", 0),
    0xD4: ("conv.ovf.i", 0),
    0xD5: ("conv.ovf.u", 0),
    0xD6: ("add.ovf", 0),
    0xD7: ("add.ovf.un", 0),
    0xD8: ("mul.ovf", 0),
    0xD9: ("mul.ovf.un", 0),
    0xDA: ("sub.ovf", 0),
    0xDB: ("sub.ovf.un", 0),
    0xDC: ("endfinally", 0),
    0xDD: ("leave", 4),
    0xDE: ("leave.s", 1),
    0xDF: ("stind.i", 0),
    0xE0: ("conv.u", 0),
    0xFE: ("prefix", 1),  # extended opcode prefix
}

# Two-byte opcodes (FE prefix)
OPCODES_FE = {
    0x00: ("arglist", 0),
    0x01: ("ceq", 0),
    0x02: ("cgt", 0),
    0x03: ("cgt.un", 0),
    0x04: ("clt", 0),
    0x05: ("clt.un", 0),
    0x06: ("ldftn", 4),
    0x07: ("ldvirtftn", 4),
    0x09: ("ldarg", 2),
    0x0A: ("ldarga", 2),
    0x0B: ("starg", 2),
    0x0C: ("ldloc", 2),
    0x0D: ("ldloca", 2),
    0x0E: ("stloc", 2),
    0x0F: ("localloc", 0),
    0x11: ("endfilter", 0),
    0x12: ("unaligned", 1),
    0x13: ("volatile", 0),
    0x14: ("tail", 0),
    0x15: ("initobj", 4),
    0x16: ("constrained", 4),
    0x17: ("cpblk", 0),
    0x18: ("initblk", 0),
    0x1A: ("rethrow", 0),
    0x1C: ("sizeof", 4),
    0x1D: ("refanytype", 0),
    0x1E: ("readonly", 0),
}


def decode_il(il_hex):
    """Decode IL hex string into readable instructions"""
    bytes_list = [int(b, 16) for b in il_hex.split()]
    instructions = []
    i = 0

    while i < len(bytes_list):
        offset = i
        op = bytes_list[i]
        i += 1

        if op == 0xFE:
            # Two-byte opcode
            if i < len(bytes_list):
                op2 = bytes_list[i]
                i += 1
                if op2 in OPCODES_FE:
                    name, operand_size = OPCODES_FE[op2]
                else:
                    name, operand_size = f"FE.{op2:02X}", 0
            else:
                name, operand_size = "FE.??", 0
        elif op in OPCODES:
            name, operand_size = OPCODES[op]
        else:
            name, operand_size = f"0x{op:02X}", 0

        # Handle special cases
        if name == "switch":
            # switch has variable length
            if i + 3 < len(bytes_list):
                count = struct.unpack('<I', bytes_list[i:i+4])[0]
                i += 4
                targets = []
                for j in range(count):
                    if i + 3 < len(bytes_list):
                        target = struct.unpack('<I', bytes_list[i:i+4])[0]
                        targets.append(target)
                        i += 4
                operand = f"[{', '.join(str(t) for t in targets)}]"
                instructions.append((offset, name, operand))
                continue

        # Read operand
        operand = ""
        if operand_size == 1:
            if i < len(bytes_list):
                operand = f"0x{bytes_list[i]:02X}"
                i += 1
        elif operand_size == 2:
            if i + 1 < len(bytes_list):
                operand = f"0x{bytes_list[i] | (bytes_list[i+1] << 8):04X}"
                i += 2
        elif operand_size == 4:
            if i + 3 < len(bytes_list):
                val = struct.unpack('<I', bytes_list[i:i+4])[0]
                operand = f"0x{val:08X}"
                i += 4
        elif operand_size == 8:
            if i + 7 < len(bytes_list):
                val = struct.unpack('<Q', bytes_list[i:i+8])[0]
                operand = f"0x{val:016X}"
                i += 8

        instructions.append((offset, name, operand))

    return instructions


def il_to_pseudocode(instructions, locals_info=None, method_name="", params_info=None):
    """Convert IL instructions to pseudo-code"""
    lines = []
    stack = []  # Simple stack simulation for expression building
    indent = 1

    if locals_info:
        lines.append(f"// Locals: {', '.join(locals_info)}")

    for offset, name, operand in instructions:
        if name == "nop":
            continue
        elif name == "ret":
            if stack:
                lines.append(f"{'  ' * indent}return {stack[-1]};")
            else:
                lines.append(f"{'  ' * indent}return;")
        elif name.startswith("ldarg"):
            if name == "ldarg.0":
                stack.append("this")
            elif name == "ldarg.1":
                stack.append("arg1")
            elif name == "ldarg.2":
                stack.append("arg2")
            elif name == "ldarg.3":
                stack.append("arg3")
            elif name == "ldarg.s":
                stack.append(f"arg{operand}")
            else:
                stack.append(f"arg({operand})")
        elif name.startswith("ldloc"):
            if name == "ldloc.0":
                stack.append("loc0")
            elif name == "ldloc.1":
                stack.append("loc1")
            elif name == "ldloc.2":
                stack.append("loc2")
            elif name == "ldloc.3":
                stack.append("loc3")
            elif name == "ldloc.s":
                stack.append(f"loc{int(operand, 16)}")
            else:
                stack.append(f"loc({operand})")
        elif name.startswith("stloc"):
            if stack:
                val = stack.pop()
                if name == "stloc.0":
                    lines.append(f"{'  ' * indent}loc0 = {val};")
                elif name == "stloc.1":
                    lines.append(f"{'  ' * indent}loc1 = {val};")
                elif name == "stloc.2":
                    lines.append(f"{'  ' * indent}loc2 = {val};")
                elif name == "stloc.3":
                    lines.append(f"{'  ' * indent}loc3 = {val};")
                elif name == "stloc.s":
                    lines.append(f"{'  ' * indent}loc{int(operand, 16)} = {val};")
                else:
                    lines.append(f"{'  ' * indent}loc({operand}) = {val};")
        elif name.startswith("ldc.i4"):
            if name == "ldc.i4.0":
                stack.append("0")
            elif name == "ldc.i4.1":
                stack.append("1")
            elif name == "ldc.i4.2":
                stack.append("2")
            elif name == "ldc.i4.3":
                stack.append("3")
            elif name == "ldc.i4.4":
                stack.append("4")
            elif name == "ldc.i4.5":
                stack.append("5")
            elif name == "ldc.i4.6":
                stack.append("6")
            elif name == "ldc.i4.7":
                stack.append("7")
            elif name == "ldc.i4.8":
                stack.append("8")
            elif name == "ldc.i4.m1":
                stack.append("-1")
            elif name == "ldc.i4.s":
                val = int(operand, 16)
                if val > 127:
                    val -= 256
                stack.append(str(val))
            elif name == "ldc.i4":
                val = int(operand, 16)
                if val > 0x7FFFFFFF:
                    val -= 0x100000000
                stack.append(str(val))
        elif name == "ldstr":
            stack.append(f"string({operand})")
        elif name in ("add", "sub", "mul", "div", "div.un", "rem", "rem.un",
                       "and", "or", "xor", "shl", "shr"):
            if len(stack) >= 2:
                b = stack.pop()
                a = stack.pop()
                op_map = {"add": "+", "sub": "-", "mul": "*", "div": "/",
                          "div.un": "/u", "rem": "%", "rem.un": "%u",
                          "and": "&", "or": "|", "xor": "^", "shl": "<<", "shr": ">>"}
                stack.append(f"({a} {op_map.get(name, name)} {b})")
        elif name in ("ceq", "cgt", "cgt.un", "clt", "clt.un"):
            if len(stack) >= 2:
                b = stack.pop()
                a = stack.pop()
                op_map = {"ceq": "==", "cgt": ">", "cgt.un": ">u", "clt": "<", "clt.un": "<u"}
                stack.append(f"({a} {op_map.get(name, name)} {b})")
        elif name == "neg":
            if stack:
                stack.append(f"(-{stack.pop()})")
        elif name == "not":
            if stack:
                stack.append(f"(~{stack.pop()})")
        elif name in ("conv.i1", "conv.i2", "conv.i4", "conv.i8", "conv.r4", "conv.r8",
                       "conv.u1", "conv.u2", "conv.u4", "conv.u8", "conv.i", "conv.u"):
            if stack:
                type_map = {"conv.i1": "(sbyte)", "conv.i2": "(short)", "conv.i4": "(int)",
                           "conv.i8": "(long)", "conv.r4": "(float)", "conv.r8": "(double)",
                           "conv.u1": "(byte)", "conv.u2": "(ushort)", "conv.u4": "(uint)",
                           "conv.u8": "(ulong)", "conv.i": "(IntPtr)", "conv.u": "(UIntPtr)"}
                stack.append(f"{type_map.get(name, name)}{stack.pop()}")
        elif name == "call":
            if len(stack) >= 1:
                # Simple: assume method call with args
                args = []
                # This is a simplification - real decompilation is more complex
                stack.append(f"call({operand}, ...)")
            else:
                stack.append(f"call({operand})")
        elif name == "callvirt":
            if len(stack) >= 2:
                obj = stack.pop()
                stack.append(f"{obj}.call({operand}, ...)")
            else:
                stack.append(f"callvirt({operand})")
        elif name == "ldfld":
            if stack:
                obj = stack.pop()
                stack.append(f"{obj}.field({operand})")
        elif name == "stfld":
            if len(stack) >= 2:
                val = stack.pop()
                obj = stack.pop()
                lines.append(f"{'  ' * indent}{obj}.field({operand}) = {val};")
        elif name == "ldsfld":
            stack.append(f"static({operand})")
        elif name == "stsfld":
            if stack:
                val = stack.pop()
                lines.append(f"{'  ' * indent}static({operand}) = {val};")
        elif name == "newobj":
            stack.append(f"new({operand}, ...)")
        elif name == "newarr":
            if stack:
                count = stack.pop()
                stack.append(f"new[{count}]({operand})")
        elif name == "ldlen":
            if stack:
                arr = stack.pop()
                stack.append(f"{arr}.Length")
        elif name.startswith("ldelem"):
            if len(stack) >= 2:
                idx = stack.pop()
                arr = stack.pop()
                stack.append(f"{arr}[{idx}]")
        elif name.startswith("stelem"):
            if len(stack) >= 3:
                val = stack.pop()
                idx = stack.pop()
                arr = stack.pop()
                lines.append(f"{'  ' * indent}{arr}[{idx}] = {val};")
        elif name.startswith("br"):
            if "false" in name:
                if stack:
                    cond = stack.pop()
                    lines.append(f"{'  ' * indent}if (!{cond}) goto {operand};")
            elif "true" in name:
                if stack:
                    cond = stack.pop()
                    lines.append(f"{'  ' * indent}if ({cond}) goto {operand};")
            elif name in ("beq", "bge", "bgt", "ble", "blt", "bne.un", "bge.un", "bgt.un", "ble.un", "blt.un"):
                if len(stack) >= 2:
                    b = stack.pop()
                    a = stack.pop()
                    op_map = {"beq": "==", "bge": ">=", "bgt": ">", "ble": "<=", "blt": "<",
                              "bne.un": "!=", "bge.un": ">=u", "bgt.un": ">u", "ble.un": "<=", "blt.un": "<u"}
                    lines.append(f"{'  ' * indent}if ({a} {op_map.get(name, name)} {b}) goto {operand};")
            else:
                lines.append(f"{'  ' * indent}goto {operand};")
        elif name == "dup":
            if stack:
                stack.append(stack[-1])
        elif name == "pop":
            if stack:
                stack.pop()
        elif name == "ldflda":
            if stack:
                obj = stack.pop()
                stack.append(f"&{obj}.field({operand})")
        elif name == "ldloca":
            stack.append(f"&loc{int(operand, 16) if operand.startswith('0x') else operand}")
        elif name == "ldarga":
            stack.append(f"&arg{int(operand, 16) if operand.startswith('0x') else operand}")
        elif name == "throw":
            if stack:
                lines.append(f"{'  ' * indent}throw {stack.pop()};")
        elif name == "rethrow":
            lines.append(f"{'  ' * indent}rethrow;")
        elif name == "initobj":
            if stack:
                stack.pop()
                lines.append(f"{'  ' * indent}initobj({operand});")
        elif name == "box":
            if stack:
                stack.append(f"box({stack.pop()})")
        elif name == "unbox.any":
            if stack:
                stack.append(f"unbox({stack.pop()})")
        else:
            lines.append(f"{'  ' * indent}// {name} {operand}")

    return "\n".join(lines)


# Extract and decode all key methods
ps_script = r"""
$asm = [Reflection.Assembly]::LoadFile('F:\file\模块资料\MKS_142模块相关资料\上位机\V2版 - 20220812(3).exe')
$types = $asm.GetTypes()

$keyMethods = @('MatchProtocol', 'AnalysisProtocol', 'SaveTJPack', 'serialPort1_DataReceived',
                'versionFiltration', 'isHave255', 'isAllZero', 'initPackageIndex',
                'ShowReport', 'InitRecData', 'initVersionFiltration',
                'TimerShowWave', 'sysTimer_Tick',
                'ShowRRNewImage', 'changeColor', 'RefurbishArray', 'ShowBloodstate',
                'createCSVTitle', 'savedata')

$results = @{}
foreach ($t in $types) {
    $methods = $t.GetMethods([Reflection.BindingFlags]::NonPublic -bor [Reflection.BindingFlags]::Public -bor [Reflection.BindingFlags]::Instance -bor [Reflection.BindingFlags]::Static -bor [Reflection.BindingFlags]::DeclaredOnly)
    foreach ($m in $methods) {
        if ($m.Name -in $keyMethods) {
            $body = $m.GetMethodBody()
            if ($body) {
                $il = $body.GetILAsByteArray()
                $hex = ($il | ForEach-Object { $_.ToString('X2') }) -join ' '
                $typeName = $t.FullName
                $params = ($m.GetParameters() | ForEach-Object { "$($_.ParameterType.Name) $($_.Name)" }) -join ', '
                $locals = @()
                foreach ($loc in $body.LocalVariables) {
                    $locals += "$($loc.LocalType.Name) loc$($loc.LocalIndex)"
                }
                $key = "$typeName::$($m.Name)"
                $results[$key] = @{
                    returnType = $m.ReturnType.Name
                    params = $params
                    ilLength = $il.Length
                    il = $hex
                    locals = $locals
                }
            }
        }
    }
}
$results | ConvertTo-Json -Depth 3
"""

print("Extracting IL from assembly...")
result = subprocess.run(['powershell.exe', '-NoProfile', '-Command', ps_script],
                       capture_output=True, text=True, timeout=120)

if result.returncode != 0:
    print(f"Error: {result.stderr}")
    exit(1)

try:
    data = json.loads(result.stdout)
except json.JSONDecodeError as e:
    print(f"JSON parse error: {e}")
    print(result.stdout[:2000])
    exit(1)

# Decode each method
for method_key, method_info in data.items():
    print(f"\n{'='*80}")
    print(f"Method: {method_key}")
    print(f"  Return: {method_info['returnType']}")
    print(f"  Params: {method_info['params']}")
    print(f"  Locals: {', '.join(method_info['locals'])}")
    print(f"  IL Length: {method_info['ilLength']} bytes")
    print(f"{'='*80}")

    instructions = decode_il(method_info['il'])
    pseudocode = il_to_pseudocode(instructions, method_info['locals'], method_key)
    print(pseudocode)
