# Alice Language Specification <!-- omit from toc -->

- [Introduction](#introduction)
- [1. Syntax and grammar](#1-syntax-and-grammar)
  - [1.1 Notation](#11-notation)
  - [1.2 Lexical grammar](#12-lexical-grammar)
    - [1.2.1 Whitespace and comments](#121-whitespace-and-comments)
    - [1.2.2 Keywords and operators](#122-keywords-and-operators)
    - [1.2.3 Literals](#123-literals)
    - [1.2.4 Identifiers](#124-identifiers)
    - [1.2.5 Tokens](#125-tokens)
  - [1.3 Syntax grammar](#13-syntax-grammar)

## Introduction

## 1. Syntax and grammar

### 1.1 Notation

TBD

### 1.2 Lexical grammar

#### 1.2.1 Whitespace and comments

```ebnf
LF           = /* the Unicode code point Line Feed U+000A */
CR           = /* the Unicode code point Carriage Return U+000D */
NL           = LF | (CR [LF])
BlockComment = "/*" { BlockComment | <any character> } "*/"
LineComment  = "//" { <any character except NL> }
Whitespace   = <one of the following characters: SPACE U+0020, TAB U+0009, Form Feed U+000C>
```

#### 1.2.2 Keywords and operators

```ebnf
Eq       = "="
Lt       = "<"
Gt       = ">"
Excl     = "!"
Plus     = "+"
Minus    = "-"
Star     = "*"
Slash    = "/"
Dot      = "."
Comma    = ","
Semi     = ";"
Colon    = ":"
Hash     = "#"
Quest    = "?"
LParen   = "("
RParen   = ")"

EqEq       = "=="
Ne         = "!="
Le         = "<="
Ge         = ">="
ColonColon = "::"

Id        = "id"
Prop      = "prop"
Query     = "query"
Include   = "include"
Namespace = "namespace"
Const     = "const"
And       = "and"
Or        = "or"
Using     = "using"
Not       = "not"
```

#### 1.2.3 Literals

TBD

#### 1.2.4 Identifiers

TBD

#### 1.2.5 Tokens

TBD

### 1.3 Syntax grammar

The grammar below replaces some lexical grammar rules with explicit literals (where such replacement in trivial and always correct, for example, for keywords) for better readability.

```ebnf
alice_script = { top_level_object } Eof

top_level_object = top_level_stmt | top_level_decl

top_level_stmt   = using_namespace
top_level_decl   = namespace_decl | prop_decl | sys_decl | const_decl

using_namespace  = "using" "namespace" ident_path

namespace_decl = "namespace" ident_path NL namespace_scope
namespace_scope = Indent { top_level_object } Dedent

prop_decl = "prop" ident [prop_body]
prop_body = tuple_prop_body | classic_prop_body | enum_prop_body
tuple_prop_body = "(" [ ident { "," ident } ] ")"
classic_prop_body = ":" NL Indent { field_decl } Dedent
enum_prop_body = "=" enum_ctor_decl { "|" enum_ctor_decl }

ident_path = ["::"] ident { "::" ident }
ident = SoftKeyword | Ident
```
