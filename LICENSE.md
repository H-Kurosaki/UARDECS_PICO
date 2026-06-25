Copyright 2024 Ken-ichiro Yasuba,Hideto Kurosaki

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## Bundled third-party code: src/_MyEthernet2/

The MIT License above applies to the UARDECS_PICO library itself.

It does **not** apply to the code under the `src/_MyEthernet2/` directory.
That directory contains a modified (forked) copy of the **Ethernet2** library
(https://github.com/adafruit/Ethernet2), adapted for the Raspberry Pi Pico.
The bundled source files have been renamed and their include paths adjusted so
that the library can be self-contained, but they are derived from Ethernet2.

The code under `src/_MyEthernet2/` is therefore licensed under the terms of the
original Ethernet2 library, the **GNU Lesser General Public License (LGPL),
version 2.1 or (at your option) any later version**, not under the MIT License.

    Ethernet2
    Copyright (c) 2009-2016 Arduino LLC. All rights reserved.

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

For the full text of the LGPL v2.1, see https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html

