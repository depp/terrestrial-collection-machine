# Terrestrial Collection Machine

Terrestrial Collection Machine (TCM) is a demo for the [Demosplash 2019][demosplash] demo party, which takes place on November 8-9, 2019 in Pittsburgh, PA. Yes, that doesn’t give us much time.

[demosplash]: http://www.demosplash.org/

## Authors

- Dietrich Epp ([Twitter: @DietrichEpp](https://twitter.com/DietrichEpp))

## Building

Required software for building:

- [Bazel 1.0](https://bazel.build/) (should also work with more recent versions)

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

- [GLFW 3.3](https://www.glfw.org/)

- [GLEW 2.0](http://glew.sourceforge.net) (except on macOS)

To build, run:

```shell
bazel build //tcm:tcm
```

This will create the executable `bazel-bin/tcm/tcm`.

### Debian / Ubuntu

To install the prerequisites:

```shell
sudo apt install pkg-config libglfw3-dev libglew-dev
```

Bazel is available as a `.deb` package from the [Bazel releases](https://github.com/bazelbuild/bazel/releases) page.

### Homebrew

To install the prerequisites:

```shell
brew cask install homebrew/cask-versions/adoptopenjdk8
brew install bazel pkg-config glfw3
```

## License

TCM is distributed under the terms of the MIT License. See [LICENSE.txt](LICENSE.txt) for details.

## Contributing

See [HACKING.md](HACKING.md).
