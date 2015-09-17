/*
	FileName:      ptmultithread.cpp

	Created Time:  2015-07-13

	Auther:        Cao Jiayin

	Email:         soraytrace@hotmail.com

	Location:      China, Shanghai

	Description:   SORT is short for Simple Open-source Ray Tracing. Anyone could checkout the source code from
				'sourceforge', https://soraytrace.svn.sourceforge.net/svnroot/soraytrace. And anyone is free to
				modify or publish the source code. It's cross platform. You could compile the source code in
				linux and windows , g++ or visual studio 2008 is required.
*/

#include "ptmultithread.h"

#if defined(SORT_IN_MAC)

#include "taskqueue.h"
#include "managers/memmanager.h"

// LTS data
Thread_Local int g_ThreadId = 0;

// get the thread id
int ThreadId()
{
	return g_ThreadId;
}

// thread mutex
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_key_t RenderThreadMac::m_threadKey;
pthread_once_t RenderThreadMac::m_threadKeyInit;

static void* RenderThread_Mac_Run(void* lpParameter)
{
	RenderThreadMac* renderThreadMac = (RenderThreadMac*)lpParameter;

	// setup lts
	g_ThreadId = renderThreadMac->GetThreadID();

	// run the thread
	renderThreadMac->RunThread();

	// end the thread
	renderThreadMac->EndThread();

	return 0;
}

RenderThreadMac::RenderThreadMac(unsigned tid)
{
	m_thread_id = tid;
	m_finished = false;
}

RenderThreadMac::RenderThreadMac()
{
}

void RenderThreadMac::BeginThread()
{
	pthread_create(&m_thread, 0, RenderThread_Mac_Run, this);
}

void RenderThreadMac::EndThread()
{
	// delete the integrator
	SAFE_DELETE(m_pIntegrator);

	// the thread is finished
	m_finished = true;
}

// Run the thread
void RenderThreadMac::RunThread()
{
	while (true)
	{
		// lock mutex
		pthread_mutex_lock(&g_mutex);

		if (RenderTaskQueue::GetSingleton().IsEmpty())
		{
			pthread_mutex_unlock(&g_mutex);
			break;
		}
		// Get a new task from the task queue
		RenderTask task = RenderTaskQueue::GetSingleton().PopTask();

		// release the mutex
		pthread_mutex_unlock(&g_mutex);

		// execute the task
		task.Execute(m_pIntegrator);

		// Destroy the task
		RenderTask::DestoryRenderTask(task);
	}
}

#endif