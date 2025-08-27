class IR
    var code = []

    func generate(node)
        if node is null -> return

        if node.type == "Assignment" ->
            var name = node.children[0].value
            var expr = node.children[1].value
            code.push(["MOV", expr, "", name])
        end

        if node.type == "BinaryOp" ->
            var left = node.children[0].value
            var right = node.children[1].value
            var temp = "t" + str(len(code))
            code.push([node.value, left, right, temp])
        end

        for child in node.children ->
            generate(child)
        end
    end

    func printIR()
        for instr in code ->
            print(instr[0], instr[1], instr[2], "->", instr[3])
        end
    end
end
