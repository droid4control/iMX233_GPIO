/*
 * untitled.c
 * 
 * Copyright 2013 Stefan Mavrodiev <support@olimex.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */


#include "Python.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_BASE 0x80018000
#define GPIO_WRITE_PIN(gpio,value) GPIO_WRITE((gpio)>>5, (gpio)&31, value)
#define GPIO_WRITE(bank,pin,value) (gpio_mmap[0x140+((bank)<<2)+((value)?1:2)] = 1<<(pin))
#define GPIO_READ_PIN(gpio) GPIO_READ((gpio)>>5, (gpio)&31)
#define GPIO_READ(bank,pin) ((gpio_mmap[0x180+((bank)<<2)] >> (pin)) & 1)

static PyObject *SetupException;
static PyObject *OutputException;
static PyObject *InputException;


struct _gpio {
    uint8_t bank;
    uint8_t mux;
    char name[10];
    };
    
struct _gpio pins[] = {
    {0, 0, "PIN9"},
    {0, 1, "PIN10"},
    {0, 2, "PIN11"},
    {0, 3, "PIN12"},
    {0, 4, "PIN13"},
    {0, 5, "PIN14"},
    {0, 6, "PIN15"},
    {0, 7, "PIN16"},
    {0, 16, "PIN17"},
    {0, 17, "PIN18"},
    {1, 18, "PIN23"},
    {1, 20, "PIN24"},
    {1, 19, "PIN25"},
    {1, 21, "PIN26"},    
    {1, 28, "PIN27"},
    {0, 25, "PIN28"},
    {0, 23, "PIN29"},
    {2, 27, "PIN31"}    
    };


int *gpio_mmap = 0;

int *gpio_map() {
	int fd;
    
	if (gpio_mmap != 0)
        return;
        
	fd = open("/dev/mem", O_RDWR);
    
	if( fd < 0 )
        fd = 0;
        
	gpio_mmap = mmap(0, 0xfff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    
	if((int)gpio_mmap == -1)
		gpio_mmap = 0;
        
	close(fd);
    
    fd = 0;

}

int gpio_rd(long offset) {
	return gpio_mmap[offset/4];
}

void gpio_wr(long offset, long value) {
	gpio_mmap[offset/4] = value;
}

void gpio_output(int bank, int pin) {
	gpio_mmap[0x1C1 + (bank*4)] = 1 << pin;
}

void gpio_input(int bank, int pin) {
	gpio_mmap[0x1C2 + (bank*4)] = 1 << pin;
}



static int module_setup(void) {
    gpio_map();
    Py_RETURN_NONE;
}

static PyObject* py_setoutput(PyObject* self, PyObject* args) {
    int gpio;

    if(!PyArg_ParseTuple(args, "i", &gpio))
        Py_RETURN_NONE;
    
    gpio_output(pins[gpio].bank, pins[gpio].mux);

    Py_RETURN_NONE;
}
static PyObject* py_setinput(PyObject* self, PyObject* args) {
    int gpio;

    if(!PyArg_ParseTuple(args, "i", &gpio))
        Py_RETURN_NONE;

    gpio_input(pins[gpio].bank, pins[gpio].mux);

    Py_RETURN_NONE;
}
static PyObject* py_output(PyObject* self, PyObject* args) {
    int gpio;
    int value;
    
    if(!PyArg_ParseTuple(args, "ii", &gpio, &value))
        Py_RETURN_NONE;
        
    if(value != 0 && value != 1) {
        PyErr_SetString(OutputException, "Invalid output state");
        Py_RETURN_NONE;
    }
    GPIO_WRITE(pins[gpio].bank, pins[gpio].mux, value);
    
    Py_RETURN_NONE;
}

static PyObject* py_input(PyObject* self, PyObject* args) {
    int gpio;
    int result;
    
    if(!PyArg_ParseTuple(args, "i", &gpio))
        Py_RETURN_NONE;
        
    result = GPIO_READ(pins[gpio].bank, pins[gpio].mux);
    
    return Py_BuildValue("i", result);
}


static PyObject* py_init(PyObject* self, PyObject* args) {

    module_setup();

    Py_RETURN_NONE;
}


PyMethodDef module_methods[] = {
    {"init", py_init, METH_NOARGS, "Initialize module"},
    {"setinput", py_setinput, METH_VARARGS, "Set input directions."},
    {"setoutput", py_setoutput, METH_VARARGS, "Set output directions."},
    {"output", py_output, METH_VARARGS, "Set output state"},
    {"input", py_input, METH_VARARGS, "Get input state"},
    {NULL, NULL, 0, NULL}
};
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "iMX233 module",
    NULL,
    -1,
    module_methods
};
#endif
PyMODINIT_FUNC initiMX233_GPIO(void) {
    PyObject* module = NULL;


#if PY_MAJOR_VERSION >= 3
    module = PyModule_Create(&module_methods);
#else
    module = Py_InitModule("iMX233_GPIO", module_methods);
#endif


    if(module == NULL)
#if PY_MAJOR_VERSION >= 3
        return module;
#else
        return;
#endif

    SetupException = PyErr_NewException("pyiMX233.SetupException", NULL, NULL);
    PyModule_AddObject(module, "SetupException", SetupException);
    OutputException = PyErr_NewException("pyiMX233.OutputException", NULL, NULL);
    PyModule_AddObject(module, "OutputException", OutputException);
    InputException = PyErr_NewException("pyiMX233.InputException", NULL, NULL);
    PyModule_AddObject(module, "InputException", InputException);

   
    PyModule_AddObject(module, "PIN9", Py_BuildValue("i", 0));
    PyModule_AddObject(module, "PIN10", Py_BuildValue("i", 1));
    PyModule_AddObject(module, "PIN11", Py_BuildValue("i", 2));
    PyModule_AddObject(module, "PIN12", Py_BuildValue("i", 3));
    PyModule_AddObject(module, "PIN13", Py_BuildValue("i", 4));
    PyModule_AddObject(module, "PIN14", Py_BuildValue("i", 5));
    PyModule_AddObject(module, "PIN15", Py_BuildValue("i", 6));
    PyModule_AddObject(module, "PIN16", Py_BuildValue("i", 7));
    PyModule_AddObject(module, "PIN17", Py_BuildValue("i", 8));
    PyModule_AddObject(module, "PIN18", Py_BuildValue("i", 9));
    PyModule_AddObject(module, "PIN24", Py_BuildValue("i", 10));
    PyModule_AddObject(module, "PIN25", Py_BuildValue("i", 11));
    PyModule_AddObject(module, "PIN26", Py_BuildValue("i", 12));
    PyModule_AddObject(module, "PIN27", Py_BuildValue("i", 13));
    PyModule_AddObject(module, "PIN28", Py_BuildValue("i", 14));
    PyModule_AddObject(module, "PIN29", Py_BuildValue("i", 15));
    PyModule_AddObject(module, "PIN31", Py_BuildValue("i", 16));

}
