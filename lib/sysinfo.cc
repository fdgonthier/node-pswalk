/*
* Copyright (C) 2014-2016, Opersys inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <node.h>
#include <unistd.h>
#include "psutils.h"

using namespace v8;

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

#define NEW_SYMBOL(I, S) String::NewFromUtf8(I, S, String::kInternalizedString)

void MemInfo(const FunctionCallbackInfo<Value>& args) {
    Isolate* i = args.GetIsolate();
	Handle<Object> jmem;
	const char *err;

	if (!mem_info(&err))
		i->ThrowException(Exception::TypeError(String::NewFromUtf8(i, err)));

	jmem = Object::New(i);
	jmem->Set(NEW_SYMBOL(i, "memTotal"), Local<Value>::New(i, Number::New(i, mem.memTotal * 1024L)));
	jmem->Set(NEW_SYMBOL(i, "memFree"), Local<Value>::New(i, Number::New(i, mem.memFree * 1024L)));
	jmem->Set(NEW_SYMBOL(i, "buffers"), Local<Value>::New(i, Number::New(i, mem.buffers * 1024L)));
	jmem->Set(NEW_SYMBOL(i, "cached"), Local<Value>::New(i, Number::New(i, mem.cached * 1024L)));
	jmem->Set(NEW_SYMBOL(i, "swapTotal"), Local<Value>::New(i, Number::New(i, mem.swapTotal * 1024L)));
	jmem->Set(NEW_SYMBOL(i, "swapFree"), Local<Value>::New(i, Number::New(i, mem.swapFree * 1024L)));

	args.GetReturnValue().Set(jmem);
}

void CpuInfo(const FunctionCallbackInfo<Value>& args) {
    Isolate* iso = args.GetIsolate();
	Handle<Object> jcpuinfo, jgcpu, jcpus;
	const char *err;

	if (!cpu_info(&err))
		iso->ThrowException(Exception::TypeError(String::NewFromUtf8(iso, err)));

	jgcpu = Object::New(iso);
	jgcpu->Set(NEW_SYMBOL(iso, "utime"), Local<Value>::New(iso, Number::New(iso, global_cpu.utime)));
	jgcpu->Set(NEW_SYMBOL(iso, "ntime"), Local<Value>::New(iso, Number::New(iso, global_cpu.ntime)));
	jgcpu->Set(NEW_SYMBOL(iso, "stime"), Local<Value>::New(iso, Number::New(iso, global_cpu.stime)));
	jgcpu->Set(NEW_SYMBOL(iso, "itime"), Local<Value>::New(iso, Number::New(iso, global_cpu.itime)));
	jgcpu->Set(NEW_SYMBOL(iso, "iowtime"), Local<Value>::New(iso, Number::New(iso, global_cpu.iowtime)));
	jgcpu->Set(NEW_SYMBOL(iso, "irqtime"), Local<Value>::New(iso, Number::New(iso, global_cpu.irqtime)));
	jgcpu->Set(NEW_SYMBOL(iso, "sirqtime"), Local<Value>::New(iso, Number::New(iso, global_cpu.sirqtime)));
	jgcpu->Set(NEW_SYMBOL(iso, "ncpu"), Local<Value>::New(iso, Number::New(iso, nb_cpu)));

    jcpus = Array::New(iso);

    for (int i = 0; i < nb_cpu; i++) {
        Handle<Object> jcpu;

        jcpu = Object::New(iso);
        jcpu->Set(NEW_SYMBOL(iso, "no"), Local<Value>::New(iso, Number::New(iso, cpu[i].no)));
        jcpu->Set(NEW_SYMBOL(iso, "utime"), Local<Value>::New(iso, Number::New(iso, cpu[i].utime)));
        jcpu->Set(NEW_SYMBOL(iso, "ntime"), Local<Value>::New(iso, Number::New(iso, cpu[i].ntime)));
        jcpu->Set(NEW_SYMBOL(iso, "stime"), Local<Value>::New(iso, Number::New(iso, cpu[i].stime)));
        jcpu->Set(NEW_SYMBOL(iso, "itime"), Local<Value>::New(iso, Number::New(iso, cpu[i].itime)));
        jcpu->Set(NEW_SYMBOL(iso, "iowtime"), Local<Value>::New(iso, Number::New(iso, cpu[i].iowtime)));
        jcpu->Set(NEW_SYMBOL(iso, "irqtime"), Local<Value>::New(iso, Number::New(iso, cpu[i].irqtime)));
        jcpu->Set(NEW_SYMBOL(iso, "sirqtime"), Local<Value>::New(iso, Number::New(iso, cpu[i].sirqtime)));

        jcpus->Set(v8::Number::New(iso, i), jcpu);
    }

    jcpuinfo = Object::New(iso);
    jcpuinfo->Set(NEW_SYMBOL(iso, "global"), jgcpu);
    jcpuinfo->Set(NEW_SYMBOL(iso, "cpus"), jcpus);

	args.GetReturnValue().Set(jcpuinfo);
}

// Native process walker.
void ProcessWalk(const FunctionCallbackInfo<Value>& args) {
    Isolate* iso = args.GetIsolate();
  	Handle<Function> cb;
  	Handle<Value> argv[1];
  	Handle<Object> obj;
    const char *err;
    long unsigned time;

  	cb = Handle<Function>::Cast(args[0]);

    if (read_procs(&err)) {

        for (int i = 0; i < num_procs; i++) {
        	if (procs[i]) {
        	    obj = Object::New(iso);

        	    /* FIXME: CLOCK_TICKS are system dependent so calculate the time
        	       value in second here as a convenience for the interface. */
				time = (procs[i]->utime + procs[i]->stime) / sysconf(_SC_CLK_TCK);

          		obj->Set(NEW_SYMBOL(iso, "pid"), Local<Value>::New(iso, Number::New(iso, procs[i]->pid)));
			  	obj->Set(NEW_SYMBOL(iso, "ppid"), Local<Value>::New(iso, Number::New(iso, procs[i]->ppid)));
  				obj->Set(NEW_SYMBOL(iso, "name"), Local<Value>::New(iso, String::NewFromUtf8(iso, procs[i]->tname)));
  				obj->Set(NEW_SYMBOL(iso, "utime"), Local<Value>::New(iso, Number::New(iso, procs[i]->utime)));
  				obj->Set(NEW_SYMBOL(iso, "stime"), Local<Value>::New(iso, Number::New(iso, procs[i]->stime)));
  				obj->Set(NEW_SYMBOL(iso, "prio"), Local<Value>::New(iso, Number::New(iso, procs[i]->prio)));
  				obj->Set(NEW_SYMBOL(iso, "nice"), Local<Value>::New(iso, Number::New(iso, procs[i]->nice)));
  				obj->Set(NEW_SYMBOL(iso, "vss"), Local<Value>::New(iso, Number::New(iso, procs[i]->vss)));
  				obj->Set(NEW_SYMBOL(iso, "rss"), Local<Value>::New(iso, Number::New(iso, procs[i]->rss * getpagesize())));
  				obj->Set(NEW_SYMBOL(iso, "shm"), Local<Value>::New(iso, Number::New(iso, procs[i]->shm * getpagesize())));
  				obj->Set(NEW_SYMBOL(iso, "state"), Local<Value>::New(iso, String::NewFromUtf8(iso, &procs[i]->state)));
  	        	obj->Set(NEW_SYMBOL(iso, "cmdline"), Local<Value>::New(iso, String::NewFromUtf8(iso, procs[i]->cmdline)));
                obj->Set(NEW_SYMBOL(iso, "time"), Local<Value>::New(iso, Number::New(iso, time)));

            	argv[0] = obj;
           		cb->Call(Null(iso), 1, argv);
           	}
        }
    } else
        iso->ThrowException(Exception::TypeError(String::NewFromUtf8(iso, err)));
}

void Init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "pswalk", ProcessWalk);
    NODE_SET_METHOD(exports, "cpuinfo", CpuInfo);
    NODE_SET_METHOD(exports, "meminfo", MemInfo);
}

NODE_MODULE(pswalk, Init)
