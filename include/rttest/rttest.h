// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RTTEST_H_
#define RTTEST_H_

#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

// rttest can have one instance per thread!
struct rttest_params
{
  unsigned int iterations;
  struct timespec update_period;
  size_t sched_policy;
  int sched_priority;
  int lock_memory;
  size_t stack_size;

  char *filename;
};

struct rttest_results
{
  int min_latency;
  int max_latency;
  double mean_latency;
  double latency_stddev;

  int min_jitter;
  int max_jitter;
  double mean_jitter;
  double jitter_stddev;

  unsigned int minor_pagefaults;
  unsigned int major_pagefaults;
};

/// \brief Initialize rttest with arguments
/// \param[in] argc Size of argument vector
/// \param[out] argv Argument vector
/// \return Error code to propagate to main
int rttest_read_args(int argc, char** argv);

/// \brief Initialize rttest. Preallocate the sample buffer, store user
/// parameters, lock memory if necessary
/// Not real time safe.
/// \param[in] iterations How many iterations to spin for
/// \param[in] update_period Time interval representing the spin period
/// \param[in] sched_policy Scheduling policy, e.g. round robin, FIFO
/// \param[in] sched_priority The thread priority
/// \param[in] stack_size How many bytes to prefault when
/// rttest_prefault_stack() is called.
/// \param[in] filename Name of the file to save results to.
/// \return Error code to propagate to main
int rttest_init(unsigned int iterations, struct timespec update_period,
    size_t sched_policy, int sched_priority, int lock_memory, size_t stack_size,
    char *filename);

/// \brief Create a new rttest instance for a new thread.
/// The thread's parameters are based on the first thread that called rttest_init.
/// To be called directly after the user creates the thread.
/// \return Error code to propagate to main
int rttest_init_new_thread();

/// \brief Spin at the specified wakeup period for the specified number of
/// iterations.
/// \param[in] user_function Function pointer to execute on wakeup
/// \param[in] args Arguments to the function
/// \return Error code to propagate to main
int rttest_spin(void *(*user_function)(void *), void *args);

// TODO better signature for user function
/// \brief Spin at the specified wakeup period for the specified number of
/// iterations. rttest_spin will attempt to time the execution of user_function
/// according to update_period.
/// Call this after everything has been initialized.
/// \param[in] user_function Function pointer to execute on wakeup.
/// \param[in] args Arguments to the function
/// \param[in] update_period Update period (overrides param read in rttest_init)
/// \param[in] iterations Iterations (overrides param read in rttest_init)
/// \return Error code to propagate to main
int rttest_spin_period(void *(*user_function)(void *), void *args,
    const struct timespec *update_period, const unsigned int iterations);

/// \brief Schedule a task at an absolute time. The interrupt
/// executes the user function and compares walltime to the expected wakeup
/// time, recording the results in the sample buffer.
/// TODO: implement asynchronous scheduling/logging
/// \param[in] user_function Function pointer to execute on interrupt.
/// \param[in] absolute_wakeup Scheduled interrupt time.
/// \param[out] Error code to propagate to main function.
/// \return Error code to propagate to main
// int rttest_schedule_wakeup(void *(*user_function)(void *), void *args,
//    const struct timespec *absolute_wakeup);

/// \brief Lock currently paged memory using mlockall.
/// \return Error code to propagate to main
int rttest_lock_memory();

/// \brief Prefault the stack.
/// \param[in] stack_size The size of the stack
/// \return Error code to propagate to main
int rttest_prefault_stack_size(const size_t stack_size);

/// \brief Prefault the stack using default stack size.
/// \return Error code to propagate to main
int rttest_prefault_stack();

/// \brief Commit a pool of dynamic memory
/// \param[in] stack_size The size of the pool
/// \return Error code to propagate to main
int rttest_lock_and_prefault_dynamic(const size_t pool_size);

/// \brief Set the priority and scheduling policy for this thread (pthreads)
/// \param[in] sched_priority The scheduling priority. Max is 99.
/// \param[in] policy The scheduling policy (FIFO, Round Robin, etc.)
/// \return Error code to propagate to main
int rttest_set_sched_priority(const size_t sched_priority, const int policy);

/// \brief Set the priority and scheduling policy for this thread using
/// default parameters.
/// \return Error code to propagate to main
int rttest_set_thread_default_priority();

/// \brief Get rusage (pagefaults) and record in the sample buffer at a
/// particular iteration
/// \param[in] i Index at which to store the pagefault information.
/// \return Error code to propagate to main
int rttest_get_next_rusage(unsigned int i);

/// \brief Calculate statistics and fill the given results struct.
/// \param[in] results The results struct to fill with statistics.
/// \return Error code if results struct is NULL or if calculations invalid
int rttest_calculate_statistics(struct rttest_results *results);

/// \brief Write the sample buffer to a file.
/// \return Error code to propagate to main
int rttest_write_results();

/// \brief Write the sample buffer to a file.
/// \param[in] Filename to store the sample buffer; overrides default param.
/// \return Error code to propagate to main
int rttest_write_results_file(char *filename);

/// \brief Free memory and cleanup
/// \return Error code to propagate to main
int rttest_finish();

#ifdef __cplusplus
}
#endif

#endif
