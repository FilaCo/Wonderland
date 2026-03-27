#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Token<'a> {
    Whitespace,
    /// `LF | (CR [LF])`
    NewLine,
    Indent,
    Dedent,

    /// A literal constant value, e.g. `123` or `"hello"`.
    Literal {
        kind: LiteralKind,
        symbol: &'a str,
    },

    /// An identifier or keyword, e.g. `ident` or `prop`.
    Ident {
        symbol: &'a str,
    },

    /// `=`
    Eq,
    /// `<`
    Lt,
    /// `>`
    Gt,
    /// `!`
    Excl,
    /// `+`
    Plus,
    /// `-`
    Minus,
    /// `*`
    Star,
    /// `/`
    Slash,
    /// `.`
    Dot,
    /// `,`
    Comma,
    /// `;`
    Semi,
    /// `:`
    Colon,
    /// `#`
    Hash,
    /// `?`
    Quest,

    /// `(`
    LParen,
    /// `)`
    RParen,

    /// `==`
    EqEq,
    /// `!=`
    Ne,
    /// `<=`
    Le,
    /// `>=`
    Ge,
    /// `::`
    ColonColon,

    Error,

    /// End of input.
    Eoi,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum LiteralKind {
    Int { base: Base },
    Float { base: Base },
    Rune { terminated: bool },
    Str { terminated: bool },
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Base {
    Bin = 2,
    Oct = 8,
    Dec = 10,
    Hex = 16,
}
