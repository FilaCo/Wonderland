# Alice Language Specification <!-- omit from toc -->

- [Introduction](#introduction)
- [Notation](#notation)
- [Source code representation](#source-code-representation)
  - [Characters](#characters)
  - [Letters and digits](#letters-and-digits)
- [Lexical elements](#lexical-elements)
  - [Comments](#comments)
  - [Tokens](#tokens)
  - [Semicolons](#semicolons)
  - [Identifiers](#identifiers)
  - [Keywords](#keywords)
  - [Operators and punctuation](#operators-and-punctuation)
  - [Integer literals](#integer-literals)
  - [Floating-point literals](#floating-point-literals)
  - [Rune literals](#rune-literals)
  - [String literals](#string-literals)
  - [Byte literals](#byte-literals)
  - [Byte string literals](#byte-string-literals)
- [Constants](#constants)

## Introduction

This is the reference manual for the Alice programming language.

Alice is a domain specific language (a.k.a DSL) designed for game and realtime simulations development. It is based on the Entity-Component-System (a.k.a ECS) design pattern. Programs are constructed from unique "things" called **Entities** that are assigned groups of **Props** (a.k.a *components* in other ECS implementations), which are then processed using pipelines of **Queries**.

The core idea behind Alice is that restricting developers to only these three ECS primitives naturally leads to software with the following characteristics out of the box:

1. **Performance** — cache-friendly data layout and a concurrency-friendly execution model.
2. **Maintainability** — composition over inheritance makes code easy to extend and refactor.
3. **Simplicity** — no complex abstractions; the mental model stays small and approachable.

## Notation

The syntax is specified using a [variant](https://en.wikipedia.org/wiki/Wirth_syntax_notation) of Extended Backus-Naur Form (EBNF):

```ebnf
syntax      = { production } .
production  = ProductionName "=" [ expression ] "." .
expression  = term { "|" term } .
term        = factor { factor } .
factor      = ProductionName 
            | Token [ "…" Token ] 
            | group 
            | option 
            | repetition .

group       = "(" expression ")" .
option      = "[" expression "]" .
repetition  = "{" expression "}" .
```

Productions are expressions constructed from terms and the following operators, in increasing precedence:

```text
|   alternation
()  grouping
[]  option (0 or 1 times)
{}  repetition (0 to n times)
```

CamelCase production names are used to identify lexical (terminal) tokens. Non-terminals are in snake_case. Lexical tokens are enclosed in double quotes "" or back quotes ``.

The form `a … b` represents the set of characters from a through b as alternatives. The horizontal ellipsis `…` is also used elsewhere in the spec to informally denote various enumerations or code snippets that are not further specified. The character `…` is not a token of the Alice language.

## Source code representation

Source code is Unicode text encoded in [UTF-8](https://en.wikipedia.org/wiki/UTF-8). The text is not canonicalized, so a single accented code point is distinct from the same character constructed from combining an accent and a letter; those are treated as two code points. For simplicity, this document will use the unqualified term character to refer to a Unicode code point in the source text.

Each code point is distinct; for instance, uppercase and lowercase letters are different characters.

### Characters

The following terms are used to denote specific Unicode character categories:

```ebnf
NewLine       = /* the Unicode code point U+000A */ .
UnicodeChar   = /* an arbitrary Unicode code point except U+000A and U+000D */ .
UnicodeLetter = /* a Unicode code point categorized as "Letter" */ .
UnicodeDigit  = /* a Unicode code point categorized as "Number, decimal digit" */ .
```

In [The Unicode Standard 8.0](https://www.unicode.org/versions/Unicode8.0.0/), Section 4.5 "General Category" defines a set of character categories. Alice treats all characters in any of the Letter categories Lu, Ll, Lt, Lm, or Lo as Unicode letters, and those in the Number category Nd as Unicode digits.

### Letters and digits

The underscore character `_` (U+005F) is considered a lowercase letter.

```ebnf
Letter   = UnicodeLetter | "_" .
DecDigit = "0" … "9" .
BinDigit = "0" | "1" .
OctDigit = "0" … "7" .
HexDigit = "0" … "9" | "A" … "F" | "a" … "f" .
```

## Lexical elements

### Comments

Comments serve as program documentation. There are two forms:

1. Line comments start with the character sequence `//` and stop at the end of the line.
2. Block comments start with the character sequence `/*` and stop with the first subsequent character sequence `*/`.

A comment cannot start inside a rune, byte, string or byte string literal, or inside a comment. A block comment containing no newlines acts like a space. Any other comment acts like a NewLine.

### Tokens

Tokens form the vocabulary of the Alice language. There are four classes: identifiers, keywords, operators and punctuation, and literals. White space, formed from spaces (U+0020), horizontal tabs (U+0009), carriage returns (U+000D), and newlines (U+000A), is ignored except as it separates tokens that would otherwise combine into a single token. Also, a newline or end of file may trigger the insertion of a semicolon. While breaking the input into tokens, the next token is the longest sequence of characters that form a valid token.

### Semicolons

The formal syntax uses semicolons `;` as terminators in a number of productions. Alice programs may omit most of these semicolons using the following two rules:

1. When the input is broken into tokens, a semicolon is automatically inserted into the token stream immediately after a line's final token if that token is
    - an identifier
    - an integer, floating-point, imaginary, rune, or string literal
    - one of the operators and punctuation `)`, `]`, or `}`
2. To allow complex statements to occupy a single line, a semicolon may be omitted before a closing `)` or `}`.

To reflect idiomatic use, code examples in this document elide semicolons using these rules.

### Identifiers

Identifiers name program entities such as variables and types. An identifier is a sequence of one or more letters and digits. The first character in an identifier must be a letter.

```ebnf
Ident = Letter { Letter | UnicodeDigit } .
```

```text
a
_x9
αβ
```

### Keywords

The following keywords are reserved and may not be used as identifiers.

```text
const     mut         prop
despawn   namespace   query
filter    not         spawn
has       or          using
```

### Operators and punctuation

The following character sequences represent operators and punctuation:

```text
+   &   ==    !=    (   )
-   |   <     <=    [   ]
*   ^   >     >=    {   }
/   <<  =           ,   ;
%   >>  ~           .   :  
```

### Integer literals

An integer literal is a sequence of digits representing an integer constant. An optional prefix sets a non-decimal base: `0b` or `0B` for binary, `0`, `0o`, or `0O` for octal, and `0x` or `0X` for hexadecimal. A single `0` is considered a decimal zero. In hexadecimal literals, letters `a` through `f` and `A` through `F` represent values `10` through `15`.

For readability, an underscore character `_` may appear after a base prefix or between successive digits; such underscores do not change the literal's value.

```ebnf
DecDigits = DecDigit { [ "_" ] DecDigit } .
BinDigits = BinDigit { [ "_" ] BinDigit } .
OctDigits = OctDigit { [ "_" ] OctDigit } .
HexDigits = HexDigit { [ "_" ] HexDigit } .

DecLit    = "0" | ( "1" … "9" ) [ [ "_" ] DecDigits ] .
BinLit    = "0" ( "b" | "B" ) [ "_" ] BinDigits .
OctLit    = "0" [ "o" | "O" ] [ "_" ] OctDigits .
HexLit    = "0" ( "x" | "X" ) [ "_" ] HexDigits .

IntLit    = DecLit | BinLit | OctLit | HexLit .
```

```text
42
4_2
0600
0_600
0o600
0O600       // second character is capital letter 'O'
0xBadFace
0xBad_Face
0x_67_7a_2f_cc_40_c6
170141183460469231731687303715884105727
170_141183_460469_231731_687303_715884_105727

_42         // an identifier, not an integer literal
42_         // invalid: _ must separate successive digits
4__2        // invalid: only one _ at a time
0_xBadFace  // invalid: _ must separate successive digits
```

### Floating-point literals

A floating-point literal is a decimal or hexadecimal representation of a floating-point constant.

A decimal floating-point literal consists of an integer part (decimal digits), a decimal point, a fractional part (decimal digits), and an exponent part (`e` or `E` followed by an optional sign and decimal digits). One of the integer part or the fractional part may be elided; one of the decimal point or the exponent part may be elided. An exponent value exp scales the mantissa (integer and fractional part) by 10<sup>exp</sup>.

A hexadecimal floating-point literal consists of a `0x` or `0X` prefix, an integer part (hexadecimal digits), a radix point, a fractional part (hexadecimal digits), and an exponent part (`p` or `P` followed by an optional sign and decimal digits). One of the integer part or the fractional part may be elided; the radix point may be elided as well, but the exponent part is required. (This syntax matches the one given in IEEE 754-2008 §5.12.3.) An exponent value exp scales the mantissa (integer and fractional part) by 2<sup>exp</sup>.

For readability, an underscore character `_` may appear after a base prefix or between successive digits; such underscores do not change the literal value.

```ebnf
DecExponent = ( "e" | "E" ) [ "+" | "-" ] DecDigits .
DecFloatLit = DecDigits "." [ DecDigits ] [ DecExponent ]
            | DecDigits DecExponent
            | "." DecDigits [ DecExponent ] .

HexExponent = ( "p" | "P" ) [ "+" | "-" ] DecDigits .
HexMantissa = [ "_" ] HexDigits "." [ HexDigits ]
            | [ "_" ] HexDigits
            | "." HexDigits .

HexFloatLit = "0" ( "x" | "X" ) HexMantissa HexExponent .

FloatLit    = DecFloatLit | HexFloatLit .
```

```text
0.
72.40
072.40       // == 72.40
2.71828
1.e+0
6.67428e-11
1E6
.25
.12345E+5
1_5.         // == 15.0
0.15e+0_2    // == 15.0

0x1p-2       // == 0.25
0x2.p10      // == 2048.0
0x1.Fp+0     // == 1.9375
0X.8p-0      // == 0.5
0X_1FFFP-16  // == 0.1249847412109375
0x15e-2      // == 0x15e - 2 (integer subtraction)

0x.p1        // invalid: mantissa has no digits
1p-2         // invalid: p exponent requires hexadecimal mantissa
0x1.5e-2     // invalid: hexadecimal mantissa requires p exponent
1_.5         // invalid: _ must separate successive digits
1._5         // invalid: _ must separate successive digits
1.5_e1       // invalid: _ must separate successive digits
1.5e_1       // invalid: _ must separate successive digits
1.5e1_       // invalid: _ must separate successive digits
```

### Rune literals

A rune literal represents a rune constant, an integer value identifying a Unicode code point. A rune literal is expressed as one or more characters enclosed in single quotes, as in `'x'` or `'\n'`. Within the quotes, any character may appear except newline and unescaped single quote. A single quoted character represents the Unicode value of the character itself, while multi-character sequences beginning with a backslash encode values in various formats.

The simplest form represents the single character within the quotes; since Alice source text is Unicode characters encoded in UTF-8, multiple UTF-8-encoded bytes may represent a single integer value. For instance, the literal `'a'` holds a single byte representing a literal `a`, Unicode U+0061, value 0x61, while `'ä'` holds two bytes (0xc3 0xa4) representing a literal a-dieresis, U+00E4, value 0xe4.

Several backslash escapes allow arbitrary values to be encoded as ASCII text. There are four ways to represent the integer value as a numeric constant: `\x` followed by exactly two hexadecimal digits; `\u` followed by exactly four hexadecimal digits; `\U` followed by exactly eight hexadecimal digits, and a plain backslash `\` followed by exactly three octal digits. In each case the value of the literal is the value represented by the digits in the corresponding base.

Although these representations all result in an integer, they have different valid ranges. Octal escapes must represent a value between 0 and 255 inclusive. Hexadecimal escapes satisfy this condition by construction. The escapes `\u` and `\U` represent Unicode code points so within them some values are illegal, in particular those above `0x10FFFF` and surrogate halves.

After a backslash, certain single-character escapes represent special values:

```text
\a   U+0007 alert or bell
\b   U+0008 backspace
\f   U+000C form feed
\n   U+000A line feed or newline
\r   U+000D carriage return
\t   U+0009 horizontal tab
\v   U+000B vertical tab
\\   U+005C backslash
\'   U+0027 single quote  (valid escape only within rune literals)
\"   U+0022 double quote  (valid escape only within string literals)
```

An unrecognized character following a backslash in a rune literal is illegal.

```ebnf
EscapedChar = `\` ( "a" | "b" | "f" | "n" | "r" | "t" | "v" | `\` | "'" | `"` ) .

BigUValue   = `\` "U" HexDigit HexDigit HexDigit HexDigit
                    HexDigit HexDigit HexDigit HexDigit .
LittleUValue = `\` "u" HexDigit HexDigit HexDigit HexDigit .
HexByteValue = `\` "x" HexDigit HexDigit .
OctalByteValue = `\` OctalDigit OctalDigit OctalDigit .
ByteValue = OctalByteValue | HexByteValue .
UnicodeValue = UnicodeChar | LittleUValue | BigUValue | EscapedChar .

RuneLit = "'" ( UnicodeValue | ByteValue ) "'" .
```

```text
'a'
'ä'
'本'
'\t'
'\000'
'\007'
'\377'
'\x07'
'\xff'
'\u12e4'
'\U00101234'
'\''         // rune literal containing single quote character
'aa'         // illegal: too many characters
'\k'         // illegal: k is not recognized after a backslash
'\xa'        // illegal: too few hexadecimal digits
'\0'         // illegal: too few octal digits
'\400'       // illegal: octal value over 255
'\uDFFF'     // illegal: surrogate half
'\U00110000' // illegal: invalid Unicode code point
```

### String literals

A string literal represents a [string constant](#constants) obtained from concatenating a sequence of characters. There are two forms: raw string literals and interpreted string literals.

Raw string literals are character sequences between two sequences of three double quotes, as in `"""foo"""`. Within the quotes, any character may appear except another triple double quotes. The value of a raw string literal is the string composed of the uninterpreted (implicitly UTF-8-encoded) characters between the quotes; in particular, backslashes have no special meaning and the string may contain newlines. Carriage return characters (`\r`) inside raw string literals are discarded from the raw string value.

Interpreted string literals are character sequences between double quotes, as in `"bar"`. Within the quotes, any character may appear except newline and unescaped double quote. The text between the quotes forms the value of the literal, with backslash escapes interpreted as they are in rune literals (except that `\'` is illegal and `\"` is legal), with the same restrictions. String literals always contain valid UTF-8 encoded text; only Unicode-level escapes (`\u` and `\U`) are allowed to specify code points, not byte-level escapes.

```ebnf
RawStringLit         = `"""` { UnicodeChar | NewLine } `"""` .
InterpretedStringLit = `"` { UnicodeValue } `"` .

StringLit            = RawStringLit | InterpretedStringLit .
```

```text
"""abc"""            // same as "abc"
"""\n
\n"""                // same as "\\n\n\\n"
"\n"
"\""                 // same as """""""
"Hello, world!\n"
"日本語"
"\u65e5本\U00008a9e"
"\uD800"             // illegal: surrogate half
"\U00110000"         // illegal: invalid Unicode code point
```

These examples all represent the same string:

```text
"日本語"                         // UTF-8 input text
"""日本語"""                     // UTF-8 input text as a raw literal
"\u65e5\u672c\u8a9e"            // the explicit Unicode code points
"\U000065e5\U0000672c\U00008a9e" // the explicit Unicode code points
```

If the source code represents a character as two code points, such as a combining form involving an accent and a letter, the result will be an error if placed in a rune literal (it is not a single code point), and will appear as two code points if placed in a string literal.

### Byte literals

A byte literal represents a byte constant, an unsigned 8-bit integer value. A byte literal is expressed as the character `b` followed by a single character or escape sequence enclosed in single quotes, as in `b'x'` or `b'\n'`. Within the quotes, any character may appear except newline and unescaped single quote.

The simplest form represents the single byte value of the character within the quotes. For instance, the literal `b'a'` holds the value 0x61 (97 in decimal). Multi-byte UTF-8 sequences are not allowed in byte literals.

The same backslash escapes are available as in rune literals, but the result is always a single byte value. Octal escapes must represent a value between 0 and 255 inclusive. Hexadecimal escapes represent a single byte. The escapes `\u` and `\U` are not allowed in byte literals since they may represent values larger than 255 or multi-byte sequences.

```ebnf
ByteLit = "b" "'" ( UnicodeChar | HexByteValue | OctalByteValue | EscapedChar ) "'" .
```

```text
b'a'
b'~'
b'\n'
b'\x00'
b'\xff'
b'\255'
b'\''         // byte literal containing single quote character

b'aa'         // illegal: too many characters
b'ä'          // illegal: multi-byte UTF-8 sequence
b'\u0061'     // illegal: \u escapes not allowed in byte literals
b'\U00000061' // illegal: \U escapes not allowed in byte literals
b'\x1'        // illegal: too few hexadecimal digits
b'\256'       // illegal: octal value over 255
```

### Byte string literals

A byte string literal represents a constant obtained from concatenating a sequence of bytes. A byte string literal is a character sequence preceded by `b` and enclosed in double quotes, as in `b"foo"`. Within the quotes, any character may appear except newline and unescaped double quote. The text between the quotes forms the value of the literal, with backslash escapes interpreted as they are in rune literals (except that `\'` and `\u`/`\U` escapes are illegal), with the same restrictions. The three-digit octal (`\nnn`) and two-digit hexadecimal (`\xnn`) escapes represent individual bytes of the resulting byte string.

```ebnf
ByteStringLit = "b" `"` { UnicodeChar | ByteValue | EscapedChar } `"` .
```

```text
b"abc"
b"\n"
b"\""
b"Hello, world!\n"
b"\xff\x00\x7f"
b"\377"

b"\uD800"            // illegal: \u escapes not allowed in byte string literals
b"\U00110000"        // illegal: \U escapes not allowed in byte string literals
b"\u0061"            // illegal: \u escapes not allowed in byte string literals
```

These examples all represent the same byte string:

```text
b"hello"
b"\x68\x65\x6c\x6c\x6f"
```

## Constants

There are *boolean constants*, *rune constants*, *integer constants*, *floating-point constants*, *string constants*, *byte constants*, and *byte string constants*. Rune, integer, floating-point, and byte constants are collectively called *numeric constants*.

A constant value is represented by a [rune](#rune-literals), [integer](#integer-literals), [floating-point](#floating-point-literals), [string](#string-literals), [byte](#byte-literals), or [byte string](#byte-string-literals) literal, an identifier denoting a constant, a constant expression, or a conversion with a result that is a constant. The boolean truth values are represented by the predeclared constants `true` and `false`.

Rune constants are 32-bit signed values representing Unicode code points from 0 to 0x10FFFF. The signed type allows sentinel values such as -1 at runtime. Integer constants are 64-bit signed values. Floating-point constants are IEEE 754 double-precision (64-bit) values. Byte constants are unsigned 8-bit values. String constants are immutable sequences of Unicode code points encoded as UTF-8. Byte string constants are immutable sequences of arbitrary bytes.

A constant may have an explicitly declared type, or its type is inferred from the initializer. It is an error if the constant value cannot be represented as a value of the respective type.

Untyped numeric constants assume a default type when a type is required: `int` for integer constants, `float` for floating-point constants, `rune` for rune constants, and `byte` for byte constants.
