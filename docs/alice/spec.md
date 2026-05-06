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
- [Constants](#constants)

## Introduction

This is the reference manual for the Alice programming language.

Alice is a domain specific language (a.k.a *DSL*) designed for game and realtime simulations development. It is based on the Entity-Component-System (a.k.a *ECS*) design pattern. Programs are constructed from unique "things" called **Entities** that are assigned groups of **Props** (a.k.a *components* in other ECS implementations), which are then processed using pipelines of **Queries**.

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

Block comments can be recursive, so a sequence like `/* /* */` is not valid.

A block comment containing no newlines acts like a space. Any other comment acts like a newline.

### Tokens

Tokens form the vocabulary of the Alice language. There are four classes: identifiers, keywords, operators and punctuation, and literals. White space, formed from spaces (U+0020), horizontal tabs (U+0009), carriage returns (U+000D), and newlines (U+000A), is ignored except as it separates tokens that would otherwise combine into a single token. Also, a newline or end of file may trigger the insertion of a semicolon. While breaking the input into tokens, the next token is the longest sequence of characters that form a valid token.

### Semicolons

The formal syntax uses semicolons `;` as terminators in a number of productions. Alice programs may omit most of these semicolons using the following two rules:

1. When the input is broken into tokens, a semicolon is automatically inserted into the token stream immediately after a line's final token if that token is
    - an identifier
    - an integer or floating-point literal
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
+   &   ==    !=    (   )   ::
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

## Constants

There are *boolean constants*, *integer constants*, and *floating-point constants*. Integer and floating-point constants are collectively called *numeric constants*.

A constant value is represented by an [integer](#integer-literals) or [floating-point](#floating-point-literals) literal, an identifier denoting a constant, a constant expression, or a conversion with a result that is a constant. The boolean truth values are represented by the predeclared constants `true` and `false`.

Integer constants are 64-bit signed values. Floating-point constants are IEEE 754 double-precision (64-bit) values.

A constant may have an explicitly declared type, or its type is inferred from the initializer. It is an error if the constant value cannot be represented as a value of the respective type.

Untyped numeric constants assume a default type when a type is required: `int` for integer constants and `float` for floating-point constants.
