//
// Copyright (c) 2003 Timothy A. Seufert (tas@mindspring.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

//
// $Id: workqueue.h,v 1.1 2011/07/12 18:47:37 acarr Exp $
//


#ifndef __WORKQUEUE_H__
#define __WORKQUEUE_H__


#include <pthread.h>


//
// Queue element class.  This is what gets passed between producer and consumer
// threads.  Users should subclass it to add a pointer to a data buffer, etc.
//
class qelem;


//
// Thread-safe producer / consumer work queue controller.
// All public methods EXCEPT constructor and destructor are thread-safe.
//
// A producer thread should call get() to obtain a queue element from the free list
// and, after doing its work, should then call enqueue() to place it on the work queue.
//
// The consumer thread should call dequeue() to obtain a queued element and, after
// doing its work, should then call release() to return it to the free list.
//
// Note: The free list starts out empty.  Therefore at least one thread must prime
// the pump by injecting at least one queue element via release() before any work
// can happen.
//
class workqueue {
public:
	workqueue();
	~workqueue();

	void enqueue(qelem* item);
	qelem* dequeue(void);

	void release(qelem* item);
	qelem* get(void);

protected:
	pthread_mutex_t mutex;
	pthread_cond_t queue_cond;
	pthread_cond_t freelist_cond;

	qelem* queue;
	qelem* freelist;
};


class qelem {
public:
	qelem() {}
	virtual ~qelem() {}

	friend void workqueue::enqueue(qelem* item);
	friend qelem* workqueue::dequeue(void);
	friend void workqueue::release(qelem* item);
	friend qelem* workqueue::get(void);

protected:
	qelem *next;
};


#endif
