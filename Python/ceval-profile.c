#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <dtrace.h>

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

static int
rec(const dtrace_probedata_t *data, const dtrace_recdesc_t *rec, void *arg)
{
    // TODO.. stuff with data
    printf("Received data!");
    return 0;
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

    int err;
    static dtrace_hdl_t * dtrace_hdl;
    struct ps_prochandle *process;

    if ((dtrace_hdl = dtrace_open(DTRACE_VERSION, 0, &err)) != 0)
    {
        printf("cannot init dtrace");
        exit(-1);
    }
    
    pid_t pid = fork();

    if (pid == 0) {
        printf("I'm a child!");
        // TODO : Start to execute the code object with PyEval..
        PyArena_Free(arena);
    } else 
    {
        printf("Opened dtrace, attaching to %d", pid);

        process = dtrace_proc_grab(dtrace_hdl, 0, 0);
        printf("Attached process");

        // I'm going to prison for this...
        int im_done = 0;
        do {
            dtrace_sleep(dtrace_hdl);

            if (im_done) {
                    if (dtrace_stop(dtrace_hdl) == -1)
                            exit(-1);    
            }
            // Start PyEval... (&co) etc.
            switch (dtrace_work(dtrace_hdl, stdout, NULL, rec, NULL)) {
                case DTRACE_WORKSTATUS_DONE:
                        im_done = 1;     
                        break;
                case DTRACE_WORKSTATUS_OKAY:
                        break;
                default:
                        exit(-1);
            }  
        } while (!im_done);

        // Sweet Release..
        dtrace_proc_release(dtrace_hdl, process);
        wait(0);
     }
    return 0;
}