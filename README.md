<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# beman.execution: Building Block For Asynchronous Programs

<img src="https://github.com/bemanproject/beman/blob/main/images/logos/beman_logo-beman_library_under_development.png" style="width:5%; height:auto;">

`beman.execution` provides the basic vocabulary for asynchronous
programming as well as important algorithms implemented in terms
of this vocabulary. The key entities of the vocabulary are:

- `scheduler` used to control where operations are executed.
    - The key operation is `schedule(scheduler) -> sender`.
- `sender` used to represent work.
    - The key operation is `connect(sender, receiver) -> operation-state`.
- `receiver` used to receive completion notifications.
    - There are multiple operations on receivers:
        - `set_value(receiver, args...)` for successful completions.
        - `set_error(receiver, error)` to report errors.
        - `set_stopped(receiver)` to report cancellation.
- `operation-state` to represent a ready to run work graph .
    - The key operation is `start(state)`.

Using these operations some fundamental algorithms are implemented,
e.g.:

- `just(args...)` as the starting point of a work graph without a
    specified scheduler`.
- `let_value(sender, fun)` to produce a sender based on `sender`'s
    results.
- `on(scheduler, sender)` to execute `sender` on `scheduler`.
- `transfer(sender, scheduler)` to complete with with `sender`'s
    results on `scheduler`.
- `when_all(sender ...)` to complete when all `sender`s have
    completed.
- `bulk(...)` to executed execute work, potentially concurrently.

**Implements:** [`std::execution` (P2300R10)](http://wg21.link/P2300R10).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#under-development-and-not-yet-ready-for-production-use)

## Help Welcome!

There are plenty of things which need to be done. Some of these
are listed in the [issues section](https://github.com/bemanproject/execution/issues).
Other contributions are, of course, also welcome. Aside from contributing
code the project welcomes reports on usage experience, code reviews, defect
reports, features requests, etc. There are plenty contribution opportunities.

If you want to contribute, you won't be left alone! Please e-mail
[Dietmar Kühl](mailto:dietmar.kuehl@me.com) if you want help or mentoring
for contributing to the library.

## Build

| Library | Linux | MacOS | Windows |
| ------- | ----- | ----- | ------- |
| build | ![Linux build status](https://github.com/bemanproject/execution/actions/workflows/linux.yml/badge.svg) | ![MacOS build status](https://github.com/bemanproject/execution/actions/workflows/macos.yml/badge.svg) | ![Window build status](https://github.com/bemanproject/execution/actions/workflows/windows.yml/badge.svg) |

The following instructions build the library and the examples:

    cmake --workflow --list-presets
    Available workflow presets:

      "debug"
      "release"

    cmake --workflow --preset release

The implementation compiles and passes tests using [clang](https://clang.llvm.org/),
[gcc](http://gcc.gnu.org), and [MSVC++](https://visualstudio.microsoft.com/vs/features/cplusplus/).

## Examples

- `<stop_token>` example: [Compiler Explorer](https://godbolt.org/z/4r4x9q1r7)
