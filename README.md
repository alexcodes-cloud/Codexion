*This project has been created as part of the 42 curriculum by yel-azim.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers table problem, reimagined in a co-working setting. Multiple coders sit around a shared table, each needing two USB dongles simultaneously to compile their quantum code. They cycle through three phases — compiling, debugging, and refactoring — and must compete fairly for a limited number of shared dongles without deadlocking, starving, or burning out.

---

## Instructions

### Compilation

```bash
make
```

This produces the `codexion` binary.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads (and dongles) |
| `time_to_burnout` | Milliseconds before a coder burns out without compiling |
| `time_to_compile` | Milliseconds spent compiling |
| `time_to_debug` | Milliseconds spent debugging |
| `time_to_refactor` | Milliseconds spent refactoring |
| `number_of_compiles_required` | Simulation stops when all coders reach this count |
| `dongle_cooldown` | Milliseconds a dongle is unavailable after being released |
| `scheduler` | `fifo` or `edf` |

### Example

```bash
./codexion 4 800 200 200 200 5 0 fifo
```

### Sample output

```
0 3 has taken a dongle
0 3 has taken a dongle
0 3 is compiling
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
200 3 is debugging
200 4 has taken a dongle
200 1 is debugging
200 4 has taken a dongle
200 4 is compiling
200 2 has taken a dongle
200 2 has taken a dongle
200 2 is compiling
```

---

## Blocking Cases Handled

### Deadlock prevention (Coffman's conditions)

The classic deadlock scenario occurs when every coder picks up their left dongle and waits for their right dongle simultaneously — a circular wait. This is broken by **asymmetric acquisition order**: even-numbered coders pick up left then right, odd-numbered coders pick up right then left. This ensures the circular dependency can never form.

### Starvation prevention

Under EDF scheduling, coders whose burnout deadline is soonest are served first, which inherently prevents starvation as long as parameters are feasible. Under FIFO, arrival order is preserved so no coder waits indefinitely past others who arrived later. The slot queue (max 2 per dongle) ensures bounded waiting, and condition variable broadcasts wake all waiters when a dongle is released, so no thread sleeps forever.

### Cooldown handling

After a dongle is released, a `last_release_time` timestamp is recorded. Before a coder marks a dongle as `in_use`, it checks whether the cooldown window has elapsed and sleeps the remaining time with `usleep`. This is done outside the dongle mutex to avoid blocking other threads during the wait.

### Precise burnout detection

A dedicated monitor thread polls all coders every 10 microseconds. For each coder, it compares the current time against `last_compile_time + time_to_burnout` — where `last_compile_time` is initialized to the simulation start time and updated at the beginning of every compile. This means the burnout clock runs continuously through all phases: debug, refactor, and waiting for dongles. If a new compile does not start within `time_to_burnout` milliseconds since the last one, burnout fires regardless of what phase the coder is in. When burnout is detected, `is_over` is set atomically, all condition variables are broadcast to unblock waiting threads, and the burnout message is printed — all within the required 10 ms window.

### Log serialization

All output is protected by a single `write_lock` mutex. State messages are only printed if `is_over` is false at the time of printing (with the exception of the `burned out` message itself), preventing interleaved or post-termination log noise.

---

## Thread Synchronization Mechanisms

### `pthread_mutex_t`

Three categories of mutexes are used:

- **`write_lock`** — a single global mutex serializing all `printf` calls so log lines
never interleave.
- **`is_over_mutex`** — protects the shared `is_over` flag, accessed by every thread
and the monitor. `get_is_over()` and `set_is_over()` wrap all access, ensuring no
torn reads or writes.
- **`coder_mutex`** (one per coder) — protects per-coder state: `last_compile_time`
and `compile_counter`. The monitor reads these under lock to avoid data races when
checking burnout conditions.
- **`dongle->mutex`** (one per dongle) — protects `in_use`, `last_release_time`,
`slot_count`, and `slots[]`. All queue operations (`add_to_queue`, `remove_from_queue`)
and dongle state transitions are performed under this lock.

### `pthread_cond_t`

Each dongle has an associated condition variable (`dongle->cond`). Coders call `pthread_cond_wait` inside `wait_dongle()` when the dongle is in use or they are not at the front of the queue. When a dongle is released, `release_dongle()` calls `pthread_cond_broadcast` to wake all waiters, who then re-evaluate the condition under the lock.

The monitor also calls `pthread_cond_broadcast` on all dongle condition variables when the simulation ends, ensuring no coder thread sleeps past termination.

### Race condition prevention

- `last_compile_time` is updated at the very start of each compile and initialized
to the simulation start time, always under `coder_mutex`. This ensures the monitor
never reads a partial or inconsistent timestamp when checking burnout.
- The `is_over` check is performed at multiple points in the coder cycle (before
and after acquiring each dongle, before compiling) so threads exit promptly without
performing further actions on stale state.
- The cooldown sleep (`usleep`) is performed **outside** the dongle mutex to avoid
blocking the entire queue during the wait period.

### Thread-safe communication between coders and the monitor

Coders never communicate directly with each other or the monitor — they only modify their own state under `coder_mutex`. The monitor reads coder state under the same lock and writes to `is_over` under `is_over_mutex`. This clean separation ensures there are no hidden dependencies between threads beyond the shared dongle queues and the termination flag.

---

## Resources

### Classic references

- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- `man pthread_mutex_init`, `man pthread_cond_wait`, `man gettimeofday`
- [Learning threads](https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2)
- 

### AI usage

- creating test cases for the project and reviewing the result for each test
- helping to create the readme file
