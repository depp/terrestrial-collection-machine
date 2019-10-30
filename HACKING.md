# Hacking

If you join the team, add yourself to the list of authors in README.md and add yourself to the copyright in LICENSE.txt.

## Controls

- F12: Capture screenshot

## Build Targets

The main build targets are:

- `//tcm:tcm`—The release build, for distribution. This produces a self-contained binary which can run on other systems. OpenGL shaders are embedded directly into the program.

- `//dev:dev`—The development and testing build. This program is not portable and must be run from within the workspace. It will automatically reload OpenGL shaders from the filesystem as they change, so you can see the changes live.

## Build Options

Build options can be added to a file named `.user.bazelrc` in the repository root.

## Warnings

The build variable `warnings` changes whether warnings are enabled and whether they are treated as errors. It has three values:

- `off` (default): No additional compiler warnings.
- `on`: Recommended compiler warnings enabled.
- `error`: Recommended compiler warnings enabled, all warnings treated as errors.

It is recommended to set `warnings=error` when developing. This can be passed to Bazel as a flag:

```shell
bazel build --warnings=error <target>
```

Or it can be added to `.user.bazelrc`:

```
build --define=warnings=error
```

## Address Sanitizer

The Address Sanitizer is enabled with the option `–-config=asan`. This can also be added to `.user.bazelrc`:

```
build --config=asan
```

## Debugging on macOS

As a workaround for [Bazel issue #6327](https://github.com/bazelbuild/bazel/issues/6327) which affects debugging on macOS, add the following line to a file named `.user.bazelrc`:

```
build --strategy_regexp=^Linking=local
```

This fixes a problem where programs are not created with correct debugging information. This happens because the linker on macOS will not embed the debug information in the executable, it will instead embed absolute paths to object files containing the debug information. If the executable is linked in a sandbox, the paths will reference files in the sandbox, but the sandbox is destroyed after linking. Choosing strategy `local` links without a sandbox and the correct paths are embedded.

## Tips

Tip for Googlers: You may want to `alias blaze=bazel` in your profile, or `alias bazel=blaze` in your profile at work.
