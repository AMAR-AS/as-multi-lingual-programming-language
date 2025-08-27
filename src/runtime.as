# =========================================================
# runtime.as — core runtime for Ask (.as) language
# Uses ONLY your keywords: class/def/ret/if/elif/else/for/in/while/brk/cont/try/catch/finally/raise
# Booleans: true/false  | None: none/null -> None (handled by translator)
# Types in decl are not required; conversions use: num/dec/float/txt/str/string/bool/map/list
# =========================================================

# ---------- I/O ----------
def print(v)
    # minimal print without newline; println provided below
    __native_print(v)   # host/VM should bind this to stdout write
    ret None
end

def println(v)
    __native_print(v)
    __native_print("\n")
    ret None
end

def readln()
    v = __native_input()   # host/VM should bind this to stdin read
    ret v
end

# ---------- Introspection / utils ----------
def len(x)
    # sequences (txt/list/array) & maps
    if __native_is_string(x)
        ret __native_strlen(x)
    elif __native_is_list(x)
        ret __native_list_len(x)
    elif __native_is_map(x)
        ret __native_map_len(x)
    else
        raise "len() unsupported for this value"
    end
end

def type(x)
    # returns "num" | "dec" | "txt" | "bool" | "list" | "map" | "none"
    ret __native_typeof(x)
end

# ---------- Conversions ----------
def to_num(x)
    # convert txt/bool/num -> num (int when possible)
    ret __native_to_num(x)
end

def to_dec(x)
    # convert to floating/decimal
    ret __native_to_dec(x)
end

def to_txt(x)
    ret __native_to_txt(x)
end

def to_bool(x)
    ret __native_to_bool(x)
end

# Aliases matching your keywords
def int(x)    ret to_num(x) end
def num(x)    ret to_num(x) end
def float(x)  ret to_dec(x) end
def dec(x)    ret to_dec(x) end
def txt(x)    ret to_txt(x) end
def str(x)    ret to_txt(x) end
def string(x) ret to_txt(x) end
def bool(x)   ret to_bool(x) end

# ---------- Math ----------
def add(a, b) ret a + b end
def sub(a, b) ret a - b end
def mul(a, b) ret a * b end

def div(a, b)
    if b == 0
        raise "division by zero"
    end
    ret a / b
end

def mod(a, b)
    if b == 0
        raise "mod by zero"
    end
    ret a % b
end

def pow(a, b) ret __native_pow(a, b) end
def abs(x)    ret __native_abs(x) end
def floor(x)  ret __native_floor(x) end
def ceil(x)   ret __native_ceil(x) end
def sqrt(x)
    if x < 0
        raise "sqrt of negative"
    end
    ret __native_sqrt(x)
end

# ---------- Random & Time ----------
def now_ms()      ret __native_now_ms() end
def random()      ret __native_rand01() end         # 0.0 <= r < 1.0
def randint(a, b) ret __native_randint(a, b) end    # inclusive

# ---------- Strings (Unicode-first) ----------
# NOTE: host must ensure UTF-8 handling in __native_* functions.

def trim(s)     ret __native_str_trim(s) end
def lower(s)    ret __native_str_lower(s) end
def upper(s)    ret __native_str_upper(s) end
def startswith(s, prefix) ret __native_startswith(s, prefix) end
def endswith(s, suffix)   ret __native_endswith(s, suffix) end
def find(s, needle)       ret __native_str_find(s, needle) end
def replace(s, old, new)  ret __native_str_replace(s, old, new) end
def split(s, sep)         ret __native_str_split(s, sep) end
def join(sep, parts)      ret __native_str_join(sep, parts) end
def codepoints(s)         ret __native_str_codepoints(s) end  # returns list of ints
def from_codepoints(lst)  ret __native_str_from_codepoints(lst) end
def normalize_nfc(s)      ret __native_str_normalize_nfc(s) end
def normalize_nfd(s)      ret __native_str_normalize_nfd(s) end

# ---------- Lists ----------
def push(lst, v)
    __native_list_push(lst, v)
    ret lst
end

def pop(lst)
    if __native_list_len(lst) == 0
        raise "pop from empty list"
    end
    ret __native_list_pop(lst)
end

def append(lst, v) ret push(lst, v) end

def extend(lst, other)
    __native_list_extend(lst, other)
    ret lst
end

def slice(lst, start, stop)
    ret __native_list_slice(lst, start, stop)
end

def reverse(lst)
    __native_list_reverse(lst)
    ret lst
end

# ---------- Maps (dict) ----------
def has(m, k)       ret __native_map_has(m, k) end
def get(m, k, dflt)
    if has(m, k)
        ret __native_map_get(m, k)
    else
        ret dflt
    end
end

def set(m, k, v)
    __native_map_set(m, k, v)
    ret m
end

def keys(m)   ret __native_map_keys(m) end
def values(m) ret __native_map_values(m) end
def items(m)  ret __native_map_items(m) end

# ---------- Functional helpers ----------
def range(a, b)
    # returns list [a, a+1, ..., b-1]
    ret __native_range(a, b)
end

def enumerate(lst)
    # returns list of [index, value]
    ret __native_enumerate(lst)
end

def map_fn(lst, f)
    out = []
    for x in lst
        out = __native_list_push(out, f(x))
    end
    ret out
end

def filter_fn(lst, pred)
    out = []
    for x in lst
        if pred(x)
            out = __native_list_push(out, x)
        end
    end
    ret out
end

def reduce_fn(lst, f, init)
    acc = init
    for x in lst
        acc = f(acc, x)
    end
    ret acc
end

# ---------- Minimal safe FS (sandboxed) ----------
# These call into host that enforces a sandbox (no absolute paths, no escapes).
def fs_read(path)
    ret __native_fs_read(path)   # must raise on violation
end

def fs_write(path, txtdata)
    __native_fs_write(path, txtdata)
    ret None
end

def fs_exists(path) ret __native_fs_exists(path) end
def fs_list(path)   ret __native_fs_list(path) end

# ---------- Concurrency stubs (future) ----------
# Host/VM can wire these to a thread pool / async runtime.
def spawn(f)
    ret __native_spawn(f)    # returns task id or handle
end

def wait(task)
    ret __native_wait(task)  # returns result
end

# ---------- Built-in print family (Unicode aware) ----------
def print_json(x)
    ret __native_print_json(x)
end

# ---------- Error helpers ----------
def assert_ok(cond, msg)
    if not cond
        raise msg
    end
    ret None
end

# ---------- Example: high-level API placeholders ----------
# Web / AI / LLM modules will live in stdlib later, these are placeholders.

class Web
    def get(url)
        ret __native_http_get(url)    # returns map {status, headers, body}
    end

    def post(url, body, headers)
        ret __native_http_post(url, body, headers)
    end
end

class AI
    def embed(txt)
        ret __native_ai_embed(txt)    # returns list of dec
    end

    def classify(txt, labels)
        ret __native_ai_classify(txt, labels)  # returns map {label, score}
    end
end

class LLM
    def generate(prompt, max_tokens)
        ret __native_llm_generate(prompt, max_tokens)
    end
end

# ---------- End of runtime.as ----------
