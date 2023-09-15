# 2048

<p align="center">
  <img src="https://raw.githubusercontent.com/sten-code/2048/master/board.png" width="300">
  <p align="center">
    2048 in the Terminal
  </p>
</p>

<h1 align="center">About</h1>

This project contains an implemented version of the game 2048 in Python for `PC`, `Casio fx-CG50` and `TI-84 Plus CE-T Python Edition`

There is also a C version for the TI-84 Plus CE-T.


<h1 align="center">Usage (Python)</h1>

## Windows/Linux

```console
python 2048.py
```

Controls:

| Action | Key         |
| ------ | ----------- |
| Up     | Up Arrow    |
| Left   | Left Arrow  |
| Down   | Down Arrow  |
| Right  | Right Arrow |
| Reset  | R           |
| Exit   | Escape      |

## TI-84 Plus CE-T Python Edition

I have created this in Python, this means that you cannot use the standard TI-84 Plus CE-T for this. You will however be able to use the version implemented in C.

There aren't any colors because whenever I use color codes it makes it green and it doesn't use the hex value that I gave.

-   Open TI-Connect on your pc
-   Go to the second tab from the side-bar with the document icon
-   Connect your calculator with the Mini-USB cable to your pc
    -   Make sure your calculator is on
-   Select your calculator and drag the `ti2048.py` file in the list with all the other files and click send.
    -   Set the location to `RAM` (should be selected by default)

Now to run it

-   Press the `prgm` button
-   Select `Python App` and wait for a couple of seconds until it loads
-   Run the `TI2048` file and you're done

Controls:

| Action | Key |
| ------ | --- |
| Up     | 8   |
| Left   | 4   |
| Down   | 5   |
| Right  | 6   |
| Reset  | 0   |
| Exit   | 1   |

> After each input press `enter`

<h1 align="center">Usage (C)</h1>

This is only made for the TI-84 Plus CE-T.

- Download the `CEdev` toolchain from [here](https://ce-programming.github.io/toolchain/static/getting-started.html).
- Extract it into `C:\CEdev`
- Add `C:\CEdev\bin` to your environment variables. [Here is a guide](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/).
- Clone this repository and run `make` in the root folder.
- After that use TI-Connect to upload the `2048CE.8xp` file to your TI-84 Plus CE-T.
