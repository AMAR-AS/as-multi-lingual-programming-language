# =========================================================
# vm.as — Ask Virtual Machine surface (final)
# Defines VM-visible interfaces and bytecode meta. Execution
# internals are provided by host via __native_vm_* bridges.
# =========================================================

# ----- Bytecode constants / meta -----
class Bytecode
    def init(bytes)
        self.bytes = bytes    # list of num (0..255)
    end
end

# List of opcodes (keep in sync with host)
OP_LOAD_CONST = 1
OP_LOAD_VAR   = 2
OP_STORE_VAR  = 3
OP_ADD        = 4
OP_SUB        = 5
OP_MUL        = 6
OP_DIV        = 7
OP_MOD        = 8
OP_EQ         = 9
OP_NE         = 10
OP_LT         = 11
OP_LE         = 12
OP_GT         = 13
OP_GE         = 14
OP_JMP        = 15
OP_JZ         = 16
OP_JNZ        = 17
OP_CALL       = 18
OP_RET        = 19
OP_PRINT      = 20
OP_INPUT      = 21
OP_INDEX_GET  = 22
OP_INDEX_SET  = 23
OP_LIST_NEW   = 24
OP_MAP_NEW    = 25
OP_HALT       = 255

# ----- VM API exposed to .as programs -----
class VM
    def init()
        self.handle = __native_vm_new()
    end

    def load(bytecode)
        __native_vm_load(self.handle, bytecode.bytes)
        ret self
    end

    def run()
        # returns exit code or None
        ret __native_vm_run(self.handle)
    end

    def step()
        # single step; returns true if more steps available
        ret __native_vm_step(self.handle)
    end

    def reset()
        __native_vm_reset(self.handle)
        ret self
    end

    def regs()
        # returns map {ip, sp, bp, frames, stack_preview}
        ret __native_vm_regs(self.handle)
    end

    def set_breakpoints(points)
        __native_vm_set_bps(self.handle, points)
        ret self
    end

    def set_global(name, v)
        __native_vm_set_global(self.handle, name, v)
        ret self
    end

    def get_global(name)
        ret __native_vm_get_global(self.handle, name)
    end
end

# ----- Assembler helpers (optional) -----
def assemble(instrs)
    # instrs: list of [op, args...]; returns Bytecode
    bytes = __native_asm_encode(instrs)
    ret Bytecode(bytes)
end

def disassemble(bytecode)
    ret __native_asm_decode(bytecode.bytes)
end

# ----- Quick-run helper -----
def run_bytecode(instrs)
    bc = assemble(instrs)
    m = VM()
    m.load(bc)
    ret m.run()
end

# ========================= end vm.as =========================
