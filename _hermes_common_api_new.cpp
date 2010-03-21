#include "_hermes_common_api_new.h"

PyObject *(*c2py_CooMatrix)(struct CooMatrix *);
PyObject *(*c2py_CSRMatrix)(struct CSRMatrix *);
PyObject *(*c2py_CSCMatrix)(struct CSCMatrix *);
PyObject *(*create_new_namespace)(void);
void (*cmd)(const char*);
void (*set_verbose_cmd)(int);
void (*insert_object)(const char*, PyObject *);
PyObject *(*get_object)(const char*);
PyObject *(*c2py_int)(int);
int (*py2c_int)(PyObject *);
char *(*py2c_str)(PyObject *);
double (*py2c_double)(PyObject *);
PyObject *(*c2numpy_int)(int *, int);
PyObject *(*c2numpy_int_inplace)(int *, int);
PyObject *(*c2numpy_double)(double *, int);
PyObject *(*c2numpy_double_inplace)(double *, int);
void (*numpy2c_int_inplace)(PyObject *, int **, int *);
void (*numpy2c_double_inplace)(PyObject *, double **, int *);
void (*run_cmd)(const char*, PyObject *);



PyObject *__Pyx_ImportModule(const char *name) {
    PyObject *py_name = 0;
    PyObject *py_module = 0;

    #if PY_MAJOR_VERSION < 3
    py_name = PyString_FromString(name);
    #else
    py_name = PyUnicode_FromString(name);
    #endif
    if (!py_name)
        goto bad;
    py_module = PyImport_Import(py_name);
    Py_DECREF(py_name);
    return py_module;
bad:
    Py_XDECREF(py_name);
    return 0;
}



int __Pyx_ImportFunction(PyObject *module, const char *funcname, void (**f)(void), const char *sig) {
    PyObject *d = 0;
    PyObject *cobj = 0;
    union {
        void (*fp)(void);
        void *p;
    } tmp;
#if PY_VERSION_HEX < 0x03010000
    const char *desc, *s1, *s2;
#endif

    d = PyObject_GetAttrString(module, (char *)"__pyx_capi__");
    if (!d)
        goto bad;
    cobj = PyDict_GetItemString(d, funcname);
    if (!cobj) {
        PyErr_Format(PyExc_ImportError,
            "%s does not export expected C function %s",
                PyModule_GetName(module), funcname);
        goto bad;
    }
#if PY_VERSION_HEX < 0x03010000
    desc = (const char *)PyCObject_GetDesc(cobj);
    if (!desc)
        goto bad;
    s1 = desc; s2 = sig;
    while (*s1 != '\0' && *s1 == *s2) { s1++; s2++; }
    if (*s1 != *s2) {
        PyErr_Format(PyExc_TypeError,
            "C function %s.%s has wrong signature (expected %s, got %s)",
             PyModule_GetName(module), funcname, sig, desc);
        goto bad;
    }
    tmp.p = PyCObject_AsVoidPtr(cobj);
#else
    if (!PyCapsule_IsValid(cobj, sig)) {
        PyErr_Format(PyExc_TypeError,
            "C function %s.%s has wrong signature (expected %s, got %s)",
             PyModule_GetName(module), funcname, sig, PyCapsule_GetName(cobj));
        goto bad;
    }
    tmp.p = PyCapsule_GetPointer(cobj, sig);
#endif
    *f = tmp.fp;
    if (!(*f))
        goto bad;
    Py_DECREF(d);
    return 0;
bad:
    Py_XDECREF(d);
    return -1;
}

int import__hermes_common(void) {
  PyObject *module = 0;
  module = __Pyx_ImportModule("_hermes_common");
  if (!module) goto bad;
  if (__Pyx_ImportFunction(module, "c2py_CooMatrix", (void (**)(void))&c2py_CooMatrix, "PyObject *(struct CooMatrix *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2py_CSRMatrix", (void (**)(void))&c2py_CSRMatrix, "PyObject *(struct CSRMatrix *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2py_CSCMatrix", (void (**)(void))&c2py_CSCMatrix, "PyObject *(struct CSCMatrix *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "create_new_namespace", (void (**)(void))&create_new_namespace, "PyObject *(void)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "cmd", (void (**)(void))&cmd, "void (const char*)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "set_verbose_cmd", (void (**)(void))&set_verbose_cmd, "void (int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "insert_object", (void (**)(void))&insert_object, "void (const char*, PyObject *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "get_object", (void (**)(void))&get_object, "PyObject *(const char*)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2py_int", (void (**)(void))&c2py_int, "PyObject *(int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "py2c_int", (void (**)(void))&py2c_int, "int (PyObject *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "py2c_str", (void (**)(void))&py2c_str, "char *(PyObject *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "py2c_double", (void (**)(void))&py2c_double, "double (PyObject *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2numpy_int", (void (**)(void))&c2numpy_int, "PyObject *(int *, int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2numpy_int_inplace", (void (**)(void))&c2numpy_int_inplace, "PyObject *(int *, int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2numpy_double", (void (**)(void))&c2numpy_double, "PyObject *(double *, int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "c2numpy_double_inplace", (void (**)(void))&c2numpy_double_inplace, "PyObject *(double *, int)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "numpy2c_int_inplace", (void (**)(void))&numpy2c_int_inplace, "void (PyObject *, int **, int *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "numpy2c_double_inplace", (void (**)(void))&numpy2c_double_inplace, "void (PyObject *, double **, int *)") < 0) goto bad;
  if (__Pyx_ImportFunction(module, "run_cmd", (void (**)(void))&run_cmd, "void (const char*, PyObject *)") < 0) goto bad;
  Py_DECREF(module); module = 0;
  return 0;
  bad:
  Py_XDECREF(module);
  return -1;
}

