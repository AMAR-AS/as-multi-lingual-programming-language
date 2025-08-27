# =========================================================
# math.as — advanced math library
# =========================================================

def sin(x)   ret __native_sin(x) end
def cos(x)   ret __native_cos(x) end
def tan(x)   ret __native_tan(x) end
def asin(x)  ret __native_asin(x) end
def acos(x)  ret __native_acos(x) end
def atan(x)  ret __native_atan(x) end
def atan2(y, x) ret __native_atan2(y, x) end

def exp(x)   ret __native_exp(x) end
def log(x)   ret __native_log(x) end        # natural log
def log10(x) ret __native_log10(x) end

def min(a, b) ret __native_min(a, b) end
def max(a, b) ret __native_max(a, b) end
def clamp(x, lo, hi)
    if x < lo ret lo end
    if x > hi ret hi end
    ret x
end

# random extras
def seed(v)  __native_seed(v) ; ret None end
