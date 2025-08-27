class Sandbox
    func run(ir)
        for instr in ir ->
            if instr[0] == "DIV" and instr[2] == "0" ->
                print("[Sandbox Error] Division by zero detected!")
                return false
            end

            if instr[0] == "MOV" and instr[1] == "" ->
                print("[Sandbox Error] Invalid MOV instruction!")
                return false
            end
        end

        print("[Sandbox] All instructions verified and safe.")
        return true
    end
end
