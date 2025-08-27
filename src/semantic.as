class Semantic
    var symbols = map()

    func analyze(node)
        if node is null -> return

        if node.type == "Assignment" ->
            var name = node.children[0].value
            if not declared(name) ->
                print("[Semantic Error] Variable ", name, " not declared!")
                return
            end

            analyze(node.children[1])
            var exprType = node.children[1].type
            checkAssign(name, exprType)
        end

        for child in node.children ->
            analyze(child)
        end
    end

    func declare(name, type)
        symbols[name] = type
    end

    func declared(name)
        return name in symbols
    end

    func typeOf(name)
        if declared(name) -> return symbols[name]
        else -> return "undefined"
    end

    func checkAssign(name, type)
        var declaredType = typeOf(name)
        if declaredType != type ->
            print("[Semantic Error] Cannot assign ", type, " to ", declaredType, " variable ", name)
        end
    end
end
