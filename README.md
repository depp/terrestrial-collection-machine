# Terrestrial Collection Machine

Terrestrial Collection Machine (TCM) is a demo for the [Demosplash 2019][demosplash] demo party, which takes place on November 8-9, 2019 in Pittsburgh, PA. Yes, that doesn’t give us much time.

[demosplash]: http://www.demosplash.org/

## Building

Required software for building:

- [Bazel 1.0](https://bazel.build/)

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

- [GLFW 3.3](https://www.glfw.org/)

To build, run:

```shell
bazel build //tcm
```

This will create the executable `bazel-bin/tcm/tcm`.

### Homebrew

Install the prerequisites with the following commands:

```shell
brew cask install homebrew/cask-versions/adoptopenjdk8
brew install bazel pkg-config glfw3
```

### Developing

While developing, it is recommended to build with warnings enabled and treated as errors, with the flag `--define=warnings=error`. The three supported values for `warnings` are off, on, and error.

Instead of passing this flag manually, you can add it to a file named `.user.bazelrc`:

```
build --define=warnings=error
```

You can compile with the address sanitizer using `–config=asan`.

### Debugging on macOS

As a workaround for [Bazel issue #6327](https://github.com/bazelbuild/bazel/issues/6327) which affects debugging on macOS, add the following line to a file named `.user.bazelrc`:

```
build --strategy_regexp=^Linking=local
```

This fixes a problem where programs are not created with correct debugging information. This happens because the linker on macOS will not embed the debug information in the executable, it will instead embed absolute paths to object files containing the debug information. If the executable is linked in a sandbox, the paths will reference files in the sandbox, but the sandbox is destroyed after linking. Choosing strategy `local` links without a sandbox and the correct paths are embedded.

### Tips

Tip for Googlers: You may want to `alias blaze=bazel` in your profile, or `alias bazel=blaze` in your profile at work.

## Authors

- Dietrich Epp ([Twitter: @DietrichEpp](https://twitter.com/DietrichEpp))

## License

TCM is distributed under the terms of the MIT License. See [LICENSE.txt](LICENSE.txt) for details.

## Contributing

If you join the team, add yourself to the list of authors and add yourself to the copyright in LICENSE.txt.
