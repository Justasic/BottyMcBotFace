/*************************************************************************
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Justin Crawford
 * Copyright (c) 2017, William Haugen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <thread>

// Move the chrono literals to the global namespace.
using namespace std::chrono_literals;

typedef std::function<bool()> timercallback_t;

// Class: TimerReactor
//
// Description:
// This class is designed to be a high-resolution timer
// class which can run at timepoints regardless of how
// system time changes. While most events will use
// system time instead of steady time, this timer is
// capable of doing steady events that should not be
// affected by real-world time (like ban expirations).
//
// NOTE: Functions run outside of main thread and
// therefore must be thread-safe! Timers are ran in
// mini-second intervals.
//
// How to use:
//
// Function definitions should be as follows:
//		bool mytimerfunc(...) { ... }
//		auto start = std::chrono::steady_clock::now() + 5min;
// then call TimerReactor::AddTimer(mytimerfunc, start, ...)
// and the timer will be called 5 minutes from now.
// If you want the timer to terminate, return false.
// if you want it to repeat, return true.
class TimerReactor
{
	struct _timer
	{
		timercallback_t callback;
        // NOTICE/TODO: These are not thread-safe!
        // Time requested (in seconds or whatever) to know how long until timer should be executed.
        std::chrono::steady_clock::duration calltime;
        // Time used to know when the next call is (calculated by this class)
        std::chrono::steady_clock::time_point next;
	};

    // our timers.
	std::vector<struct _timer> timers;
    
    // mutex so we block in the thread.
    std::mutex mtex;
    // the thread we run in.
    std::thread thrd;
    // If we're quitting terminate the thread
    std::atomic<bool> quitting;

public:
	TimerReactor();
	~TimerReactor();

	template<class _Function, class... Args>
	void AddTimer(_Function&& __f, std::chrono::steady_clock::duration calltime, Args&&... __args)
	{
		struct _timer timer;
		timer.callback = std::bind(std::forward<_Function>(__f), std::forward<Args>(__args)...);
		timer.calltime = calltime;
        timer.next = std::chrono::steady_clock::now() + calltime;
		this->timers.push_back(timer);
	}

protected:
	// MUST be run in it's own thread as it is a blocking call
	// and must be accurate.
	void ExecuteTimers();
};
