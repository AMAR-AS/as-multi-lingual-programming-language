# Minimal indigenous lexer written in Ask
# Note: runs on your Ask interpreter; mirrors C++ behavior (simplified)

class Lexer
    fcn init(src: txt)
        self.src = src
        self.pos = 0
        self.line = 1
        self.col = 1

    fcn atEnd() -> bool
        ret self.pos >= len(self.src)

    fcn peek(ahead: num = 0) -> txt
        i = self.pos + ahead
        if i >= len(self.src) ret ""
        ret self.src[i]

    fcn advance() -> txt
        if self.atEnd() ret ""
        c = self.src[self.pos]
        self.pos = self.pos + 1
        if c == "\n"
            self.line = self.line + 1
            self.col = 1
        else
            self.col = self.col + 1
        ret c

    fcn isStart(c: txt) -> bool
        ret (c.isalpha() or c == "_")

    fcn isCont(c: txt) -> bool
        ret (c.isalnum() or c == "_")

    fcn skipWhitespace()
        loop:
            c = self.peek()
            if c in [" ", "\t", "\r"]
                self.advance()
                goto loop
            if c == "#"
                while self.peek() != "\n" and not self.atEnd()
                    self.advance()
                goto loop
            if c == "/" and self.peek(1) == "*"
                self.advance(); self.advance()
                while not self.atEnd()
                    if self.peek() == "*" and self.peek(1) == "/"
                        self.advance(); self.advance(); break
                    self.advance()
                goto loop

    fcn makeNumber() -> dict
        startCol = self.col
        value = ""
        seenDot = False
        while self.peek().isdigit() or self.peek() == "_" or (not seenDot and self.peek() == ".")
            c = self.advance()
            if c == "."
                seenDot = True
            if c != "_"
                value = value + c
        ret {"type":"Number","lexeme":value,"line":self.line,"col":startCol}

    fcn makeString() -> dict
        startCol = self.col
        q = self.advance()
        out = ""
        while not self.atEnd() and self.peek() != q
            c = self.advance()
            if c == "\\"
                e = self.advance()
                if e == "n" out = out + "\n"
                elif e == "t" out = out + "\t"
                elif e == "r" out = out + "\r"
                else out = out + e
            else
                out = out + c
        if self.peek() == q
            self.advance()
        ret {"type":"String","lexeme":out,"line":self.line,"col":startCol}

    fcn lower(s: txt) -> txt
        ret s.lower()

    fcn makeIdentOrKeyword() -> dict
        startCol = self.col
        id = ""
        while self.isCont(self.peek())
            id = id + self.advance()
        low = self.lower(id)

        specials = {"true":"Bool","false":"Bool","yes":"Bool","no":"Bool","none":"None","null":"None"}
        if low in specials
            ret {"type":specials[low],"lexeme":low,"line":self.line,"col":startCol}

        keywords = {"fcn","function","ret","giveback","if","elif","orif","else","check","for","let","while","when",
                    "break","brk","continue","cont","skip","exit","class","self","own","mine",
                    "import","include","from","as","try","hit","catch","except","finally","cleanup","raise","throw",
                    "with","pass","donothing","leave","del","delete"}
        if low in keywords
            ret {"type":"Keyword","lexeme":low,"line":self.line,"col":startCol}
        ret {"type":"Identifier","lexeme":id,"line":self.line,"col":startCol}

    fcn makeOpOrSym() -> dict
        startCol = self.col
        c0 = self.advance()
        two = c0 + self.peek()
        twoSet = {"==","!=", "<=", ">=", "&&","||","->","=>","::","++","--",":="}
        if two in twoSet
            self.advance()
            ret {"type":"Operator","lexeme":two,"line":self.line,"col":startCol}
        if c0 in ["+","-","*","/","%","=","!","<",">","&","|","^","~","?",":","."]
            ret {"type":"Operator","lexeme":c0,"line":self.line,"col":startCol}
        if c0 in ["(",")","{","}","[","]",",",";"]
            ret {"type":"Symbol","lexeme":c0,"line":self.line,"col":startCol}
        ret {"type":"Error","lexeme":c0,"line":self.line,"col":startCol}

    fcn tokenize() -> list
        toks = []
        while not self.atEnd()
            self.skipWhitespace()
            if self.atEnd() break
            c = self.peek()
            if c.isdigit()
                toks.append(self.makeNumber())
            elif c == "\"" or c == "'"
                toks.append(self.makeString())
            elif self.isStart(c)
                toks.append(self.makeIdentOrKeyword())
            elif c == "\n"
                self.advance()
                toks.append({"type":"Newline","lexeme":"\\n","line":self.line,"col":self.col})
            else
                toks.append(self.makeOpOrSym())
        toks.append({"type":"EndOfFile","lexeme":"","line":self.line,"col":self.col})
        ret toks
