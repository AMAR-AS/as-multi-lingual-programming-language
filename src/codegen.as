class CodeGenerator
    func generate(ir)
        asmCode = "section .text\n"
        asmCode += "global _start\n\n"
        asmCode += "_start:\n"

        for instr in ir ->
            if instr[0] == "ADD" ->
                asmCode += "    add " + instr[1] + ", " + instr[2] + "\n"
            elif instr[0] == "SUB" ->
                asmCode += "    sub " + instr[1] + ", " + instr[2] + "\n"
            elif instr[0] == "MOV" ->
                asmCode += "    mov " + instr[1] + ", " + instr[2] + "\n"
            elif instr[0] == "PRINT" ->
                asmCode += "    ; printing not yet implemented in raw asm\n"
            end
        end

        asmCode += "    mov eax, 60   ; syscall: exit\n"
        asmCode += "    xor edi, edi  ; status 0\n"
        asmCode += "    syscall\n"

        return asmCode
    end
end
