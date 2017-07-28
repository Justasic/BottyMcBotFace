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
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVEdR
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "TimerReactor.h"
#include "ThreadEngine.h"
#include <thread>

TimerReactor::TimerReactor()
{
    this->thrd = std::thread(&TimerReactor::ExecuteTimers, this);
}

TimerReactor::~TimerReactor()
{
    this->quitting = true;
    // Wait to join.
    this->thrd.join();
}

void TimerReactor::ExecuteTimers()
{
    while (!quitting)
    {
        // Get the ThreadEngine handle.
        extern ThreadHandler *thread;
        // Get the current time we're testing for.
        auto now = std::chrono::steady_clock::now();
        // Lock so we can't have shit added to the list while we iterate/modify it
        this->mtex.lock();
        // This variable is used to get the next sleep duratuon
        // from the timers. This is more efficient than iterating
        // the list below every milisecond. Instead we start running
        // the second the next timer needs to tick.
        std::chrono::steady_clock::time_point sleepduration = this->timers.empty() ? now + 1ms : this->timers.front().next;

        for (auto it = this->timers.begin(); it != this->timers.end(); ++it)
        {
            struct _timer &t = *it;
            // if we're equal to now or we've exceeded the timepoint requested, execute.
            // if we're not executing, compare to sleepduration and set it if we're shorter than the previous value
            // else we just continue.
            if (now >= t.next)
            {
                // Ok so this call is complicated for a few reasons:
                // 1. we must maintain the fastest timer system possible
                // 2. we still have to set the fact that the timer may be repeating or not
                // 3. idk.
                // So. We call AddQueue with a lambda function and execute the timer in the thread engine
                // then check if it's a repeatable timer.
                thread->AddQueue([this, &t, it]() -> void {
                        bool repeat = t.callback();
                        if (!repeat)
                        {
                            std::unique_lock<std::mutex> lock(this->mtex);
                            this->timers.erase(it);
                        }
                        else
                        {
                            // we use steady_clock::now() instead of above definition of now so we can
                            // get more accurate recall time.
                            auto next = std::chrono::steady_clock::now() + t.calltime;
                            t.next = next;
                        }
               }); // end of tread->AddQueue();
            }
            else if (t.next < sleepduration)
                sleepduration = t.next;
        }
        // we're done!
        this->mtex.unlock();

        now = std::chrono::steady_clock::now();
        // Make sure a timer doesn't try and execute the thread faster than we mandate.
        if (sleepduration < now + 1ms)
            sleepduration = now + 1ms;

        // Sleep the thread for sleepduration.
        // This is more efficient than running the thread
        // every milisecond.
        std::this_thread::sleep_until(sleepduration);
    }
}
