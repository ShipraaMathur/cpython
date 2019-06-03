import cython

def foo():
    cdef int i = 1
    if cython.compiled:
        print(f"I'm a Cython compiled module {i}!")
