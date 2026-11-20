/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/

/**
 * @file      core_portme.c
 * @authors   Protik Banerji <protik09@gmail.com>
 * @copyright Protik Banerji, 2021
 *
 * @brief Coremark port for the RP2040 and RP2350
 */

#include <stdlib.h>
#include "coremark.h"
#include "core_portme.h"
#include <time.h>
#if LIB_PICO_STDIO_USB
#include "pico/stdio_usb.h"
#endif

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif

#define ITERATIONS 0
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;
/* Porting : Timing functions
        How to capture time and convert to seconds must be ported to whatever is
   supported by the platform. e.g. Read value from on board RTC, read value from
   cpu clock cycles performance counter etc. Sample implementation for standard
   time.h and windows.h definitions included.
*/
CORETIMETYPE barebones_clock()
{
    #ifndef NDEBUG
    return (CORETIMETYPE)to_us_since_boot(get_absolute_time());
    #else
    CORETIMETYPE t = to_us_since_boot(get_absolute_time());
    return t;
    #endif
}
/* Define : TIMER_RES_DIVIDER
        Divider to trade off timer resolution and total time that can be
   measured.

        Use lower values to increase resolution, but make sure that overflow
   does not occur. If there are issues with the return value overflowing,
   increase this value.
        */
#define CLOCKS_PER_SEC 1000000.0
#define GETMYTIME(_t) (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini) ((fin) - (ini))
#define TIMER_RES_DIVIDER 1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC (CLOCKS_PER_SEC / TIMER_RES_DIVIDER)
/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
        This function will be called right before starting the timed portion of
   the benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or zeroing some system parameters - e.g. setting the cpu clocks
   cycles to 0.
*/
void start_time(void)
{
    GETMYTIME(&start_time_val);
}
/* Function : stop_time
        This function will be called right after ending the timed portion of the
   benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or other system parameters - e.g. reading the current value of
   cpu cycles counter.
*/
void stop_time(void)
{
    GETMYTIME(&stop_time_val);
}
/* Function : get_time
        Return an abstract "ticks" number that signifies time on the system.

        Actual value returned may be cpu cycles, milliseconds or any other
   value, as long as it can be converted to seconds by <time_in_secs>. This
   methodology is taken to accomodate any hardware or simulated platform. The
   sample implementation returns millisecs by default, and the resolution is
   controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void)
{
    CORE_TICKS elapsed
        = (CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
    return elapsed;
}
/* Function : time_in_secs
        Convert the value returned by get_time to seconds.

        The <secs_ret> type is used to accomodate systems with no support for
   floating point. Default implementation implemented by the EE_TICKS_PER_SEC
   macro above.
*/
secs_ret time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    return retval;
}

/* Run plan : multicore runs (RP2040 and RP2350)
        CoreMark's main() is run once for every entry below. Each run is a
   complete, unmodified CoreMark run (calibration, timed run, validation and
   report). Only the number of contexts and the core each context runs on
   differ, which is the job the template gives to the porting layer.
*/
typedef enum
{
    RUN_CORE0 = 0,
    RUN_CORE1,
    RUN_BOTH_CORES,
    NUM_RUNS
} port_run_id;

typedef struct
{
    const char *name;
    ee_u32      num_contexts;                /* becomes default_num_contexts */
    ee_u8       core_of_context[MULTITHREAD]; /* core that runs context i */
} port_run;

static const port_run port_runs[NUM_RUNS] = {
    [RUN_CORE0]      = { "Core 0 only", 1, { 0, 0 } },
    [RUN_CORE1]      = { "Core 1 only", 1, { 1, 0 } },
    [RUN_BOTH_CORES] = { "Core 0 + Core 1", 2, { 0, 1 } },
};

ee_u32 default_num_contexts = 1;

static ee_u32        current_run;
static ee_u32        next_context;   /* contexts dispatched so far this run */
static ee_u32        run_iterations; /* iterations summed over all contexts */
static core_results *core0_pending;  /* core 0 context not yet executed */
static CORE_TICKS    run_ticks[NUM_RUNS];
static ee_u32        run_total_iterations[NUM_RUNS];

