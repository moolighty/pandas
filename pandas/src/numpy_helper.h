#include "Python.h"
#include "numpy/arrayobject.h"
#include "numpy/arrayscalars.h"

#ifndef PANDAS_INLINE
  #if defined(__GNUC__)
    #define PANDAS_INLINE __inline__
  #elif defined(_MSC_VER)
    #define PANDAS_INLINE __inline
  #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define PANDAS_INLINE inline
  #else
    #define PANDAS_INLINE
  #endif
#endif

#define PANDAS_FLOAT 0
#define PANDAS_INT 1
#define PANDAS_BOOL 2
#define PANDAS_STRING 3
#define PANDAS_OBJECT 4
#define PANDAS_DATETIME 5

PANDAS_INLINE int
infer_type(PyObject* obj) {
  if (PyBool_Check(obj)) {
    return PANDAS_BOOL;
  }
  else if (PyArray_IsIntegerScalar(obj)) {
    return PANDAS_INT;
  }
  else if (PyArray_IsScalar(obj, Datetime)) {
    return PANDAS_DATETIME;
  }
  else if (PyFloat_Check(obj) || PyArray_IsScalar(obj, Floating)) {
    return PANDAS_FLOAT;
  }
  else if (PyString_Check(obj) || PyUnicode_Check(obj)) {
    return PANDAS_STRING;
  }
  else {
    return PANDAS_OBJECT;
  }
}

PANDAS_INLINE npy_int64
get_nat(void) {
  return NPY_MIN_INT64;
}

PANDAS_INLINE npy_datetime
get_datetime64_value(PyObject* obj) {
  return ((PyDatetimeScalarObject*) obj)->obval;

}


PANDAS_INLINE int
is_integer_object(PyObject* obj) {
  return (!PyBool_Check(obj)) && PyArray_IsIntegerScalar(obj);
//  return PyArray_IsIntegerScalar(obj);
}

PANDAS_INLINE int
is_float_object(PyObject* obj) {
  return (PyFloat_Check(obj) || PyArray_IsScalar(obj, Floating));
}
PANDAS_INLINE int
is_complex_object(PyObject* obj) {
  return (PyComplex_Check(obj) || PyArray_IsScalar(obj, ComplexFloating));
}

PANDAS_INLINE int
is_bool_object(PyObject* obj) {
  return (PyBool_Check(obj) || PyArray_IsScalar(obj, Bool));
}

PANDAS_INLINE int
is_string_object(PyObject* obj) {
  return (PyString_Check(obj) || PyUnicode_Check(obj));
}

PANDAS_INLINE int
is_datetime64_object(PyObject *obj) {
  return PyArray_IsScalar(obj, Datetime);
}

PANDAS_INLINE int
assign_value_1d(PyArrayObject* ap, Py_ssize_t _i, PyObject* v) {
  npy_intp i = (npy_intp) _i;
  char *item = (char *) PyArray_DATA(ap) + i * PyArray_STRIDE(ap, 0);
  return PyArray_DESCR(ap)->f->setitem(v, item, ap);
}

PANDAS_INLINE PyObject*
get_value_1d(PyArrayObject* ap, Py_ssize_t i) {
  char *item = (char *) PyArray_DATA(ap) + i * PyArray_STRIDE(ap, 0);
  return PyArray_Scalar(item, PyArray_DESCR(ap), (PyObject*) ap);
}


PANDAS_INLINE char*
get_c_string(PyObject* obj) {
#if PY_VERSION_HEX >= 0x03000000
  PyObject* enc_str = PyUnicode_AsEncodedString(obj, "utf-8", "error");

  char *ret;
  ret = PyBytes_AS_STRING(enc_str);

  // TODO: memory leak here

  // Py_XDECREF(enc_str);
  return ret;
#else
  return PyString_AsString(obj);
#endif
}

PANDAS_INLINE PyObject*
char_to_string(char* data) {
#if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_FromString(data);
#else
    return PyString_FromString(data);
#endif
}

// PANDAS_INLINE int
// is_string(PyObject* obj) {
// #if PY_VERSION_HEX >= 0x03000000
//   return PyUnicode_Check(obj);
// #else
//   return PyString_Check(obj);
// #endif

PANDAS_INLINE PyObject* floatify(PyObject* str) {

#if PY_VERSION_HEX >= 0x03000000
  return PyFloat_FromString(str);
#else
  return PyFloat_FromString(str, NULL);
#endif

}


void set_array_owndata(PyArrayObject *ao) {
    ao->flags |= NPY_OWNDATA;
}


// PANDAS_INLINE PyObject*
// get_base_ndarray(PyObject* ap) {
//   // if (!ap || (NULL == ap)) {
//   //   Py_RETURN_NONE;
//   // }

//   while (!PyArray_CheckExact(ap)) {
//     ap = PyArray_BASE((PyArrayObject*) ap);
//     if (ap == Py_None) Py_RETURN_NONE;
//   }
//   // PyArray_BASE is a borrowed reference
//   if(ap) {
//     Py_INCREF(ap);
//   }
//   return ap;
// }

