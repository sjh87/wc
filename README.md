This project's goal is to [clone wc](https://codingchallenges.fyi/challenges/challenge-wc), the classic Unix command-line tool, in C++.

This is just a small part of the greater goal of completing each challenge [here](https://codingchallenges.fyi/challenges/intro).

# To Run

Use flags to select what to count:
`w` = words
`c` = bytes
`m` = characters
`l` = lines

`FLAGS` is optional. By default, `w`, `l` and `c` are used, just like the OG `wc`.
```
$ INPUT_FILE="test.txt" FLAGS="" make
342190 test.txt
```

Okay. Good bye.