/* Function : portable_malloc
        Provide a memory block for a context (MEM_METHOD == MEM_MALLOC).
        Called by core 0 only, before any context starts running.
*/
void *portable_malloc(ee_size_t size)
{
    return malloc(size);
}

/* Function : portable_free
        Release a block from <portable_malloc>.
*/
void portable_free(void *p)
{
    free(p);
}

/* Function : portable_init
        Target specific initialization code
        Test for some common mistakes.

        Called by CoreMark's main() at the start of every run. The one-time
   board setup lives in __wrap_main().
*/
void portable_init(core_portable *p, int *argc, char *argv[])
{
    const port_run *run = &port_runs[current_run];

    (void)argc;
    (void)argv;

    ee_printf("\n========================================\n");
    ee_printf("Run %lu of %d: %s\n",
              (unsigned long)(current_run + 1),
              (int)NUM_RUNS,
              run->name);
    ee_printf("========================================\n");

    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        ee_printf(
            "ERROR! Please define ee_ptr_int to a type that holds a "
            "pointer!\n");
    }
    if (sizeof(ee_u32) != 4)
    {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }

    default_num_contexts = run->num_contexts;
    next_context         = 0;
    run_iterations       = 0;
    core0_pending        = NULL;
    p->portable_id       = 1;
}

/* Function : portable_fini
        Target specific final code

        Called by CoreMark's main() after it has printed its report. The last
   start_time()/stop_time() pair is the timed benchmark, so get_time() is the
   same "Total ticks" value that the report shows.
*/
void portable_fini(core_portable *p)
{
    run_ticks[current_run]            = get_time();
    run_total_iterations[current_run] = run_iterations;
    p->portable_id                    = 0;
}

/* Function : core1_worker
        Entry point of core 1. It runs for the whole session. It takes a
   context (a core_results pointer) from the inter-core FIFO, runs it, and
   sends the same pointer back when it has finished.
*/
static void core1_worker(void)
{
    for (;;)
    {
        core_results *res = (core_results *)multicore_fifo_pop_blocking();
        iterate(res);
        multicore_fifo_push_blocking((ee_ptr_int)res);
    }
}

/* Function : core_start_parallel
        Start a context. CoreMark calls this for every context after
   start_time().

        A context for core 1 is sent to core 1 and starts at once. A context for
   core 0 is only recorded here. Core 0 is the caller, so running it now
   would stop core 0 from starting the other contexts until it finished. It
   runs in <core_stop_parallel> instead.
*/
ee_u8 core_start_parallel(core_results *res)
{
    ee_u8 core = port_runs[current_run].core_of_context[next_context++];

    res->port.core = core;
    run_iterations += res->iterations;

    if (core == 1)
    {
        multicore_fifo_push_blocking((ee_ptr_int)res);
    }
    else
    {
        core0_pending = res;
    }
    return 0;
}

/* Function : core_stop_parallel
        Wait for a context to finish. CoreMark calls this for every context
   before stop_time().

        Core 0 first runs its own context, if it has one, while core 1 works on
   its context. Then, if this context belongs to core 1, core 0 waits for
   core 1 to report it finished.
*/
ee_u8 core_stop_parallel(core_results *res)
{
    if (core0_pending != NULL)
    {
        iterate(core0_pending);
        core0_pending = NULL;
    }
    if (res->port.core == 1)
    {
        if ((core_results *)multicore_fifo_pop_blocking() != res)
        {
            ee_printf("ERROR! Core 1 returned an unexpected context!\n");
            return 1;
        }
    }
    return 0;
}

/* Function : print_chip_info
        Chip, core type and system clock. CMakeLists.txt runs each chip at its
   fastest officially supported clock (200 MHz on the RP2040, 150 MHz on the
   RP2350), so Iterations/Sec from different chips are only comparable per
   MHz.
*/
static void print_chip_info(void)
{
    ee_printf("Chip: %s, system clock %lu MHz\n",
              PORT_CHIP_NAME,
              (unsigned long)(clock_get_hz(clk_sys) / 1000000u));
}

