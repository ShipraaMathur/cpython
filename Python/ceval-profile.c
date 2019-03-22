#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <dtrace.h>

#include "pyconfig.h"

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

static int
fatal(int exitcode, const char *format, const char *message)
{
    fprintf(stderr, format, message);
    exit(exitcode);
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
    PyObject *filename,*co,*bytes,*unicode;
    PyArena *arena;
    PyCompilerFlags flags;
    flags.cf_flags = 0;
    flags.cf_feature_version = PY_MINOR_VERSION;

    Py_Initialize();

    char* fn = "test.py";
    char* errors;

    //unicode = PyUnicode_FromWideChar("print(1)", -1);
    unicode = PyUnicode_DecodeASCII(input, strlen(input), &errors);
    if (unicode == NULL) {
        printf("Failed to get input command");
        exit(-1);
    }
    
    bytes = PyUnicode_AsUTF8String(unicode);
    Py_DECREF(unicode);
    char* str = PyBytes_AsString(bytes);
    co = Py_CompileStringFlags(str, fn, Py_eval_input, NULL);

    if (co == NULL) {
        exit(-1);
    }
    arena = PyArena_New();
    if (arena == NULL)
        exit(-1);

    int err;
    static dtrace_hdl_t * dtrace_hdl;
    struct ps_prochandle *process;
    int dtrace_flags = DTRACE_O_NODEV;
    dtrace_hdl = dtrace_open(DTRACE_VERSION, dtrace_flags, &err);
    if (dtrace_hdl == NULL){
        printf("cannot init dtrace %d", err);
        fatal(1,"ERROR1: opening dtrace: %s\n",dtrace_errmsg(NULL,err));
    }

    dtrace_setopt(dtrace_hdl, "zone", "cpython");
    dtrace_go(dtrace_hdl);

    // TODO : Start to execute the code object with PyEval..
    PyObject * globals = PyEval_GetGlobals();
    PyObject * locals = PyEval_GetLocals();

    // Construct frames..
    int ret = PyEval_EvalCode(co, globals, locals);

    PyArena_Free(arena);

//        // this doesn't really work at the moment.
//        printf("Opened dtrace, attaching to %d", 0);
//
//        process = dtrace_proc_grab(dtrace_hdl, 0, 0);
//        printf("Attached process");
//
//        // I'm going to prison for this...
//
//        do {
//            dtrace_sleep(dtrace_hdl);
//
//            if (im_done) {
//                    if (dtrace_stop(dtrace_hdl) == -1)
//                            exit(-1);
//            }
//            // Start PyEval... (&co) etc.
//            switch (dtrace_work(dtrace_hdl, stdout, NULL, rec, NULL)) {
//                case DTRACE_WORKSTATUS_DONE:
//                        im_done = 1;
//                        break;
//                case DTRACE_WORKSTATUS_OKAY:
//                        break;
//                default:
//                        exit(-1);
//            }
//        } while (!im_done);
//
//        // Sweet Release..
//        dtrace_proc_release(dtrace_hdl, process);
//        wait(0);

    return 0;
}