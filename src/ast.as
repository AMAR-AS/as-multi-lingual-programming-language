# =========================================================
# ast.as — Abstract Syntax Tree for Ask (.as) language
# =========================================================
# Uses ONLY your keywords: class/def/ret/if/elif/else/for/in/while/brk/cont/try/catch/finally/raise
# No foreign keywords.
# Each node is a class with fields and optional methods.
# =========================================================

# ---------- Base ----------
class Node
    def init(kind, pos)
        self.kind = kind      # string, e.g. "Num", "BinOp"
        self.pos  = pos       # source position (line, col)
    end
end

# ---------- Program root ----------
class Program : Node
    def init(stmts, pos)
        self.kind  = "Program"
        self.stmts = stmts   # list of Node
        self.pos   = pos
    end
end

# ---------- Expressions ----------
class NumLiteral : Node
    def init(value, pos)
        self.kind  = "NumLiteral"
        self.value = value
        self.pos   = pos
    end
end

class DecLiteral : Node
    def init(value, pos)
        self.kind  = "DecLiteral"
        self.value = value
        self.pos   = pos
    end
end

class TxtLiteral : Node
    def init(value, pos)
        self.kind  = "TxtLiteral"
        self.value = value
        self.pos   = pos
    end
end

class BoolLiteral : Node
    def init(value, pos)
        self.kind  = "BoolLiteral"
        self.value = value    # true / false
        self.pos   = pos
    end
end

class NoneLiteral : Node
    def init(pos)
        self.kind = "NoneLiteral"
        self.value = None
        self.pos = pos
    end
end

class Var : Node
    def init(name, pos)
        self.kind = "Var"
        self.name = name
        self.pos  = pos
    end
end

class BinOp : Node
    def init(op, left, right, pos)
        self.kind  = "BinOp"
        self.op    = op      # "+", "-", "*", "/", "%"
        self.left  = left
        self.right = right
        self.pos   = pos
    end
end

class UnaryOp : Node
    def init(op, expr, pos)
        self.kind = "UnaryOp"
        self.op   = op       # "-", "not"
        self.expr = expr
        self.pos  = pos
    end
end

class Call : Node
    def init(func, args, pos)
        self.kind = "Call"
        self.func = func    # Var or attribute
        self.args = args    # list of expr
        self.pos  = pos
    end
end

class Index : Node
    def init(seq, idx, pos)
        self.kind = "Index"
        self.seq  = seq
        self.idx  = idx
        self.pos  = pos
    end
end

class Attr : Node
    def init(obj, name, pos)
        self.kind = "Attr"
        self.obj  = obj
        self.name = name
        self.pos  = pos
    end
end

# ---------- Statements ----------
class Assign : Node
    def init(target, value, pos)
        self.kind   = "Assign"
        self.target = target   # Var or Index
        self.value  = value
        self.pos    = pos
    end
end

class ExprStmt : Node
    def init(expr, pos)
        self.kind = "ExprStmt"
        self.expr = expr
        self.pos  = pos
    end
end

class Block : Node
    def init(stmts, pos)
        self.kind  = "Block"
        self.stmts = stmts
        self.pos   = pos
    end
end

class If : Node
    def init(cond, then_block, elifs, else_block, pos)
        self.kind       = "If"
        self.cond       = cond
        self.then_block = then_block
        self.elifs      = elifs       # list of (cond, block)
        self.else_block = else_block  # optional
        self.pos        = pos
    end
end

class While : Node
    def init(cond, body, pos)
        self.kind = "While"
        self.cond = cond
        self.body = body
        self.pos  = pos
    end
end

class For : Node
    def init(var, iterable, body, pos)
        self.kind     = "For"
        self.var      = var
        self.iterable = iterable
        self.body     = body
        self.pos      = pos
    end
end

class FuncDef : Node
    def init(name, params, body, pos)
        self.kind   = "FuncDef"
        self.name   = name
        self.params = params   # list of names
        self.body   = body     # Block
        self.pos    = pos
    end
end

class ClassDef : Node
    def init(name, methods, pos)
        self.kind   = "ClassDef"
        self.name   = name
        self.methods = methods # list of FuncDef
        self.pos    = pos
    end
end

class Return : Node
    def init(value, pos)
        self.kind  = "Return"
        self.value = value   # expr or None
        self.pos   = pos
    end
end

class Break : Node
    def init(pos)
        self.kind = "Break"
        self.pos  = pos
    end
end

class Continue : Node
    def init(pos)
        self.kind = "Continue"
        self.pos  = pos
    end
end

class Raise : Node
    def init(expr, pos)
        self.kind = "Raise"
        self.expr = expr
        self.pos  = pos
    end
end

class Try : Node
    def init(body, catches, finally_block, pos)
        self.kind   = "Try"
        self.body   = body
        self.catches = catches       # list of (var, block)
        self.finally_block = finally_block
        self.pos    = pos
    end
end

# ---------- Data literals ----------
class ListLiteral : Node
    def init(items, pos)
        self.kind  = "ListLiteral"
        self.items = items
        self.pos   = pos
    end
end

class MapLiteral : Node
    def init(pairs, pos)
        self.kind  = "MapLiteral"
        self.pairs = pairs   # list of (key, value)
        self.pos   = pos
    end
end

# ---------- Helper factory ----------
def make_binop(op, left, right, pos)
    ret BinOp(op, left, right, pos)
end

def make_call(func, args, pos)
    ret Call(func, args, pos)
end

# =========================================================
# End of ast.as
# =========================================================
