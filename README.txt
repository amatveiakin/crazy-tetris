CrazyTetris
===========

A tetris-like game where several players act simultaneously trying to knock
down each other with bonuses that occasionally appear in the playing field.
See sample screenshot for examples of ways in which you can diversify your
opponent's life.

Authors: Andrey Matveyakin & Alexey Eremin.

We didn't use git when the game was written, so the history was restored from
the tarballs that we send each other to exchange patches. That's why commit
messages are so informative :-)


Information on how to build and launch
======================================

1. Install DirectX Software Development Kit (https://www.microsoft.com/en-us/download/details.aspx?id=6812).
   NB: if it installation fails with error 1023, uninstall all VS 2010 redistributables (x86 and 64), install DirectX SDK,
       install VS 2010 redistributables back (link to current version: https://www.microsoft.com/en-us/download/details.aspx?id=26999)
2. Use Visual Studio 2013 to build.
3. Working directory should be set to repository root directory.