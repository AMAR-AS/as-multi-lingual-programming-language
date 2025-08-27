class Optimizer
    func optimize(ir)
        var optimized = ir

        for instr in optimized ->
            if instr[0] in ["ADD", "SUB", "MUL", "DIV"] ->
                if isnum(instr[1]) and isnum(instr[2]) ->
                    var a = int(instr[1])
                    var b = int(instr[2])
                    var res = 0

                    if instr[0] == "ADD" -> res = a + b
                    if instr[0] == "SUB" -> res = a - b
                    if instr[0] == "MUL" -> res = a * b
                    if instr[0] == "DIV" and b != 0 -> res = a / b

                    instr[0] = "MOV"
                    instr[1] = str(res)
                    instr[2] = ""
                end
            end
        end

        return optimized
    end
end
