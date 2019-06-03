import _xxsubinterpreters as interpreters

# Create a sub-interpreter
interpid = interpreters.create()

interpreters.run_module(interpid, "cython_test", "foo")