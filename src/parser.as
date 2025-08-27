# parser.as — Ask-native recursive-descent + Pratt

class ASTNode
    fcn init(txt kind, any val = none, list kids = [])
        own.kind = kind
        own.val = val
        own.kids = kids
    end
end

class Parser
    fcn init(list toks)
        own.toks = toks
        own.i = 0
        own.errs = []
    end

    fcn cur() ret own.toks[own.i]
    fcn atEnd() ret own.cur().type == "EOF"

    fcn match(txt s) -> bool
        if own.cur().lexeme == s
            own.i = own.i + 1
            ret True
        ret False
    end

    fcn check(txt s) -> bool ret own.cur().lexeme == s
    fcn advance() own.i = own.i + 1

    # --- Statements ---
    fcn parse() -> ASTNode
        list ss = []
        while not own.atEnd()
            ss.append(own.statement())
            # skip newlines/semicolons
            while own.cur().type == "NEWLINE" or own.cur().lexeme == ";"
                own.advance()
        end
        ret ASTNode("PROGRAM", none, ss)
    end

    fcn statement() -> ASTNode
        t = own.cur()
        if t.type == "KEYWORD"
            k = t.lexeme
            if k in ["if","check"] ret own.ifStmt()
            if k in ["while","when"] ret own.whileStmt()
            if k in ["for","let"] ret own.forStmt()
            if k in ["fcn","function"] ret own.funcDef()
            if k in ["ret","giveback"] ret own.retStmt()
            if k in ["break","brk","exit"] ret ASTNode("BREAK")
            if k in ["continue","cont","skip"] ret ASTNode("CONTINUE")
            if k in ["try","hit"] ret own.tryCatch()
            if k == "with" ret own.withStmt()
        end
        ret own.assignOrExpr()
    end

    fcn assignOrExpr() -> ASTNode
        if own.cur().type == "IDENT" and own.toks[own.i+1].lexeme == "="
            name = own.cur().lexeme
            own.i = own.i + 2
            val = own.expression()
            ret ASTNode("ASSIGN", none, [ASTNode("IDENT", name), val])
        end
        ret ASTNode("EXPRSTMT", none, [own.expression()])
    end

    fcn block() -> ASTNode
        if own.match("{")
            list ss = []
            while not own.match("}")
                ss.append(own.statement())
                while own.cur().type == "NEWLINE" or own.cur().lexeme == ";"
                    own.advance()
            end
            ret ASTNode("BLOCK", none, ss)
        end
        ret ASTNode("BLOCK", none, [own.statement()])
    end

    fcn ifStmt() -> ASTNode
        own.advance()  # consume if/check
        cond = own.expression()
        thenB = own.block()
        list arms = [[cond, thenB]]
        while own.cur().lexeme in ["elif","orif"]
            own.advance()
            c = own.expression()
            b = own.block()
            arms.append([c,b])
        end
        node = ASTNode("IF", none, [])
        node.arms = arms
        if own.cur().lexeme == "else"
            own.advance()
            node.elseBlock = own.block()
        end
        ret node
    end

    fcn whileStmt() -> ASTNode
        own.advance()
        cond = own.expression()
        body = own.block()
        ret ASTNode("WHILE", none, [cond, body])
    end

    fcn forStmt() -> ASTNode
        own.advance()
        var = own.cur().lexeme
        own.advance()
        own.match("in")
        it = own.expression()
        body = own.block()
        ret ASTNode("FORIN", none, [ASTNode("IDENT", var), it, body])
    end

    fcn funcDef() -> ASTNode
        own.advance()
        name = own.cur().lexeme
        own.advance()
        own.match("(")
        list ps = []
        if not own.check(")")
            loop:
                p = own.cur().lexeme
                own.advance()
                if own.match(":")
                    t = own.cur().lexeme
                    own.advance()
                    ps.append([p, t])
                else
                    ps.append([p, none])
                end
                if not own.match(",") brk
            end
        end
        own.match(")")
        retType = none
        if own.match("->")
            retType = own.cur().lexeme
            own.advance()
        end
        body = own.block()
        node = ASTNode("FUNCDEF", name, [])
        node.params = ps
        node.retType = retType
        node.kids = [body]
        ret node
    end

    fcn retStmt() -> ASTNode
        own.advance()
        if own.cur().type in ["NEWLINE","SYMBOL"] and own.cur().lexeme in [";","}"]
            ret ASTNode("RETURN", none, [])
        end
        val = own.expression()
        ret ASTNode("RETURN", none, [val])
    end

    fcn tryCatch() -> ASTNode
        own.advance()
        tb = own.block()
        own.match("catch"); own.match("(")
        err = own.cur().lexeme; own.advance()
        own.match(")")
        cb = own.block()
        fb = none
        if own.cur().lexeme in ["finally","cleanup"]
            own.advance()
            fb = own.block()
        end
        node = ASTNode("TRYCATCH", none, [])
        node.tryBlock = tb; node.errName = err; node.catchBlock = cb; node.finallyBlock = fb
        ret node
    end

    fcn withStmt() -> ASTNode
        own.advance()
        ctx = own.expression()
        body = own.block()
        ret ASTNode("WITH", none, [ctx, body])
    end

    # --- Expressions (Pratt) ---
    fcn expression() -> ASTNode
        left = own.unary()
        # binary ops with increasing precedence (implemented simply)
        loop:
            op = own.cur().lexeme
            if op in ["||","&&","==","!=", "<","<=",">",">=","+","-","*","/","%"]
                own.advance()
                right = own.unary()
                left = ASTNode("BINARY", op, [left, right])
                goto loop
            end
        end
        ret own.postfix(left)
    end

    fcn unary() -> ASTNode
        op = own.cur().lexeme
        if op in ["!","-","+"]
            own.advance()
            e = own.unary()
            ret ASTNode("UNARY", op, [e])
        end
        ret own.primary()
    end

    fcn postfix(n)
        loop:
            if own.match("(")
                list args = []
                if not own.check(")")
                    loop:
                        args.append(own.expression())
                        if not own.match(",") brk
                    end
                end
                own.match(")")
                n = ASTNode("CALL", none, [n] + args)
                goto loop
            end
            if own.match("[")
                idx = own.expression()
                own.match("]")
                n = ASTNode("INDEX", none, [n, idx])
                goto loop
            end
            if own.match(".")
                name = own.cur().lexeme; own.advance()
                n = ASTNode("MEMBER", name, [n])
                goto loop
            end
            if own.match("++") or own.match("--")
                n = ASTNode("POSTFIX", own.toks[own.i-1].lexeme, [n])
                goto loop
            end
            brk
        end
        ret n
    end

    fcn primary() -> ASTNode
        t = own.cur()
        if t.type == "IDENT" own.advance(); ret ASTNode("IDENT", t.lexeme)
        if t.type == "NUMBER" own.advance(); ret ASTNode("NUMBER", t.lexeme)
        if t.type == "STRING" own.advance(); ret ASTNode("STRING", t.lexeme)
        if t.type == "BOOL" own.advance(); ret ASTNode("BOOL", t.lexeme)
        if t.type == "NONE" own.advance(); ret ASTNode("NONE", t.lexeme)
        if own.match("(")
            e = own.expression()
            own.match(")")
            ret e
        end
        ret ASTNode("ERROR", "unexpected primary: " + t.lexeme)
    end
end
