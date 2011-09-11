/*
 * =====================================================================================
 *
 *       Filename:  py-redis.c
 *
 *    Description:  python binding for redis using hiredis c api.
 *
 *        Version:  1.0
 *        Created:  09/09/2011 08:01:12 PM
 *       Revision:  r1 
 *       Compiler:  gcc
 *
 *         Author:  Fu Haiping (haipingf@gmail.com)
 *        Company:  ICT
 *
 * =====================================================================================
 */
#include <Python.h>
#include <structmember.h>
#include "hiredis.h"


#if !defined(_XDECREF)
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#else
#undef _XDECREF
#define _XDECREF(ptr) do { if ((ptr) != NULL) free((ptr));} while(0)
#endif

#define REDIS_DEFINE_KVBUF(buf) const char * s_##buf = NULL; size_t i_##buf

PyTypeObject RedisContext_Type;

typedef struct {
	PyObject_HEAD
	redisContext *_rContext;
} RedisContext;

static void RedisContext_dealloc(RedisContext *self)
{
	Py_BEGIN_ALLOW_THREADS
	redisFree(self->_rContext);
	_XDECREF(self->_rContext);
	Py_END_ALLOW_THREADS

	self->_rContext = NULL;
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *RedisContext_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	RedisContext *self = (RedisContext *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->_rContext = NULL;
	}
	return (PyObject *)self;
}

static int RedisContext_init(RedisContext *self, PyObject *args, PyObject *kwds)
{
	if (self->_rContext) {
		Py_BEGIN_ALLOW_THREADS
		redisFree(self->_rContext);
		_XDECREF(self->_rContext);
		Py_END_ALLOW_THREADS

		self->_rContext = NULL;
	}
	
	const char *ip = NULL;
	int port = -1;
	const char *kwargs[] = {"ip", "port", 0};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"si", (char**)kwargs, &ip, &port))
		return -1;
	self->_rContext = redisConnect(ip, port);
	if (self->_rContext->err) {
		fprintf(stderr, "Connection error: %s\n", self->_rContext->errstr);
		exit(-1);
	}
	return 0;
}

static PyObject *RedisContext_SET(RedisContext *self, PyObject *args, PyObject *kwds)
{
	const char *kwargs[] = {"key", "value", 0};
	redisReply *reply = NULL;
	REDIS_DEFINE_KVBUF(key);
	REDIS_DEFINE_KVBUF(value);

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#s#", (char**)kwargs, &s_key, &i_key, &s_value, &i_value)) {
		return -1;
	}

	reply = redisCommand(self->_rContext, "SET %b %b", s_key, i_key, s_value, i_value);
	fprintf(stderr, "SET: %s\n", reply->str);
	freeReplyObject(reply);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *RedisContext_GET(RedisContext *self, PyObject *args, PyObject *kwds)
{
	const char *kwargs[] = {"key", 0};
	redisReply *reply = NULL;

	PyObject *result;
	REDIS_DEFINE_KVBUF(key);

	if (!PyArg_ParseTupleAndKeywords(args, kwds, (char*)"s#", (char**)kwargs, &s_key, &i_key)) {
		return -1;
	}

	reply = redisCommand(self->_rContext, "GET %s", s_key);
	fprintf(stderr, "GET: %s\n", reply->str);
	result = PyString_FromStringAndSize(reply->str, strlen(reply->str));

	freeReplyObject(reply);
	return result;
}
static PyMethodDef RedisContext_methods[] = {
	{(char*)"SET",    (PyCFunction)RedisContext_SET,    METH_VARARGS, (char*)"set key-value" },
	{(char*)"GET", (PyCFunction)RedisContext_GET, METH_VARARGS, (char*)"get value of key" },
	{NULL}
};

PyTypeObject RedisContext_Type= {
	PyObject_HEAD_INIT(NULL)
	0,                             /*ob_size*/
	(char*)"redis.RedisContext",      /*tp_name*/
	sizeof(RedisContext),             /*tp_basicsize*/
	0,                             /*tp_itemsize*/
	(destructor)RedisContext_dealloc, /*tp_dealloc*/
	0,                             /*tp_print*/
	0,                             /*tp_getattr*/
	0,                             /*tp_setattr*/
	0,                             /*tp_compare*/
	0,                             /*tp_repr*/
	0,                             /*tp_as_number*/
	0,                             /*tp_as_sequence*/
	0,                             /*tp_as_mapping*/
	0,                             /*tp_hash */
	0,                             /*tp_call*/
	0,                             /*tp_str*/
	0,                             /*tp_getattro*/
	0,                             /*tp_setattro*/
	0,                             /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,            /*tp_flags*/
	(char*)"redis python bindings",   /*tp_doc */
	0,                             /*tp_traverse */
	0,                             /*tp_clear */
	0,                             /*tp_richcompare */
	0,                             /*tp_weaklistoffset */
	0,                             /*tp_iter */
	0,                             /*tp_iternext */
	RedisContext_methods,             /*tp_methods */
	0,                             /*tp_members */
	0,                             /*tp_getset */
	0,                             /*tp_base */
	0,                             /*tp_dict */
	0,                             /*tp_descr_get */
	0,                             /*tp_descr_set */
	0,                             /*tp_dictoffset */
	(initproc)RedisContext_init,      /*tp_init */
	0,                             /*tp_alloc */
	RedisContext_new,                 /*tp_new */
};


PyMODINIT_FUNC
initpyredis(void)
{
	PyObject* redis_module = Py_InitModule3((char*)"pyredis", RedisContext_methods, 0);

	if (redis_module == 0)
		return;

	if (PyType_Ready(&RedisContext_Type) < 0)
		return;

	Py_INCREF(&RedisContext_Type);

	if (PyModule_AddObject(redis_module, (char*)"Redis", (PyObject*)&RedisContext_Type) != 0)
		return;
}

