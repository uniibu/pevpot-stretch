PevPot Stretch
--------------

High performance hash stretch for [pevpot's provably fair lottery](https://www.pevpot.com/provably-fair)!


This repository includes both a node library and a stand-alone command line tool.

For the node library just grab with npm:


`npm install pevpot-stretch`

and use

```
var stetch = require('pevpot-stetch');
stetch(
  "00000000000000000ef86b27c174df6a412c0ce43eab1d532034555749294137",
  100000,
  function(err, data) {
    console.log('Stretch callback was called with: ', err, data);
  }
);
```

And the simple utility to stretch a blockhash for the [pevpot lottery](https://www.pevpot.com/provably-fair). Based on openssl
with an iteration counter changed from `int` to `long` to support 64 bit iterations

Compiling
---------

Linux: `gcc -O3 stretch.c -o stretch -lssl -lcrypto`

OSX: `clang -O3 stretch.c -o stretch -O3 -g -Wall -I/usr/local/Cellar/openssl/1.0.2d_1/include/ -L/usr/local/Cellar/openssl/1.0.2d_1/lib -lcrypto`


Usage
----

`./stetch blockhash [iterations]`

(If iterations is not provided, default to the pevpot 5 billion)



Test Vectors:
-------------
`./stetch 00000000000000000ef86b27c174df6a412c0ce43eab1d532034555749294137 100000`

39aed3d2eff372916100106e355384495d1794320d4c3cb4c2e0e48cc36bbd06

`./stretch 000000000000000009b7fb236187f120a0c86eb8785f099a8d197dd34b9d2553`

6e466cdd13cc80b1137addf46362bbe3714fc9bf7faef9aba930554d3e080ba5





Credits
-------
* OpenSSL  (original code)
* Dooglus [original extraction](https://bitcointalk.org/index.php?topic=1243970.msg12997448#msg12997448)