/* Function : print_summary
        One line per run with the result from that run's report, for easy
   comparison. Whether a run validated is shown in its own report.
*/
static void print_summary(void)
{
    ee_u32 i;
#if HAS_FLOAT
    secs_ret mhz = (secs_ret)clock_get_hz(clk_sys) / 1000000.0;
#endif

    ee_printf("\n========================================\n");
    ee_printf("CoreMark summary\n");
    print_chip_info();
    ee_printf("========================================\n");
    for (i = 0; i < NUM_RUNS; i++)
    {
#if HAS_FLOAT
        secs_ret secs = time_in_secs(run_ticks[i]);
        secs_ret ips  = (secs > 0) ? run_total_iterations[i] / secs : 0;
        ee_printf("%-16s: %f Iterations/Sec, %f CoreMark/MHz (%lu iterations, %f secs)\n",
                  port_runs[i].name,
                  ips,
                  ips / mhz,
                  (unsigned long)run_total_iterations[i],
                  secs);
#else
        ee_printf("%-16s: %lu iterations, %lu ticks\n",
                  port_runs[i].name,
                  (unsigned long)run_total_iterations[i],
                  (unsigned long)run_ticks[i]);
#endif
    }
#if HAS_FLOAT
    {
        secs_ret single = time_in_secs(run_ticks[RUN_CORE0]);
        secs_ret both   = time_in_secs(run_ticks[RUN_BOTH_CORES]);
        if ((single > 0) && (both > 0)
            && (run_total_iterations[RUN_CORE0] > 0))
        {
            ee_printf("Dual core speed-up vs Core 0 only: %fx\n",
                      (run_total_iterations[RUN_BOTH_CORES] / both)
                          / (run_total_iterations[RUN_CORE0] / single));
        }
    }
#endif
    ee_printf("See each run's report above for validation.\n");
}

/* Function : __wrap_main
        Entry point. CMakeLists.txt links with --wrap=main, so the SDK
   start-up code calls this function, and __real_main is CoreMark's own
   main() in core_main.c, unmodified. Calling main() once per run plan entry
   is the on-target equivalent of running coremark.exe several times with a
   different "M<n>" argument on the reference POSIX port.
*/
#if !MAIN_HAS_NOARGC
#error "__wrap_main expects CoreMark's main() to take no arguments."
#endif
MAIN_RETURN_TYPE __real_main(void);

int __wrap_main(void)
{
    // Board init (RP2040 or RP2350) with a nice welcome message
    stdio_init_all();
#if LIB_PICO_STDIO_USB
    // USB CDC output is discarded until a terminal opens the port, so wait
    // for one before printing anything, or the banner and prompt are lost.
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
#endif
    ee_printf("CoreMark Performance Benchmark\n");
    print_chip_info();
    ee_printf("\n");
    ee_printf("CoreMark measures how quickly your processor can manage linked\n");
    ee_printf("lists, compute matrix multiply, and execute state machine code.\n\n");
    ee_printf("Iterations/Sec is the main benchmark result, higher numbers are better.\n\n");
    ee_printf("CoreMark runs %d times: Core 0 only, Core 1 only, then both cores\n",
              (int)NUM_RUNS);
    ee_printf("together. Each run usually takes 12 to 20 seconds.\n\n");
    ee_printf("Press any key to start the benchmark...\n");
    getchar(); // PAUSE FOR HUMAN INPUT

    multicore_launch_core1(core1_worker);

    for (current_run = 0; current_run < NUM_RUNS; current_run++)
    {
        __real_main();
    }

    print_summary();
    ee_printf("\n\nProgram Ended\n");

    #ifndef PICO_DEFAULT_LED_PIN
    #warning This program requires a board with a regular LED. Replace this line with your own stuff.
    while (true)
    {
        tight_loop_contents();
    }
    #else
    // initialise GPIO (Green LED connected to pin 25)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1); // Set pin 25 to high
        sleep_ms(1000); // 1s delay

        gpio_put(PICO_DEFAULT_LED_PIN, 0); // Set pin 25 to low
        sleep_ms(1000); // 1s delay
    }
    #endif
}
