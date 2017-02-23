# Aardvark Library

A collection of single-header libraries for C implementing common data 
structures and useful utility functions. The table below is a rundown of all
the files provided and their fucntion.

| File                     | Docs                   | Description                                    |
| ---                      | ---                    | ---                                            |
| (bstree.h)[src/bstree.h] | (Docs)[docs/bstree.md] | Intrusive binary search tree                   |
| (hash.h)[src/hash.h]     | (Docs)[docs/hash.md]   | Intrusive hash table                           | 
| (ini.h)[src/ini.h]       | (Docs)[docs/ini.md]    | INI file parser                                |   
| (lex.h)[src/lex.h]       | (Docs)[docs/lex.md]    | Lexical analysis routines                      |   
| (list.h)[src/list.h]     | (Docs)[docs/list.md]   | Intrusive doubly-linked list                   |  
| (parse.h)[src/parse.h]   | (Docs)[docs/parse.md]  | LL(k) parser utility functions                 | 
| (slist.h)[src/slist.h]   | (Docs)[docs/slist.md]  | Intrusive singly-linked list                   | 
| (stdc.h)[src/stdc.h]     | (Docs)[docs/stdc.md]   | Common includes and helpers for writing ANSI C |  
| (strbuf.h)[src/strbuf.h] | (Docs)[docs/strbuf.md] | String buffer implementation                   |
| (utf8.h)[src/utf8.h]     | (Docs)[docs/utf8.md]   | UTF-8 encoding/decoding routines               |  
| (vec.h)[src/vec.h]       | (Docs)[docs/vec.md]    | Generic vector implementation                  |

## License

All files provided are distributed under the [ISC license](LICENSE.md) 
unless explicitly state otherwise.

## Installation and Usage

Simply copy the desired header(s) into the include path for the target project.
Modify at will. If you have any useful tweaks or bug fixes to contribute back, 
feel free to send me a pull request or a patch.