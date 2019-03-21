#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Python.h"
#include "ceval.h"
#include "unicodeobject.h"
#include "pythonrun.h"
#include "Python-ast.h"
#include "pyarena.h"
#include "compile.h"

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int
main(int argc, char **argv)
{
    signal(SIGSEGV, handler);   // install our handler
    char* input;
    if (argc > 1){
        input = argv[1];
    } else {
        printf("Provide script.");
        exit(-1);
    }
    mod_ty mod;
    PyObject *filename,*co,*bytes,*unicode;
    PyArena *arena;
    PyCompilerFlags flags;
    char *in = "a = 1";
    flags.cf_flags = 0;
    flags.cf_feature_version = PY_MINOR_VERSION;

    Py_Initialize();

    char* fn = "test.py";
    char* errors;
    unicode = PyUnicode_DecodeASCII(in, strlen(in), &errors);
    if (unicode == NULL) {
        exit(-1);
    }
    
    bytes = PyUnicode_AsUTF8String(unicode);
    Py_DECREF(unicode);
    char* str = PyBytes_AsString(bytes);
    co = Py_CompileStringFlags(str, fn, Py_file_input, NULL);

    if (co == NULL) {
        exit(-1);
    }
    arena = PyArena_New();
    if (arena == NULL)
        exit(-1);

    printf("Compiled!");
    PyArena_Free(arena);
    return 0;
}